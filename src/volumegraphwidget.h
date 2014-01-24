#ifndef volumegraphwidget_h__
#define volumegraphwidget_h__

#include <QGraphicsView>

#include "markettypes.h"

struct Order;

class VolumeGraphWidget : public QGraphicsView
{
	Q_OBJECT 
		;
public:
	VolumeGraphWidget(QWidget* parent = 0);

	void set_orders(const BuyOrderSet& buyOrders, const SellOrderSet& sellOrders);

protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
	void resize_scene();

protected slots:
	
signals:

private:
	QGraphicsScene* _scene;
};

#endif // volumegraphwidget_h__
