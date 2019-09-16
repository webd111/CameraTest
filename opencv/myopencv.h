#ifndef MYOPENCV_H
#define MYOPENCV_H

#include <QDebug>

#ifdef WIN32
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#elif __linux__
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#endif

#include <halcon/myhalcon.h>

using namespace HalconCpp;

extern HImage IplImageToHImage(cv::Mat& pImage);
extern cv::Mat HImageToIplImage(HImage& img);
extern HImage IplImageRGBToHImage(cv::Mat& pImage);
extern void IplImageRGBSplitToHImage(cv::Mat& pImage, HImage& img1, HImage& img2);
extern void IplImageRGBDSplitToHImage(cv::Mat& pImage, HImage& img1, HImage& img2, HImage& imgd);

#endif
