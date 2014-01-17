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

	void set_orders(const std::vector<Order>& buyOrders, const std::vector<Order>& sellOrders);
	void set_recent(const std::vector<Trade>& trades);

private:
	Ui::RecentOrdersWidget* ui;
};

#endif // recentorderswidget_h__
