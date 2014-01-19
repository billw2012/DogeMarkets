
#include "exchangequery.h"

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QFile>

#include <fstream>

/*
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestReceived(QNetworkReply*)));
manager->get(QNetworkRequest(QUrl("http://google.com")));
    readSettings();
	*/

//class FindMarketsThread : public QThread
//{
//Q_OBJECT
//	;
//
//	void run() Q_DECL_OVERRIDE 
//	{
//		
//		emit resultReady(result);
//	}
//
//signals:
//	void resultReady(const QString &s);
//};

struct Exchange
{
	QUrl marketDataUrl;

	struct ExchangeErrorCodes { enum type {
		NoError,
		RawDataInvalid,
		ServerReturnedFailure,
	};};
	virtual ExchangeErrorCodes::type parse_market_list(const QByteArray& data, std::unordered_map<int, QString>& foundMarkets) = 0;
	virtual QUrl get_market_url(int marketCode) = 0;
	virtual ExchangeErrorCodes::type parse_market(const QByteArray& data, Market& market) = 0;
};

struct CryptsyExchange : public Exchange
{
	CryptsyExchange()
	{
		marketDataUrl = QUrl("http://pubapi.cryptsy.com/api.php?method=marketdatav2");
	}

	virtual ExchangeErrorCodes::type parse_market_list(const QByteArray& data, std::unordered_map<int, QString>& foundMarkets)
	{
		QFile file("markets.log");
		file.open(QFile::WriteOnly);
		file.write(data);
		file.close();

		const QJsonDocument doc = QJsonDocument::fromJson(data);
		
		if(!doc.isObject())
			return ExchangeErrorCodes::RawDataInvalid;
		const QJsonObject docObj = doc.object();
		const QJsonValue success = docObj["success"];
		if(success == QJsonValue::Undefined)
			return ExchangeErrorCodes::RawDataInvalid;
		if(success.toInt() != 1)
			return ExchangeErrorCodes::ServerReturnedFailure;

		// success == 1, so for now we assume data is correct from this point on (risky, maybe API will change...)
		const QJsonObject markets = docObj["return"].toObject()["markets"].toObject();
		//QStringList keys = markets.keys();
		//std::ofstream keyFile("marketkeys.log");
		//for(auto itr = keys.begin(); itr != keys.end(); ++itr)
		//{
		//	keyFile << itr->toLocal8Bit().data() << std::endl;
		//}

		const QJsonObject dogeLtc = markets["DOGE/LTC"].toObject();
		const QJsonObject dogeBtc = markets["DOGE/BTC"].toObject();
		const QJsonObject ltcBtc = markets["LTC/BTC"].toObject();
		
		foundMarkets[dogeLtc["marketid"].toString("-1").toInt()] = "DOGE/LTC";
		foundMarkets[dogeBtc["marketid"].toString("-1").toInt()] = "DOGE/BTC";
		foundMarkets[ltcBtc["marketid"].toString("-1").toInt()] = "LTC/BTC";

		return ExchangeErrorCodes::NoError;
	}

	virtual QUrl get_market_url( int marketCode ) 
	{
		return QUrl(QString("http://pubapi.cryptsy.com/api.php?method=singlemarketdata&marketid=%1").arg(marketCode));
	}

