#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    qDebug() << "QCoreApplication::applicationPid()" << QCoreApplication::applicationPid();
#ifdef WIN32
    SetSystem("use_window_thread", "true");
#elif defined(__linux__) && (defined(__i386__) || defined(__x86_64__))
    XInitThreads();

    // file was stored with local-8-bit encoding
    //   -> set the interface encoding accordingly
    SetHcppInterfaceStringEncodingIsUtf8(false);
#endif

    QApplication a(argc, argv);

    QPixmap pixmap(":/timg.png");
    QSplashScreen splash(pixmap.scaled(QSize(200,200),Qt::KeepAspectRatio));
    splash.show();
    a.processEvents();

    MainWindow w;
    w.show();
#ifdef WIN32
    w.HalconInit();
#elif __linux__

#endif
    w.updateParamsWidget();       // 初始化参数控件中的参数
    splash.finish(&w);

    // Test code
    hImageSrc1.ReadImage("C:/Users/12257/Desktop/calibration/left/imgA181719.png");
    hImageSrc2.ReadImage("C:/Users/12257/Desktop/calibration/right/imgB181719.png");

    return a.exec();
}

