#include "imgacqthread.h"

ImgAcqThread::ImgAcqThread(HCameraParams _params, int _mode)
{
    cameraInterface = _params.cameraInterface;
    cameraIndex = _params.cameraIndex;
    mode = _mode;

    halconCamera = new HalconCamera(_params);

    himage = hImage_ptr[cameraIndex-1];
    hwindowhandle = hv_WindowHandle_ptr[cameraIndex-1];
    m = m_ptr[cameraIndex-1];

//    qDebug() << QString("New ImgAcqThread is constructed with cameraIndex = %1, cameraInterface = %2, device = %3, port = %4, cameraType = %5").arg(cameraIndex).arg(cameraInterface).arg(device).arg(port).arg(cameraType);
}

ImgAcqThread::ImgAcqThread(WCameraParams _params, int _mode)
{
    cameraInterface = _params.cameraInterface;
    cameraIndex = _params.cameraIndex;
    mode = _mode;

    webCamera = new WebCamera(_params);

    himage = hImage_ptr[cameraIndex-1];
    hwindowhandle = hv_WindowHandle_ptr[cameraIndex-1];
    qDebug() << "mode: " << mode;
}

ImgAcqThread::~ImgAcqThread()
{
    qDebug() << "ImgAcqThread::~ImgAcqThread()";
    if (halconCamera != nullptr)
    {
        delete halconCamera;
        halconCamera = nullptr;
    }
    if (webCamera != nullptr)
    {
        delete webCamera;
        webCamera = nullptr;
    }
}

QString ImgAcqThread::getInterface()
{
    return cameraInterface;
}

void ImgAcqThread::end()
{
    requestInterruption();
//    quit();
//    wait(100);
    qDebug() << QString("cameraIndex = %1, ImgAcqThread::end()").arg(cameraIndex);
}

void ImgAcqThread::run()
{
    qDebug() << "ImgAcqThread::run() starts";
    qDebug() << "Worker Thread Id: " << currentThreadId();
    if(cameraInterface == USBCameraInterface || cameraInterface == "GigEVision2")
    {
        // Check parameters
        if(cameraIndex != 0 &&
                halconCamera->params.device != ""/* &&
                halconCamera->params.port != -5 &&
                halconCamera->params.cameraType != ""*/)
        {
            qDebug() << "ImgAcqThread::run() parameters are valid.";
            try
            {
                HImage hi_Image;
                double time = 0;

                if(halconCamera->connect())
                    qDebug() << "ImgAcqThread::run() OpenFramegrabber succeeds";
                else
                {
                    qDebug() << "ImgAcqThread::run() OpenFramegrabber fails";
                    return;
                }

                QString info[4] = {halconCamera->params.cameraInterface, halconCamera->params.device,
                                   QString::number(halconCamera->params.port), halconCamera->params.cameraType};
                emit ImgAcqInfo(cameraIndex, info);

                while (!isInterruptionRequested())
                {
                    try
                    {
                        time = timer.elapsed();
                        timer.start();

                        // Grab Image
                        halconCamera->grab();
//                        qDebug() << "halconCamera->grab()" << timer.elapsed();
                        hi_Image = halconCamera->getGribbedImage();
//                        qDebug() << "halconCamera->getGribbedImage()" << timer.elapsed();

                        // Preprocessing

                        // Copy to Global variant /*and normalize*/
                        QMutexLocker locker(m);
//                      *himage = hi_Image.ScaleImageMax();
                        *himage = hi_Image;
#ifdef WIN32
                        DispImage(*himage, *hwindowhandle);
#elif __linux__
                        emit ImgCaptured(cameraIndex);
#endif
//                      qDebug() << "DispImage(*himage, *hwindowhandle)" << timer.elapsed();
                        locker.unlock();

                        // Display image

                        emit ImgAcqTime(cameraIndex, time);    // Emit signal for passing parameters
                    }
                    catch (HException &exception)
                    {
                        emit ImgAcqException(cameraIndex, QTime::currentTime().toString("HH:mm:ss:zzz"), exception);
                        fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
                                exception.ProcName().Text(),
                                exception.ErrorMessage().Text());
                    }
                }
                halconCamera->close();
            }
            catch (HException &exception)
            {
                emit ImgAcqException(cameraIndex, QTime::currentTime().toString("HH:mm:ss:zzz"), exception);
                fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
                        exception.ProcName().Text(),
                        exception.ErrorMessage().Text());
            }
        }
        else
        {
            qDebug() << "ImgAcqThread::run() parameters are not valid!";
        }
    }
    else if (cameraInterface == "UserDefined")
    {
        // Check parameters
        WCameraParams params = webCamera->getWParams();
        if(cameraIndex != 0 &&
                params.ip_client != "" &&
                params.ip_server != "" &&
                params.port_client != 0 &&
                params.port_server != 0 &&
                params.width != 0 &&
                params.height != 0)
        {
            double time = 0;


            QString info[4] = {params.cameraInterface,
                               "",
                               QString::fromStdString(params.ip_client)+QString("::")+QString::number(params.port_client),
                               QString::fromStdString(params.ip_server)+QString("::")+QString::number(params.port_server)};
            emit ImgAcqInfo(cameraIndex, info);

            qDebug() << "ImgAcqThread::run() parameters are valid.";

            // Using socket
            Mat img;
//            webCamera->sendCommand(0x01);           // cmd: send. The send procedure seems to be necessary before it can start to receive.
            while( !isInterruptionRequested() )
            {
                webCamera->sendCommand(0x01);           // cmd: send
                while(!webCamera->grabSocket() && !isInterruptionRequested());
//                webCamera->sendCommand(0x02);           // cmd: received
                if (!webCamera->getImage(img))
                    continue;

                time = timer.elapsed();
                timer.start();

                QMutexLocker locker(m);

                // 以下代码根据实际需求注释
                // 如果为灰度或者彩色的单视图图像，19-09-11
                if (mode == 1)
                {
                    *himage = IplImageRGBToHImage(img);         // 输入图像保持RGB结构，cvMat转HImage中进行了修改，可以直接将RGB转为HImage
                }
                else if (mode == 2)
                {
                    // 如果为彩色+左右双视图图像，在使用时将图像高度*2，表示双视图大小，19-09-11
                    hImage_ptr[0]->Clear();
                    hImage_ptr[1]->Clear();
                    IplImageRGBSplitToHImage(img,*hImage_ptr[0],*hImage_ptr[1]);
                }
                else if (mode == 3)
                {
                    // Waiting for implementation
                }
#ifdef WIN32
//                DispImage(*himage, *hwindowhandle);
                for(int i = 0; i < mode; ++i)
                {
                    DispImage(*hImage_ptr[i], *hv_WindowHandle_ptr[i]);
                }
#elif __linux__
                emit ImgCaptured(cameraIndex);
#endif
                locker.unlock();

//                Sleep(100);

                emit ImgAcqTime(cameraIndex, time);    // Emit signal for passing parameters
            }
        }
        else
        {
            qDebug() << "ImgAcqThread::run() parameters are not valid!";
        }
    }
    else
    {
        qDebug() << "ImgAcqThread::run() wrong cameraInterface!";
    }
}