	virtual ExchangeErrorCodes::type parse_market( const QByteArray& data, Market& market ) 
	{
		QFile file("markets.log");
		file.open(QFile::WriteOnly);
		file.write(data);
		file.close();

		const QJsonDocument doc = QJsonDocument::fromJson(data);

		if(!doc.isObject())
			return ExchangeErrorCodes::RawDataInvalid;
		const QJsonObject docObj = doc.object();
		const QJsonValue success = docObj["success"];
		if(success == QJsonValue::Undefined)
			return ExchangeErrorCodes::RawDataInvalid;
		if(success.toInt() != 1)
			return ExchangeErrorCodes::ServerReturnedFailure;

		const QJsonObject markets = docObj["return"].toObject()["markets"].toObject();
		const QJsonObject marketDetails = markets.begin().value().toObject();
		
		market.marketID = marketDetails["marketid"].toString("-1").toInt();
		market.label = marketDetails["label"].toString();
		market.volume = marketDetails["volume"].toString();
		market.primaryName = marketDetails["primaryname"].toString();
		market.primaryCode = marketDetails["primarycode"].toString();
		market.secondaryName = marketDetails["secondaryname"].toString();
		market.secondaryCode = marketDetails["secondarycode"].toString();

		const QJsonArray recentTrades = marketDetails["recenttrades"].toArray();
		market.recentTrades.resize(recentTrades.count());
		for(int recentTradeIdx = 0; recentTradeIdx < recentTrades.count(); ++recentTradeIdx)
		{
			const QJsonObject recentTrade = recentTrades.at(recentTradeIdx).toObject();
			Trade& newTrade = market.recentTrades[recentTradeIdx];
			newTrade.id = recentTrade["id"].toString("-1").toInt();
			newTrade.time = QDateTime::fromString(recentTrade["time"].toString(), QString("yyyy-MM-dd HH:mm:ss"));
			newTrade.price = recentTrade["price"].toString("-1").toDouble();
			newTrade.quantity = recentTrade["quantity"].toString("-1").toDouble();
			newTrade.total = recentTrade["total"].toString("-1").toDouble();
		}

		auto parseOrders = [&](const QJsonArray orderArray, std::vector<Order>& orders)
		{
			orders.resize(orderArray.count());
			for(int orderIdx = 0; orderIdx < orderArray.count(); ++orderIdx)
			{
				const QJsonObject order = orderArray.at(orderIdx).toObject();
				Order& newOrder = orders[orderIdx];
				newOrder.price = order["price"].toString("-1").toDouble();
				newOrder.quantity = order["quantity"].toString("-1").toDouble();
				newOrder.total = order["total"].toString("-1").toDouble();
			}
		};

		parseOrders(marketDetails["sellorders"].toArray(), market.sellOrders);
		parseOrders(marketDetails["buyorders"].toArray(), market.buyOrders);

		return ExchangeErrorCodes::NoError;
	}


};

ExchangeQuery::ExchangeQuery()
	: _networkManager(new QNetworkAccessManager(this)),
	_marketsToUpdate(0)
{
	_exchange = new CryptsyExchange();

	//connect(_networkManager, &QNetworkAccessManager::finished, this, &ExchangeQuery::network_request_finished);
}

void ExchangeQuery::connect_to_exchange()
{
	emit connection_waiting();
	QNetworkReply* reply = _networkManager->get(QNetworkRequest(_exchange->marketDataUrl));
	connect(reply, &QNetworkReply::finished, [=]() {
		if(reply->error() != QNetworkReply::NoError)
		{
			emit connection_error(reply->error(), reply->errorString());
			return ;
		}
		if(reply->url() == _exchange->marketDataUrl)
		{
			std::unordered_map<int, QString> foundMarkets;
			Exchange::ExchangeErrorCodes::type error = _exchange->parse_market_list(reply->readAll(), foundMarkets);
			if(error != Exchange::ExchangeErrorCodes::NoError)
			{
				emit data_error();
			}
			else
			{
				for(auto itr = foundMarkets.begin(); itr != foundMarkets.end(); ++itr)
				{
					_markets.insert(std::make_pair(itr->first, Market()));
				}
				emit connection_ok();
				//update_markets();
			}
		}
	});
}

bool ExchangeQuery::update_markets()
{
	if(_markets.empty() || _marketsToUpdate != 0)
		return false;

	_marketsToUpdate = _markets.size();

	emit connection_waiting();

	for(auto marketItr = _markets.begin(); marketItr != _markets.end(); ++marketItr)
	{
		//connect(_networkManager, &QNetworkAccessManager::finished, this, &ExchangeQuery::network_request_finished);
		QNetworkReply* reply = _networkManager->get(QNetworkRequest(_exchange->get_market_url(marketItr->first)));
		connect(reply, &QNetworkReply::finished, [=]() {
			-- _marketsToUpdate;
			if(reply->error() != QNetworkReply::NoError)
			{
				emit connection_error(reply->error(), reply->errorString());
				return ;
			}
			_exchange->parse_market(reply->readAll(), marketItr->second);
			reply->deleteLater();
			if(_marketsToUpdate == 0)
			{
				emit markets_updated();
			}
		});
	}
	return true;
}