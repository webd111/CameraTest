#include "WebCamera.h"

WebCamera::WebCamera(WCameraParams _params, QWidget*)
{
    qDebug() << "WebCamera::WebCamera()";
    cameraIndex = _params.cameraIndex;
    cameraInterface = _params.cameraInterface;

    hei = _params.height*2;
    wid = _params.width;
    data_length = _params.data_length;
    channels = _params.channels;
    if(channels == 1)
        img = Mat::zeros(hei, wid, CV_8UC1);
    else if (channels == 3)
        img = Mat::zeros(hei, wid, CV_8UC3);

#ifdef WIN32
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsa) != 0)
    {
        // Exception
        perror("WSAStartup");
    }
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock == INVALID_SOCKET)
    {
        // Exception
        perror("socket");
    }
    unsigned long ul = 1;           // Allow block mode, 0 to forbid it
    int ret = ioctlsocket(sock, long(FIONBIO), &ul);
#elif __linux__
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock == -1)
    {
        // Exception
        perror("socket");
    }
    unsigned long ul = 1;           // Allow block mode, 0 to forbid it
    int ret = ioctl(sock, long(FIONBIO), &ul);
#endif
    if(ret == -1)
    {
        // Exception
        perror("ioctlsocket");
    }

    int nRecvBuf = 32*1024;//设置为32K
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));      // 设置接受缓冲区大小
    int nZero=0;
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero));     // 在发送数据的时，不经历由系统缓冲区到socket缓冲区的拷贝

    // Server address i.e. camera address
    addr_server.sin_family = AF_INET;                       //使用AF_INET协议族
    addr_server.sin_port = htons(_params.port_server);                     //设置地址结构体中的端口号
    addr_server.sin_addr.s_addr = inet_addr(_params.ip_server.c_str());   //设置通信ip

    // Client address i.e. controller address
    addr_client.sin_family = AF_INET;                       //使用AF_INET协议族
    addr_client.sin_port = htons(_params.port_client);                     //设置地址结构体中的端口号
    addr_client.sin_addr.s_addr = inet_addr(_params.ip_client.c_str());   //设置通信ip
    //将套接字地址与所创建的套接字号联系起来，并检测是否绑定成功
    socklen_t addrlen = sizeof(struct sockaddr);
    int res = ::bind(sock,(struct sockaddr*)&addr_client, addrlen);
    if(res == -1)
    {
        // Exception
        perror("bind");
    }    
}

WebCamera::~WebCamera()
{
    qDebug() << "WebCamera::~WebCamera()";
#ifdef WIN32
    closesocket(sock);
    WSACleanup();
#endif
}

bool WebCamera::getImage(Mat& _img)
{
    if(isGrabbed)
    {
        QMutexLocker locker(&m_image);
        isGrabbed = false;
        image.copyTo(_img);
//        cvtColor(_img, _img, COLOR_RGB2BGR);
        locker.unlock();
        return true;       // Object has been copied
    }
    else
        return false;
}

bool WebCamera::sendCommand(char cmd)
{
    char packet_command[1];
    packet_command[0] = cmd;   // 捕捉开始

    //Step5：与客户端进行通信
    // 发一个数据包返回给客户
    //sendto参数：socket名称，发送数据的缓冲区，缓冲区大小，标志位（调用操作方式），sockaddr结构地址，sockaddr结构大小地址
    int ret = sendto(sock, packet_command, 1, 0, (sockaddr*)&addr_server, sizeof(sockaddr));
//    qDebug() <<  qPrintable(QString("Send message to [%1::%2]").arg(inet_ntoa(addr_server.sin_addr)).arg(ntohs(addr_server.sin_port)));
    return ret != -1;
}

bool WebCamera::grabSocket()
{
    memset(buf, 0, data_length+8);
#ifdef WIN32
    int serverAddrLen = sizeof(sockaddr);
    int ret = recvfrom(sock, buf, data_length+8, 0, (sockaddr*)&addr_server, &serverAddrLen);
#elif __linux__
    socklen_t serverAddrLen = sizeof(sockaddr);
    int ret = recvfrom(sock, buf, data_length+8, 0, (sockaddr*)&addr_server, &serverAddrLen);
#endif
    if(ret == -1)
    {
//        qDebug() << "ret = " << ret;
        return false;
    }

    // 解包
    Packet* p_data = (Packet*)buf;

//    qDebug() << "p_data->pos =" << p_data->pos;
//    qDebug() << "p_data->packet_num =" << p_data->num;
//    qDebug() << "p_data->length =" << p_data->length;
//    qDebug()<<hei<<","<<wid<<","<<channels<<endl;
    memcpy(img.data + p_data->pos, p_data->data, p_data->length);

    //
    if(int(p_data->pos + p_data->length) >= (hei * wid * channels - 1))
    {
//            qDebug()<<"here"<<endl;
        QMutexLocker locker(&m_image);
        img.copyTo(image);
//        qDebug()<<hei<<","<<wid<<","<<channels<<endl;
//        qDebug() << "img.rows = " << img.rows << "img.cols = " << img.cols;
        isGrabbed = true;
        locker.unlock();
    }
    return true;
}

WCameraParams WebCamera::getWParams()
{
    WCameraParams p;
    p.ip_client = inet_ntoa(addr_client.sin_addr);
    p.ip_server = inet_ntoa(addr_server.sin_addr);
    p.cameraIndex = cameraIndex;
    p.port_client = ntohs(addr_client.sin_port);
    p.port_server = ntohs(addr_server.sin_port);
    p.cameraInterface = "UserDefined";
    return p;
}

QVector<QString> WebCamera::getAvailableDevice(QString _cameraInterface)
{
    QVector<QString> ip;
    if(_cameraInterface == "UserDefined")
    {
        ip.push_back("127.0.0.1");
        char host[255];
#ifdef WIN32
        if (gethostname(host, sizeof(host)) == SOCKET_ERROR)
#elif __linux__
        if (gethostname(host, sizeof(host)) == -1)
#endif
        {
            qDebug() << "Get host name failed.";
        }

        struct hostent *p = gethostbyname(host);

        if (p == nullptr)
        {
            qDebug() << "Get ip failed.";
        }
        else
        {
            //本机IP:利用循环,输出本机所有IP
            for (int i = 0; p->h_addr_list[i] != nullptr; ++i)
            {
                struct in_addr in;
                memcpy(&in, p->h_addr_list[i], sizeof(struct in_addr));
                ip.push_back(inet_ntoa(in));
    //            cout << "第" << i + 1 << "块网卡的IP为:" << inet_ntoa(in) << endl;
            }
        }
    }
    return ip;
}

int WebCamera::imageCheck()
{
    Mat img_temp;
    int cnt = 0;
    QMutexLocker locker(&m_image);
    image.copyTo(img_temp);
    m_image.unlock();
    for(int i = 0; i < img_temp.rows; ++i)
    {
        auto ptr = img_temp.ptr(i);
        auto ptr_std = image_std.ptr(i);
        for(int j = 0; j < img_temp.cols; ++j)
        {
            if(*ptr != *ptr_std)
                cnt++;
            ++ptr;
            ++ptr_std;
        }
    }
    return cnt;
}
