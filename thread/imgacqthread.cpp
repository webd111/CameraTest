#include "imgacqthread.h"

ImgAcqThread::ImgAcqThread(HCameraParams _params)
{
    cameraInterface = _params.cameraInterface;
    cameraIndex = _params.cameraIndex;

    halconCamera = new HalconCamera(_params);

    himage = hImage_ptr[cameraIndex-1];
    hwindowhandle = hv_WindowHandle_ptr[cameraIndex-1];
    m = m_ptr[cameraIndex-1];

//    qDebug() << QString("New ImgAcqThread is constructed with cameraIndex = %1, cameraInterface = %2, device = %3, port = %4, cameraType = %5").arg(cameraIndex).arg(cameraInterface).arg(device).arg(port).arg(cameraType);
}

ImgAcqThread::ImgAcqThread(WCameraParams _params)
{
    cameraInterface = _params.cameraInterface;
    cameraIndex = _params.cameraIndex;

    webCamera = new WebCamera(_params);

    himage = hImage_ptr[cameraIndex-1];
    hwindowhandle = hv_WindowHandle_ptr[cameraIndex-1];
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

//            QString info[5] = {params.cameraInterface, QString::fromStdString(params.ip_client), QString::number(params.port_client),
//                               QString::fromStdString(params.ip_client), QString::number(params.port_client)};
//            emit ImgAcqInfo(cameraIndex, info);

            qDebug() << "ImgAcqThread::run() parameters are valid.";

            // Using socket
            Mat img;
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
                *himage = IplImageToHImage(img);
#ifdef WIN32
                DispImage(*himage, *hwindowhandle);
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


