#include "capture/HalconCamera.h"

QVector<CameraInfoUSB> HalconCamera::cameraInfosetDirectShow(100);
QVector<CameraInfoEthernet> HalconCamera::cameraInfosetGigEVision2(100);

HalconCamera::HalconCamera(HCameraParams _params)
{
    qDebug() << "HalconCamera::HalconCamera()";
    params = _params;
}

HalconCamera::~HalconCamera()
{
    qDebug() << "HalconCamera::~HalconCamera()";
}

bool HalconCamera::connect()
{
    if(params.cameraInterface == USBCameraInterface)
    {
        OpenFramegrabber(HTuple(USBCameraInterface.toStdString().c_str()), 1, 1, 0, 0, 0, 0, "default", 8, "default", -1, "false",
                         HTuple(params.cameraType.toStdString().c_str()), HTuple(params.device.toStdString().c_str()),
                         params.port, -1, &hv_AcqHandle);
    }
    else if (params.cameraInterface == "GigEVision2")
    {
        OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default", -1, "false",
                         "default", HTuple(params.device.toStdString().c_str()), 0, -1, &hv_AcqHandle);
    }
    else
    {
        qDebug() << "HalconCamera::connectCamera() parameters are not valid!";
    }
    if(hv_AcqHandle.Length() != 0)
    {
#ifdef __linux__
        if(params.cameraInterface == USBCameraInterface)
            GrabImageStart(hv_AcqHandle, -1);
#endif
        params.isConnected = true;
        return true;
    }
    params.isConnected = false;
    return false;
}

void HalconCamera::grab()
{
    // Grab Image
    if(params.cameraInterface == USBCameraInterface)
    {
#ifdef WIN32
        GrabImage(&himage, hv_AcqHandle);
#elif __linux__
        GrabImageAsync(&himage, hv_AcqHandle, -1);
#endif
    }
    else
    {
        GrabImageAsync(&himage, hv_AcqHandle, -1);
    }
//    himage.GetImageSize(&hv_Width, &hv_Height);
}

void HalconCamera::close()
{
    if(params.isConnected)
        CloseFramegrabber(hv_AcqHandle);
    params.isConnected = false;
}

HImage HalconCamera::getGribbedImage()
{
    QMutexLocker locker(&m);
    return himage;
}

void HalconCamera::setParams(HCameraParams _params)
{
    QMutexLocker locker(&m);
    params = _params;
    locker.unlock();
}

HCameraParams HalconCamera::getParams()
{
    QMutexLocker locker(&m);
    return params;
}

void HalconCamera::getCameraInfo()
{
    try
    {
        // Get USBCameraInterface info
        HTuple* Information = new HTuple();
        HTuple* ValueList = new HTuple();
        InfoFramegrabber(HTuple(USBCameraInterface.toStdString().c_str()), "info_boards", Information, ValueList);
        cameraInfosetDirectShow.clear();
        QString list;
        int index;
        CameraInfoUSB info1;
        for (int i = 0; i < ValueList->Length(); i++)
        {
            list = ValueList[0][i].S();
            qDebug() << "Camera list:" << list;
#ifdef WIN32
            index = list.indexOf("device:", 0, Qt::CaseSensitive);
            info1.device = QString(list.at(index + 7)).toInt();
            index = list.indexOf("port:", 0, Qt::CaseSensitive);
            info1.port = QString(list.at(index + 5)).toInt();
            index = list.indexOf("camera_type:", 0, Qt::CaseSensitive);
            info1.cameraType = list.mid(index + 12);
#elif __linux__
            index = list.indexOf("device:", 0, Qt::CaseSensitive);
            info1.device = QString(list.at(index + 12)).toInt();
            index = list.indexOf("port:", 0, Qt::CaseSensitive);
            info1.port = QString(list.at(index + 5)).toInt();
            index = list.indexOf("input_name:", 0, Qt::CaseSensitive);
            info1.cameraType = list.mid(index + 11);
#endif
            cameraInfosetDirectShow.push_back(info1);
        }

        // Get "GigEVision2" info
        Information->Clear();
        ValueList->Clear();
        InfoFramegrabber("GigEVision2", "info_boards", Information, ValueList);
        cameraInfosetGigEVision2.clear();
        list.clear();
        index = 0;
        int index2 = 0;
        CameraInfoEthernet info2;
        for (int i = 0; i < ValueList->Length(); i++)
        {
            list = ValueList[0][i].S();
            index = list.indexOf("device:", 0, Qt::CaseSensitive);
            index2 = list.indexOf(" | unique_name:", 0, Qt::CaseSensitive);
            info2.device = list.mid(index + 7, index2 - index - 7);
            index = list.indexOf("unique_name:", 0, Qt::CaseSensitive);
            index2 = list.indexOf(" | interface:", 0, Qt::CaseSensitive);
            info2.uniqueName = list.mid(index + 12, index2 - index - 12);
            index = list.indexOf("interface:", 0, Qt::CaseSensitive);
            index2 = list.indexOf(" | producer:", 0, Qt::CaseSensitive);
            info2.interface_ = list.mid(index + 10, index2 - index - 10);
            index = list.indexOf("producer:", 0, Qt::CaseSensitive);
            index2 = list.indexOf(" | vendor:", 0, Qt::CaseSensitive);
            info2.producer = list.mid(index + 9, index2 - index - 9);
            index = list.indexOf("device_ip:", 0, Qt::CaseSensitive);
            index2 = list.indexOf(" | interface_ip:", 0, Qt::CaseSensitive);
            info2.deviceIP = list.mid(index + 10, index2 - index - 10);
            index = list.indexOf("interface_ip:", 0, Qt::CaseSensitive);
            index2 = list.indexOf(" | status:", 0, Qt::CaseSensitive);
            info2.interfaceIP = list.mid(index + 13, index2 - index - 13);
            cameraInfosetGigEVision2.push_back(info2);
        }

//        // Get "UserDefined" info
//        cameraInfosetUserDefined.clear();
//        CameraInfoUserDefined info;
//        pcap_if_t *d;
//        pcap_if_t *AdapterList;
//        int AdapterNumber = 0;
////        AdapterComboBox->clear();
//        char errbuf[PCAP_ERRBUF_SIZE];
//        if(pcap_findalldevs(&AdapterList, errbuf) == -1)
//        {
//            // Exception handle
//            qDebug() << errbuf;
////            AdapterComboBox->addItem(tr("Search failed!"));
//        }
//        else
//        {
//            for(d = AdapterList; d; d = d->next)
//            {
//                ++AdapterNumber;
//                qDebug() << QString("No: %1").arg(QString::number(AdapterNumber));
//                qDebug() << QString("name: %1").arg(QString::fromLocal8Bit(d->name));
//                qDebug() << QString("descriptor: %1").arg(QString::fromLocal8Bit(d->description));
//                qDebug() << QString("flags: %1").arg(d->flags);

//                // Add adapter items
//                cameraInfosetUserDefined.push_back(*d);
//            }
//        }
    }
    catch (HException &exception)
    {
        fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
                exception.ProcName().Text(),
                exception.ErrorMessage().Text());
    }
}

