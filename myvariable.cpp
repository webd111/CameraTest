#include "myvariable.h"

HTuple hv_WindowHandleSrc1, hv_WindowHandleSrc2, hv_WindowHandleSrc3, hv_WindowHandleSrc4;
//HTuple hv_WindowHandleOI3, hv_WindowHandleOI4, hv_WindowHandlePI;

std::vector<HTuple*> hv_WindowHandle_ptr = {&hv_WindowHandleSrc1, &hv_WindowHandleSrc2, &hv_WindowHandleSrc3, &hv_WindowHandleSrc4};

QMutex m_img1, m_img2, m_img3, m_img4;

HImage hImageSrc1;
HImage hImageSrc2;
HImage hImageSrc3;
HImage hImageSrc4;

std::vector<QMutex*> m_ptr = {&m_img1, &m_img2, &m_img3, &m_img4};
std::vector<HImage*> hImage_ptr = {&hImageSrc1, &hImageSrc2, &hImageSrc3, &hImageSrc4};

//HImage hImageOutput;
//HImage hImageDeep;
//HImage hImageMatchPair;

//cv::Mat coordinates;

