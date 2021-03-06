#include <QApplication>
#include "mainwindow.h"

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    #include <QtWinExtras>
#endif
#endif

#ifdef Q_OS_LINUX
    #include <QtDBus>
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/app/icons/application-%ProjectName%.png"));
    QCoreApplication::setOrganizationName("%Company%");
    QCoreApplication::setOrganizationDomain("%homepage%");
    QCoreApplication::setApplicationName("%ProductName%");
    
    MainWindow window;

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    if (QtWin::isCompositionEnabled()) {
        QtWin::extendFrameIntoClientArea(&window, 0, 0, 0, 5);
        //window.setContentsMargins(0, 0, 0, 5);
    }
#endif
#endif

#ifdef Q_OS_LINUX
    QDBusMessage signal = QDBusMessage::createSignal("/", "com.canonical.Unity.LauncherEntry", "Update");
    signal << "application://%ProductName%.desktop";
    QVariantMap setProperty;
    setProperty.insert("count", qint64(0));
    setProperty.insert("count-visible", false);
    setProperty.insert("progress", double(0.00));
    setProperty.insert("progress-visible", false);
    setProperty.insert("urgent", false);
    signal << setProperty;
    QDBusConnection::sessionBus().send(signal);
#endif
   
    window.show();

    return a.exec();
}
