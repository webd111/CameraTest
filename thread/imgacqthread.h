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


#ifdef WIN32
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#elif __linux__
#include <opencv4/opencv2/calib3d/calib3d.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#endif

class ImgAcqThread : public QThread
{
    Q_OBJECT

    int cameraIndex = 0;
    QString cameraInterface = "";

    // These pointers should not be deallocated for they point at global variable
    HImage* himage = nullptr;
    HTuple* hwindowhandle = nullptr;
    QMutex* m = nullptr;

    // Halcon Camera
    HalconCamera* halconCamera = nullptr;

    // UserDefined Camera
    WebCamera* webCamera = nullptr;

    HighAccuracyTimer timer;

    ~ImgAcqThread();

public:
    ImgAcqThread(HCameraParams _params);
    ImgAcqThread(WCameraParams _params);

    QString getInterface();

    void end();

signals:
    void ImgCaptured(int cameraIndex);          // Only used in linux
    void ImgAcqInfo(int cameraIndex, QString* cameraInfo);
    void ImgAcqTime(int cameraIndex, double time);
    void ImgAcqException(int cameraIndex, QString time, HException e);

protected:
    void run();         // override run()
};

#endif
