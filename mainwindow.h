#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QMetaType>
#include <QPointer>
#include <QFileDialog>
#include <QDateTime>
//#include <ActiveQt/QAxObject>
#include <QPoint>
#include <QTime>
#include <QMessageBox>
#include <QTableView>
#include <QStandardItemModel>

#include "myvariable.h"
#include "halcon/myhalcon.h"
#include "opencv/myopencv.h"
#include "opencv/stereocamera.h"
#include "capture/HalconCamera.h"
#include "capture/WebCamera.h"
#include "thread/imgacqthread.h"
#include "thread/depthmappingthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int cameraType = 1;
    int resolution = 1;

    QLabel* labelRunTime;
    QLabel* labelSystemTime;
    QTimer* MainTimer;
    QTime* SystemTime;

    QPointer<ImgAcqThread> imgAcqThread1;
    QPointer<ImgAcqThread> imgAcqThread2;
    QPointer<ImgAcqThread> imgAcqThread3;
    QPointer<DepthMappingThread> depthMappingThread;

    QStandardItemModel *model = new QStandardItemModel();

    StereoCamera* stereoCamera = new StereoCamera;

    QVector<QLabel*> qlabel_ptr;

    void HalconInit();                              // Halcon window initialization
    void HalconDeinit();
    void WidgetInit();                              // Some widgets initialization
    void AlgorithmInit();                           // Algorithm initialize
    void updateParamsWidget();                      //

public slots:
    void updateImage(int);
    void updateStatistic(int, double);          // Update bar chart
    void updateException(int cameraIndex, QString time, HException e);      // Show exception message
    void updateException(QString time, QString e1, HException e2);
    void updateInformation(int cameraIndex, QString* cameraInfo);

private slots:
    void on_actionCamera1Read_triggered();           // Read camera1
    void on_actionCamera2Read_triggered();           // Read camera2
    void on_actionCamera3Read_triggered();           // Read camera3
    void on_actionDepthMapping_triggered();         // Depthmap
    void on_actionMatching_triggered();             // Matching
    void on_pushButtonReadParams_clicked();         // Read params
    void on_pushButtonApplyParams_clicked();

    void MainTimerUpdate();                         // Timer

    void imgAcqThread1Destoryed();
    void imgAcqThread2Destoryed();
    void imgAcqThread3Destoryed();
    void depthMappingThreadDestoryed();

    void on_actionSave_triggered();
    void on_actionRefresh_triggered();
    void on_comboBoxCameraInterface1_currentTextChanged(const QString &arg1);
    void on_comboBoxCameraDevice1_currentTextChanged(const QString &);
    void on_comboBoxCameraPort1_currentTextChanged(const QString &);
    void on_comboBoxCameraInterface2_currentTextChanged(const QString &arg1);
    void on_comboBoxCameraDevice2_currentTextChanged(const QString &);
    void on_comboBoxCameraPort2_currentTextChanged(const QString &);
    void on_comboBoxCameraInterface3_currentTextChanged(const QString &arg1);
    void on_comboBoxCameraDevice3_currentTextChanged(const QString &);
    void on_comboBoxCameraPort3_currentTextChanged(const QString &);

protected:
    void resizeEvent(QResizeEvent*);                // Resize window
    void closeEvent(QCloseEvent*);

private:
    Ui::MainWindow *ui;
};

#endif
