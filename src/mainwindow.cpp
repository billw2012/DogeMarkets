#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QMessageBox>

#include "recentorderswidget.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets>
#endif

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    #include <QtWinExtras>
#endif
#endif

#ifdef Q_OS_MAC
extern void qt_mac_set_dock_menu(QMenu *);
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
 
#ifdef Q_OS_MAC

    /* Dock Menu */
    QMenu *menu = new QMenu;
    // Add actions to the menu

    qt_mac_set_dock_menu(menu);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    setUnifiedTitleAndToolBarOnMac(true);
#endif
#endif

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    /* Jump List */
    //QWinJumpList jumplist;
    //jumplist.begin();
    //Add categories, tasks, items, and links to the menu

    //jumplist.commit();
#endif
#endif
    
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(writeSettings()));

	connect(&_exchangeQuery, &ExchangeQuery::connected, this, &MainWindow::exchange_connected);
	connect(&_exchangeQuery, &ExchangeQuery::connection_error, this, &MainWindow::exchange_connection_error);
	connect(&_exchangeQuery, &ExchangeQuery::markets_updated, this, &MainWindow::exchange_markets_updated);

	ui->actionRefreshMarketData->setEnabled(false);
	connect(ui->actionRefreshMarketData, &QAction::triggered, [&](bool) {
		_exchangeQuery.update_markets();
	});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::about()
{
    QMessageBox::about(this,
                       "About DogeMarkets",
                       QString("<html><b><p>%short_description%</p></b>")
                       + QString("<p>%long_description%</p><br>")
                       + QString("Author: %author% <%email%><br>")
                       + QString("Homepage: %homepage%<br>")
                       + QString("License: %license%<br>")
                       + QString("<b>Copyright &copy; %year%, %Company%</b>")
                       );
}

void MainWindow::writeSettings()
{
    QSettings settings; // To add settings: settings.setValue(QString("key"),QVariant());
}

void MainWindow::readSettings()
{
    QSettings settings;  //To get settings: QVariant thing = settings.value("key").toQVariant();
}

void MainWindow::exchange_connected()
{
	ui->actionRefreshMarketData->setEnabled(true);
}

void MainWindow::exchange_connection_error(QNetworkReply::NetworkError code, const QString& errorStr)
{
	QMessageBox::critical(this, "Connection error", errorStr);
}

void MainWindow::exchange_markets_updated()
{
	ExchangeQuery::MarketsMap marketData = _exchangeQuery.get_markets();

	for(auto marketItr = marketData.begin(); marketItr != marketData.end(); ++marketItr)
	{
		int marketId = marketItr->first;
		const Market& market = marketItr->second;
		auto widgetFItr = _recentOrdersWidgets.find(marketId);
		RecentOrdersWidget* recentOrdersWidget = NULL;
		if(widgetFItr == _recentOrdersWidgets.end())
		{
			recentOrdersWidget = new RecentOrdersWidget();
			ui->marketsTabWidget->addTab(recentOrdersWidget, market.label);
			_recentOrdersWidgets[marketId] = recentOrdersWidget;
		}
		else
		{
			recentOrdersWidget = widgetFItr->second;
		}
		recentOrdersWidget->set_orders(market.buyOrders, market.sellOrders);
		recentOrdersWidget->set_recent(market.recentTrades);
	}
}
