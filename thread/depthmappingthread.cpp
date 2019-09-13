#include "depthmappingthread.h"

using namespace cv;

DepthMappingThread::DepthMappingThread(StereoCamera* _stereoCamera)
{
    stereoCamera = _stereoCamera;
}

DepthMappingThread::~DepthMappingThread()
{

}

void DepthMappingThread::end()
{
    requestInterruption();
//    quit();
//    wait(100);
//    qDebug() << QString("cameraIndex = %1, ImgAcqThread::end()").arg(cameraIndex);
}

void DepthMappingThread::run()
{
    qDebug() << "DepthMappingThread::run() starts";
    qDebug() << "Worker Thread Id: " << currentThreadId();

    double time = 0;
    // Check parameters
    while (!isInterruptionRequested())
    {
        try
        {
            time = timer.elapsed();
            timer.start();

            HImage himg_left, himg_right;
            QMutexLocker locker1(m_ptr[0]);
            himg_left = hImageSrc1.CopyImage();
            locker1.unlock();
            QMutexLocker locker2(m_ptr[1]);
            himg_right = hImageSrc2.CopyImage();
            locker2.unlock();

            himg_left = himg_left.GaussImage(5);
            himg_right = himg_right.GaussImage(5);

            qDebug() << "HImageToIplImage()";
            stereoCamera->src1 = HImageToIplImage(himg_left);
            stereoCamera->src2 = HImageToIplImage(himg_right);
            qDebug() << "stereoCamera->undist()";
            stereoCamera->undist();
            qDebug() << "stereoCamera->computeSGBM()";
            stereoCamera->computeSGBM(stereoCamera->dst1, stereoCamera->dst2);

            HImage himg_deep = IplImageToHImage(stereoCamera->disp);
            DispImage(himg_deep, hv_WindowHandleSrc4);
        }
        catch (HException &exception)
        {
//            emit ImgAcqException(cameraIndex, QTime::currentTime().toString("HH:mm:ss:zzz"), exception);
//            fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
//                    exception.ProcName().Text(),
//                    exception.ErrorMessage().Text());
        }
    }
}
