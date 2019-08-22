#include "depthmapthread.h"

DepthMapThread::DepthMapThread(StereoCamera* _stereoCamera)
{
    stereoCamera = _stereoCamera;
}

DepthMapThread::~DepthMapThread(){}

void DepthMapThread::run()
{
    try
    {
        QVector<int> times(6);
        QStringList timeNames = {"Rectify", "SGBM", "Display", "Reprojection", "", "All"};

        while (IsStillRunning)
        {
            try
            {
                QTime timedebug;
                timedebug.start();
                QMutexLocker locker(&m);
                stereoCamera->src1 = HImageToIplImage(hImageSrc1);
                stereoCamera->src2 = HImageToIplImage(hImageSrc2);
                locker.unlock();
                stereoCamera->undist(cv::INTER_LINEAR);
                times[0] = timedebug.elapsed();
                stereoCamera->computeSGBM(stereoCamera->dst1, stereoCamera->dst2);
                times[1] = timedebug.elapsed();
//                cv::namedWindow("Deep", cv::WINDOW_NORMAL);
//                cv::imshow("Deep", stereoCamera->disp);

                hImageDeep = IplImageToHImage(stereoCamera->disp);
                cv::imwrite("Depth Map.png", stereoCamera->disp);
                cv::imwrite("Original.png", stereoCamera->src1);
                cv::waitKey();
//                cv::Mat imageColorDeep = stereoCamera->GenerateFalseMap();
//                hImageDeep = IplImageToHImage(imageColorDeep);
                DispImage(hImageDeep, hv_WindowHandleOI4);
                times[2] = timedebug.elapsed();
                cv::reprojectImageTo3D(stereoCamera->dispReal, coordinates, stereoCamera->Q, true);
//                stereoCamera->saveXYZ("C:/Users/12257/Desktop/GraduationThesis/QtProject/coordinates.data", coordinates);
                times[3] = timedebug.elapsed();

                times[5] = times[3];
                times[4] = 0;
                times[3] = times[3] - times[2];
                times[2] = times[2] - times[1];
                times[1] = times[1] - times[0];

                emit runTimes(times, timeNames, 2);    // Emit signal for passing parameters
            } catch (const std::exception&){}
            catch (HException &){}
        }
    }
    catch (HException &){}
}
