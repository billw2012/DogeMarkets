
#include "volumegraphwidget.h"
#include "markettypes.h"

#include <QRectF>
#include <QGraphicsRectItem>

VolumeGraphWidget::VolumeGraphWidget( QWidget* parent /*= 0*/ )
	: QGraphicsView(parent)
{
	_scene = new QGraphicsScene();
	setScene(_scene);
}

void VolumeGraphWidget::set_orders( const BuyOrderSet& buyOrders, const SellOrderSet& sellOrders )
{
	_scene->clear();

	double maxQuantity = 0, minPrice = std::numeric_limits<double>::max(), maxPrice = 0;
	for(auto itr = buyOrders.begin(); itr != buyOrders.end(); ++itr)
	{
		maxQuantity = std::max(maxQuantity, itr->quantity);
		minPrice = std::min(minPrice, itr->price);
	}
	for(auto itr = sellOrders.begin(); itr != sellOrders.end(); ++itr)
	{
		maxQuantity = std::max(maxQuantity, itr->quantity);
		maxPrice = std::max(maxPrice, itr->price);
	}

	double priceRange = maxPrice - minPrice;

	//float x = 1;
	int idx = 0;
	for(auto itr = buyOrders.rbegin(); itr != buyOrders.rend(); ++itr, ++idx)
	{	
		QPen pen(QColor(30, 127 + idx * (127.0f / (float)buyOrders.size()), 30, 100));
		pen.setCosmetic(true);
		pen.setWidth(10);
		_scene->addLine(maxPrice - itr->price, maxQuantity - itr->quantity, maxPrice - itr->price, maxQuantity, pen);
	}
	idx = 0;
	for(auto itr = sellOrders.begin(); itr != sellOrders.end(); ++itr, ++idx)
	{
		QPen pen(QColor(255 - idx * (127.0f / (float)sellOrders.size()), 30, 30, 100));
		pen.setCosmetic(true);
		pen.setWidth(10);
		_scene->addLine(maxPrice - itr->price, maxQuantity - itr->quantity, maxPrice - itr->price, maxQuantity, pen);
	}
	//setSceneRect(0, 0, buyOrders.size() + sellOrders.size() + 1, maxQuantity);
	setSceneRect(0, 0, priceRange, maxQuantity);
	resize_scene();
}

void VolumeGraphWidget::resizeEvent(QResizeEvent * event)
{
	resize_scene();
}

void VolumeGraphWidget::resize_scene()
{
	fitInView(sceneRect());
}
