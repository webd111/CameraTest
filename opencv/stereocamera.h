#ifndef STEREOCALIBRATIONRECTIFY_H
#define STEREOCALIBRATIONRECTIFY_H

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QDebug>
#include <QThread>
#include <QObject>
#include <QTime>

#include "opencv/myopencv.h"
#include "myvariable.h"

class StereoCamera
{
public:
    cv::Mat src1, src2, dst1, dst2, disp, dispReal;
    cv::Mat cameraMatrix1, distCoffs1, cameraMatrix2, distCoffs2;
    cv::Mat R12, T12;
    cv::Mat R1, R2, P1, P2, Q;

    StereoCamera();

    void undist(int interp = cv::INTER_LINEAR);
    void resetCameraParams();
    void setCameraParams(cv::Mat, cv::Mat, cv::Mat, cv::Mat, cv::Mat, cv::Mat);
    void rectify(cv::Size size);
    void computeSGBM(const cv::Mat&, const cv::Mat&, int outputImg = LEFT);
    void saveXYZ(const char* filename, const cv::Mat& mat);
    cv::Mat GenerateFalseMap();

    enum ImageIndex
    {
        LEFT,
        RIGHT
    };
};



#endif