QVector<QString> HalconCamera::getAvailableDevice(QString cameraInterface)
{
    QVector<QString> deviceList;
    try
    {
        HTuple* Information = new HTuple();
        HTuple* ValueList = new HTuple();
        if(cameraInterface == USBCameraInterface)
        {
            InfoFramegrabber(HTuple(USBCameraInterface.toStdString().c_str()), "device", Information, ValueList);
            for(int i = 0; i < ValueList->Length(); i++)
            {
                deviceList.push_back(QString(ValueList[0][i].S()));
            }
        }
        else if(cameraInterface == "GigEVision2")
        {
            InfoFramegrabber("GigEVision2", "device", Information, ValueList);
            QString list;
            int index = 0;
            int index2 = 0;
            CameraInfoEthernet info2;
            for (int i = 0; i < ValueList->Length(); i++)
            {
                list = ValueList[0][i].S();
                index = list.indexOf("device:", 0, Qt::CaseSensitive);
                index2 = list.indexOf(" | unique_name:", 0, Qt::CaseSensitive);
                deviceList.push_back(list.mid(index + 7, index2 - index - 7));
            }
        }
        else
        {
        }
    }
    catch (HException &exception)
    {
        fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
                exception.ProcName().Text(),
                exception.ErrorMessage().Text());
    }
    return deviceList;
}

QVector<int> HalconCamera::getAvailablePort(QString device)
{
    QVector<int> portList;
    try
    {
        for(int i = 0; i < cameraInfosetDirectShow.length(); i++)
        {
//            qDebug() << "HalconCamera::getAvailablePort" << cameraInfosetDirectShow[i].device << device.mid(1,1).toInt();
#ifdef WIN32
            if(cameraInfosetDirectShow[i].device == device.mid(1,1).toInt())
#elif __linux__
            if(cameraInfosetDirectShow[i].device == device.mid(5,1).toInt())
#endif
                if(!portList.contains(cameraInfosetDirectShow[i].port))
                    portList.push_back(cameraInfosetDirectShow[i].port);
        }
    }
    catch (HException &exception)
    {
        fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
                exception.ProcName().Text(),
                exception.ErrorMessage().Text());
    }
    return portList;
}

QVector<QString> HalconCamera::getAvailableCameraType(QString device, int port)
{
    QVector<QString> typeList;
    try
    {
        for(int i = 0; i < cameraInfosetDirectShow.length(); i++)
        {
#ifdef WIN32
            if(cameraInfosetDirectShow[i].device == device.mid(1,1).toInt())
#elif __linux__
            if(cameraInfosetDirectShow[i].device == device.mid(5,1).toInt())
#endif
                if(cameraInfosetDirectShow[i].port == port)
                    if(!typeList.contains(cameraInfosetDirectShow[i].cameraType))
                        typeList.push_back(cameraInfosetDirectShow[i].cameraType);
        }
    }
    catch (HException &exception)
    {
        fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
                exception.ProcName().Text(),
                exception.ErrorMessage().Text());
    }
    return typeList;
}

