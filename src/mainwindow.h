#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "exchangequery.h"

namespace Ui {
    class MainWindow;
}

class RecentOrdersWidget;

#include <QMovie>
#include <QToolButton>
#include <QLabel>
#include <QIcon>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void readSettings();
    
private slots:
    void about();
    void writeSettings();

	void exchange_markets_updated();

private:
	struct ConnectionState { enum type {
		ConnectionOff,
		ConnectionOk,
		ConnectionError,
		ConnectionWorking
	};};

	void animate_icon();
	void set_connection_state(ConnectionState::type state);

	
signals:

private:
	ExchangeQuery _exchangeQuery;
	std::unordered_map<int, RecentOrdersWidget*> _recentOrdersWidgets;
	QMovie* _connectionIconAnim;
	QToolButton* _connectionButton;
	QLabel* _statusLabel;
	QIcon _connectionErrorIcon, _connectionOffIcon, _connectionOkIcon;
	ConnectionState::type _state;
	QTimer _updateTimer;
};

#endif // MAINWINDOW_H
