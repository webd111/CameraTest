#ifndef WEBCAMERA_H
#define WEBCAMERA_H

#include <QWidget>
#include <QDebug>

#include "opencv/myopencv.h"
#include "myvariable.h"

// Using socket
#ifdef WIN32
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
#elif __linux__
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#endif

using namespace std;
using namespace cv;

const int bufsize = 8192;

#pragma pack(1)
// UDP data structure
typedef struct Packet
{
    uint pos;
    ushort num;
    ushort length;
    uchar data[bufsize-8];
}Packet;


typedef struct WCameraParams
{
    int cameraIndex = 0;
    QString cameraInterface = "";
    string ip_server = "";
    string ip_client = "";
    u_short port_server = 0;
    u_short port_client = 47802;
    int height = 640;
    int width = 480;
    int data_length = 0;
    int channels = 1;

    // Not using "delete" in destructor
    WCameraParams(){}
    ~WCameraParams(){}

    // Using deleted function to prevent /*copy*/ and valuation
//    WCameraParams(const WCameraParams&) = delete;
    WCameraParams& operator=(const WCameraParams&) = delete;
}WCameraParams;

class WebCamera : public QWidget
{
    Q_OBJECT

    Mat image;
    Mat image_std;

    // Camera parameters
    int cameraIndex;
    QString cameraInterface;
    sockaddr_in addr_server;
    sockaddr_in addr_client;

#ifdef WIN32
    WSADATA wsa;
    SOCKET sock;
#elif __linux__
    int sock;
#endif

    QMutex m_image;

    int data_length = bufsize - 8;
    char buf[bufsize];

    bool isGrabbed = false;

    int hei = 1080;
    int wid = 1920;
    int channels = 1;
    Mat img;

public:
    WebCamera(WCameraParams _params, QWidget* parent = nullptr);
    ~WebCamera();

    WCameraParams getWParams();
    bool getImage(Mat& img);             // 获取cv::Mat格式的图像
//    bool openSocketAdapter();
//    bool initSocket();
    bool sendCommand(char cmd);
    bool grabSocket();
    int imageCheck();

    static QVector<QString> getAvailableDevice(QString cameraInterface);

signals:

public slots:
};

#pragma pack()
#endif
