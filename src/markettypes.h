#ifndef markettypes_h__
#define markettypes_h__

#include <vector>
#include <set>
#include <functional>

#include <QString>
#include <QDateTime>

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

	Order() : price(0), quantity(0), total(0) {}
};

struct BuyOrderCompare : public std::binary_function<Order, Order, bool>
{
	bool operator()(const Order& lhs, const Order& rhs) const
	{
		return lhs.price > rhs.price;
	}
};

struct SellOrderCompare : public std::binary_function<Order, Order, bool>
{
	bool operator()(const Order& lhs, const Order& rhs) const
	{
		return lhs.price < rhs.price;
	}
};

typedef std::set<Order, BuyOrderCompare> BuyOrderSet; 
typedef std::set<Order, SellOrderCompare> SellOrderSet;

struct Market
{
	int marketID;
	QString label;
	QString volume;
	QString primaryName, primaryCode;
	QString secondaryName, secondaryCode;

	std::vector<Trade> recentTrades;

	BuyOrderSet buyOrders;
	SellOrderSet sellOrders;
};

#endif // markettypes_h__
