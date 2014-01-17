#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "exchangequery.h"

namespace Ui {
    class MainWindow;
}

class RecentOrdersWidget;

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

	void exchange_connected();
	void exchange_connection_error(QNetworkReply::NetworkError, const QString&);
	void exchange_markets_updated();

signals:

private:
	ExchangeQuery _exchangeQuery;
	std::unordered_map<int, RecentOrdersWidget*> _recentOrdersWidgets;
};

#endif // MAINWINDOW_H
