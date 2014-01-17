#include "recentorderswidget.h"
#include "ui_recentorderswidget.h"

RecentOrdersWidget::RecentOrdersWidget( QWidget* parent /*= 0*/ )
	: QWidget(parent),
	ui(new Ui::RecentOrdersWidget)
{
	ui->setupUi(this);
}

RecentOrdersWidget::~RecentOrdersWidget()
{

}

void updateOrders(QTreeWidget* widget, const std::vector<Order>& orders)
{
	widget->setUpdatesEnabled(false);
	widget->clear();

	for(auto itr = orders.begin(); itr != orders.end(); ++itr)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, QString("%1").arg(itr->price, 0, 'f', 8));
		item->setText(1, QString("%1").arg(itr->quantity, 0, 'f', 4));
		item->setText(2, QString("%1").arg(itr->total, 0, 'f', 8));
		widget->addTopLevelItem(item);
	}

	widget->setUpdatesEnabled(true);
}

void RecentOrdersWidget::set_orders( const std::vector<Order>& buyOrders, const std::vector<Order>& sellOrders )
{
	updateOrders(ui->buyTreeWidget, buyOrders);
	updateOrders(ui->sellTreeWidget, sellOrders);
}

void RecentOrdersWidget::set_recent( const std::vector<Trade>& trades )
{
	ui->recentTreeWidget->setUpdatesEnabled(false);
	ui->recentTreeWidget->clear();
	for(auto itr = trades.begin(); itr != trades.end(); ++itr)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, itr->time.toString());
		//item->setText(1, QString("%1").arg(itr->));
		item->setText(2, QString("%1").arg(itr->price, 0, 'f', 8));
		item->setText(3, QString("%1").arg(itr->quantity, 0, 'f', 4));
		item->setText(4, QString("%1").arg(itr->total, 0, 'f', 8));
		ui->recentTreeWidget->addTopLevelItem(item);
	}
	ui->recentTreeWidget->setUpdatesEnabled(true);
}

