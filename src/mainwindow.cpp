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

	_connectionErrorIcon = QIcon("../data/icons/connection_error.png");
	_connectionOffIcon = QIcon("../data/icons/connection_off.png");
	_connectionOkIcon = QIcon("../data/icons/connection_ok.png");
	_connectionIconAnim = new QMovie("../data/icons/processing.gif");

	_statusLabel = new QLabel();
	ui->statusBar->addWidget(_statusLabel);
	
	_connectionButton = new QToolButton();
	_connectionButton->setEnabled(false);
	ui->statusBar->addPermanentWidget(_connectionButton);
	set_connection_state(ConnectionState::ConnectionOff);

	_updateTimer.setSingleShot(false);
	_updateTimer.setInterval(10000);

	connect(&_updateTimer, &QTimer::timeout, [&]() {
		_exchangeQuery.update_markets();
	});

	connect(&_exchangeQuery, &ExchangeQuery::connection_ok, [&](){
		_connectionButton->setEnabled(true);
		set_connection_state(ConnectionState::ConnectionOk);
		_updateTimer.start();
		_exchangeQuery.update_markets();
	});
	connect(&_exchangeQuery, &ExchangeQuery::connection_waiting, [&](){
		set_connection_state(ConnectionState::ConnectionWorking);
	});
	connect(&_exchangeQuery, &ExchangeQuery::connection_error, [&](QNetworkReply::NetworkError, const QString& errorStr){
		_statusLabel->setText(QString("Connection error: ") + errorStr);
		set_connection_state(ConnectionState::ConnectionError);
	});
	connect(&_exchangeQuery, &ExchangeQuery::data_error, [&](){
		_statusLabel->setText(QString("Error in returned data, did the API change?"));
		set_connection_state(ConnectionState::ConnectionError);
	});
	connect(&_exchangeQuery, &ExchangeQuery::markets_updated, this, &MainWindow::exchange_markets_updated);

	connect(_connectionButton, &QToolButton::pressed, [&]() {
		_exchangeQuery.update_markets();
	});

	_exchangeQuery.connect_to_exchange();

	//_connectionButton->setIcon(_connectionOffIcon);
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
	set_connection_state(ConnectionState::ConnectionOk);
}

void MainWindow::animate_icon()
{
	_connectionButton->setIcon(QIcon(_connectionIconAnim->currentPixmap()));
}

void MainWindow::set_connection_state( ConnectionState::type state )
{
	if(_state == ConnectionState::ConnectionWorking)
	{
		_connectionIconAnim->stop();
		disconnect(_connectionIconAnim, &QMovie::frameChanged, this, &MainWindow::animate_icon);
	}
	_state = state;
	switch(_state)
	{
	case ConnectionState::ConnectionWorking:
		connect(_connectionIconAnim, &QMovie::frameChanged, this, &MainWindow::animate_icon);
		_connectionIconAnim->start();
		break;
	case ConnectionState::ConnectionError:
		_connectionButton->setIcon(_connectionErrorIcon);
		break;
	case ConnectionState::ConnectionOff:
		_connectionButton->setIcon(_connectionOffIcon);
		break;
	case ConnectionState::ConnectionOk:
		_connectionButton->setIcon(_connectionOkIcon);
		break;
	};

}
