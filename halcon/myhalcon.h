#ifndef MYHALCON_H
#define MYHALCON_H

#include <QImage>
#include <QLabel>

#include "Halcon.h"
#include "HalconCpp.h"
#include "HDevThread.h"
#if defined(__linux__) && (defined(__i386__) || defined(__x86_64__)) \
    && !defined(NO_EXPORT_APP_MAIN)
#include <X11/Xlib.h>
#endif

using namespace HalconCpp;

bool HImage2QImage(HalconCpp::HImage &from, QImage &to);
bool QImage2HImage(QImage &from, HalconCpp::HImage &to);

// Chapter: Graphics / Text
// Short Description: Set font independent of OS
extern void set_display_font (HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold,
    HTuple hv_Slant);

// Chapter: Develop
// Short Description: Changes the size of a graphics window with a given maximum and minimum extent such that it preserves the aspect ratio of the given image
void dev_resize_window_fit_image (HObject ho_Image, HTuple hv_Row, HTuple hv_Column,
    HTuple hv_WidthLimit, HTuple hv_HeightLimit);

void DispImage(HImage& img, HTuple& win);

#endif
