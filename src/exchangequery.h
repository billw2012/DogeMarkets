#ifndef exchangequery_h__
#define exchangequery_h__

#include <unordered_map>
#include <QObject>
#include <QDateTime>
#include <QNetworkReply>

struct Trade
{
	int id;
	QDateTime time;
	double price;
	double quantity;
	double total;
	//bool buy; // or sell
};

struct Order
{
	double price;
	double quantity;
	double total;
};


struct Market
{
	int marketID;
	QString label;
	QString volume;
	QString primaryName, primaryCode;
	QString secondaryName, secondaryCode;

	std::vector<Trade> recentTrades;
	std::vector<Order> buyOrders, sellOrders;
};

class QNetworkAccessManager;

struct Exchange;
class ExchangeQuery : public QObject
{
	Q_OBJECT 
		;

public:
	typedef std::unordered_map<int, Market> MarketsMap;

public:
	ExchangeQuery();
	bool update_markets();

	MarketsMap get_markets() const { return _markets; }

protected slots:
	void network_request_finished(QNetworkReply* reply);

signals:
	void connected();
	void connection_error(QNetworkReply::NetworkError, const QString&);
	void markets_updated();


private:
	MarketsMap _markets;
	QNetworkAccessManager* _networkManager;
	Exchange* _exchange;
	int _marketsToUpdate;
};

#endif // exchangequery_h__
