#ifndef HALCONCAMERA_H
#define HALCONCAMERA_H

#include <QDebug>
#include <QObject>
#include <QPointer>

#include "opencv/myopencv.h"
#include "myvariable.h"

#ifdef WIN32
const QString USBCameraInterface = "DirectShow";
#elif __linux__
const QString USBCameraInterface = "Video4Linux2";
#endif

using namespace std;

// This struct have nothing with memory deallocation
typedef struct HCameraParams
{
    int cameraIndex = 0;
    QString cameraInterface = "";
    QString device = "";
    int port = -5;
    QString cameraType = "";
    bool isConnected = false;
}HCameraParams;

struct CameraInfoUSB
{
    int device;
    int port;
    QString cameraType;
};

struct CameraInfoEthernet
{
    QString device;
    QString uniqueName;
    QString interface_;         // "_" for avoiding conflict with "interface"
    QString producer;
    QString deviceIP;
    QString interfaceIP;
};

// This class is used for cameras halcon supports, including USB and GigEVision2
class HalconCamera
{
    HImage himage;                  // Grabbed image
    HTuple hv_AcqHandle = NULL;     // Image Grabber handle
    QMutex m;                       // Thread lock

    static QVector<CameraInfoUSB> cameraInfosetDirectShow;
    static QVector<CameraInfoEthernet> cameraInfosetGigEVision2;
//    QVector<pcap_if_t> cameraInfosetUserDefined;

public:
    HalconCamera(HCameraParams _params);
    ~HalconCamera();

    HCameraParams params;
    HTuple hv_Width, hv_Height;

    bool connect();
    void grab();
    void close();
    HImage getGribbedImage();
    void setParams(HCameraParams _params);
    HCameraParams getParams();

    static void getCameraInfo();                                               // Get info of all camera interfaces
    static QVector<QString> getAvailableDevice(QString cameraInterface);       //
    static QVector<int> getAvailablePort(QString device);                      // Only used in USBCameraInterface interface
    static QVector<QString> getAvailableCameraType(QString device, int port);  // Only used in USBCameraInterface interface
};

#endif
