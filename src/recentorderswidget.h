#ifndef recentorderswidget_h__
#define recentorderswidget_h__

#include <QWidget>
#include "exchangequery.h"

namespace Ui {
	class RecentOrdersWidget;

}
class RecentOrdersWidget : public QWidget
{
	Q_OBJECT
		;
public:
	RecentOrdersWidget(QWidget* parent = 0);
	~RecentOrdersWidget();

	void set_orders(const BuyOrderSet& buyOrders, const SellOrderSet& sellOrders);
	void set_recent(const std::vector<Trade>& trades);

private:
	Ui::RecentOrdersWidget* ui;
};

#endif // recentorderswidget_h__
