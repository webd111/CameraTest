#ifndef IMGACQTHREAD_H
#define IMGACQTHREAD_H

#include <QDebug>
#include <QThread>
#include <QObject>
#include <QTime>
#include <QPointer>

#include "capture/WebCamera.h"
#include "capture/HalconCamera.h"
#include "highaccuracytimer.h"
#include "opencv/myopencv.h"
#include "myvariable.h"
#include "thread/dispthread.h"

#ifdef WIN32
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#elif __linux__
#include <opencv4/opencv2/calib3d/calib3d.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#endif

class ImageHandler : public QObject
{
    Q_OBJECT

    int mode = 3;

public:
    explicit ImageHandler(int mode);
    ~ImageHandler();

//    QQueue<Mat> img_queue;      // RGB图像
//    QQueue<Mat> imgd_queue;     // 深度图图像
//    QMutex m_queue;

signals:
public slots:
    void getImage(Mat img, Mat depth_img);            //
};

class ImgAcqThread : public QThread
{
    Q_OBJECT

    int cameraIndex = 0;
    QString cameraInterface = "";
    int mode = 3;       // Indicate image spliting mode,
                        // 1 for single view, 2 for double view, 3 for double view + disparity map

    // These pointers should not be deallocated for they point at global variable
    HImage* himage = nullptr;
    HTuple* hwindowhandle = nullptr;
    QMutex* m[3] = {nullptr, nullptr, nullptr};

    // Halcon Camera
    HalconCamera* halconCamera = nullptr;

    // UserDefined Camera
    WebCamera* webCamera = nullptr;

    QThread imgHandleThread;

    ImageHandler* imgHandler = nullptr;

    HighAccuracyTimer timer;

    ~ImgAcqThread();

public:
    ImgAcqThread(HCameraParams _params, int mode = 3);
    ImgAcqThread(WCameraParams _params, int mode = 3);

    QString getInterface();

    void end();

signals:
    void ImgCaptured(int cameraIndex);          // Only used in linux
    void ImgAcqInfo(int cameraIndex, QString* cameraInfo);
    void ImgAcqTime(int cameraIndex, double time);
    void ImgAcqException(int cameraIndex, QString time, HException e);
    void ImgGrabbed();
protected:
    void run();         // override run()
};

#endif
