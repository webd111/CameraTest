#ifndef MYVARIABLE_H
#define MYVARIABLE_H

#include "halcon/myhalcon.h"
#include "opencv/myopencv.h"

#include <QMutex>
#include <QMutexLocker>

extern HTuple hv_WindowHandleSrc1, hv_WindowHandleSrc2, hv_WindowHandleSrc3, hv_WindowHandleSrc4;
//extern HTuple hv_WindowHandleOI3, hv_WindowHandleOI4, hv_WindowHandlePI;              // 显示窗口句柄
extern QMutex m_img1, m_img2, m_img3, m_img4;

extern HImage hImageSrc1;           // 原始图像1
extern HImage hImageSrc2;           // 原始图像2
extern HImage hImageSrc3;           // 原始图像3
extern HImage hImageSrc4;           // 原始图像4
//extern HImage hImageOutput;          // 输出图像
//extern HImage hImageDeep;            // 深度图像
//extern HImage hImageMatchPair;       // 匹配对

//extern cv::Mat coordinates;

extern std::vector<QMutex*> m_ptr;
extern std::vector<HTuple*> hv_WindowHandle_ptr;
extern std::vector<HImage*> hImage_ptr;

#endif
