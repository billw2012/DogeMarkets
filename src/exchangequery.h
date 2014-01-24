#ifndef exchangequery_h__
#define exchangequery_h__

#include <unordered_map>
#include <QObject>
#include <QNetworkReply>

#include "markettypes.h"

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
	void connect_to_exchange();
	bool update_markets();

	MarketsMap get_markets() const { return _markets; }

protected slots:

signals:
	void connection_ok();
	void connection_waiting();
	void connection_error(QNetworkReply::NetworkError, const QString&);
	void data_error();
	void markets_updated();

private:
	MarketsMap _markets;
	QNetworkAccessManager* _networkManager;
	Exchange* _exchange;
	int _marketsToUpdate;
};

#endif // exchangequery_h__
