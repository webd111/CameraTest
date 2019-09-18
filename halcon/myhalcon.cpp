#include "myhalcon.h"

/**
 * @brief HImage2QImage 将 Halcon 的 HImage 转换为 Qt 的 QImage
 * @param from HImage ，暂时只支持 8bits 灰度图像和 8bits 的 3 通道彩色图像
 * @param to QImage ，这里 from 和 to 不共享内存。如果 to 的内存大小合适，那么就不用重新分配内存。所以可以加快速度。
 * @return  true 表示转换成功，false 表示转换失败
 */
bool HImage2QImage(HalconCpp::HImage &from, QImage &to)
{
    Hlong width;
    Hlong height;
    from.GetImageSize(&width, &height);

    HTuple channels = from.CountChannels();
    HTuple type = from.GetImageType();

    if( strcmp(type[0].S(), "byte" )) // 如果不是 byte 类型，则失败
    {
        return false;
    }

    QImage::Format format;
    switch(channels[0].I())
    {
    case 1:
//        format = QImage::Format_Grayscale8;
        format = QImage::Format_Indexed8;
        break;
    case 3:
        format = QImage::Format_RGB32;
        break;
    default:
        return false;
    }

    if(to.width() != width || to.height() != height || to.format() != format)
    {
        to = QImage(static_cast<int>(width),
                    static_cast<int>(height),
                    format);
    }
    HString Type;
    if(channels[0].I() == 1)
    {
        unsigned char* pSrc = reinterpret_cast<unsigned char *>( from.GetImagePointer1(&Type, &width, &height) );
//        memcpy( to.bits(), pSrc, static_cast<size_t>(width) * static_cast<size_t>(height) );
        for (int row = 0; row < height; ++row) {
            memcpy(to.scanLine(row), pSrc + row * width, static_cast<size_t>(width));
        }
        return true;
    }
    else if(channels[0].I() == 3)
    {
        uchar *R, *G, *B;
        from.GetImagePointer3(reinterpret_cast<void **>(&R),
                              reinterpret_cast<void **>(&G),
                              reinterpret_cast<void **>(&B), &Type, &width, &height);

        for(int row = 0; row < height; row ++)
        {
            QRgb* line = reinterpret_cast<QRgb*>(to.scanLine(row));
            for(int col = 0; col < width; col ++)
            {
                line[col] = qRgb(*R++, *G++, *B++);
            }
        }
        return true;
    }

    return false;
}

/**
 * @brief QImage2HImage 将 Qt QImage 转换为 Halcon 的 HImage
 * @param from 输入的 QImage
 * @param to 输出的 HImage ，from 和 to 不共享内存数据。 每次都会为 to 重新分配内存。
 * @return true 表示转换成功，false 表示转换失败。
 */
bool QImage2HImage(QImage &from, HalconCpp::HImage &to)
{
    if(from.isNull()) return false;

    int width = from.width(), height = from.height();
    QImage::Format format = from.format();

    if(format == QImage::Format_RGB32 ||
            format == QImage::Format_ARGB32 ||
            format == QImage::Format_ARGB32_Premultiplied)
    {
        to.GenImageInterleaved(from.bits(), "rgbx", width, height, 0,  "byte", width, height, 0, 0, 8, 0);
        return true;
    }
    else if(format == QImage::Format_RGB888)
    {
        to.GenImageInterleaved(from.bits(), "rgb", width, height, 0,  "byte", width, height, 0, 0, 8, 0);
        return true;
    }
    else if(format == QImage::Format_Grayscale8 || format == QImage::Format_Indexed8)
    {
        to.GenImage1("byte", width, height, from.bits());
        return true;
    }
    return false;
}


// Chapter: Graphics / Text
// Short Description: Set font independent of OS
void set_display_font (HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold,
                       HTuple hv_Slant)
{
    HTuple  hv_OS, hv_Fonts, hv_Style, hv_Exception;
    HTuple  hv_AvailableFonts, hv_Fdx, hv_Indices;

    GetSystem("operating_system", &hv_OS);
    if (0 != (HTuple(hv_Size==HTuple()).TupleOr(hv_Size==-1)))
    {
        hv_Size = 16;
    }
    if (0 != ((hv_OS.TupleSubstr(0,2))==HTuple("Win")))
    {
        hv_Size = (1.13677*hv_Size).TupleInt();
    }
    else
    {
        hv_Size = hv_Size.TupleInt();
    }
    if (0 != (hv_Font==HTuple("Courier")))
    {
        hv_Fonts.Clear();
        hv_Fonts[0] = "Courier";
        hv_Fonts[1] = "Courier 10 Pitch";
        hv_Fonts[2] = "Courier New";
        hv_Fonts[3] = "CourierNew";
        hv_Fonts[4] = "Liberation Mono";
    }
    else if (0 != (hv_Font==HTuple("mono")))
    {
        hv_Fonts.Clear();
        hv_Fonts[0] = "Consolas";
        hv_Fonts[1] = "Menlo";
        hv_Fonts[2] = "Courier";
        hv_Fonts[3] = "Courier 10 Pitch";
        hv_Fonts[4] = "FreeMono";
        hv_Fonts[5] = "Liberation Mono";
    }
    else if (0 != (hv_Font==HTuple("sans")))
    {
        hv_Fonts.Clear();
        hv_Fonts[0] = "Luxi Sans";
        hv_Fonts[1] = "DejaVu Sans";
        hv_Fonts[2] = "FreeSans";
        hv_Fonts[3] = "Arial";
        hv_Fonts[4] = "Liberation Sans";
    }
    else if (0 != (hv_Font==HTuple("serif")))
    {
        hv_Fonts.Clear();
        hv_Fonts[0] = "Times New Roman";
        hv_Fonts[1] = "Luxi Serif";
        hv_Fonts[2] = "DejaVu Serif";
        hv_Fonts[3] = "FreeSerif";
        hv_Fonts[4] = "Utopia";
        hv_Fonts[5] = "Liberation Serif";
    }
    else
    {
        hv_Fonts = hv_Font;
    }
    hv_Style = "";
    if (0 != (hv_Bold==HTuple("true")))
    {
        hv_Style += HTuple("Bold");
    }
    else if (0 != (hv_Bold!=HTuple("false")))
    {
        hv_Exception = "Wrong value of control parameter Bold";
        throw HException(hv_Exception);
    }
    if (0 != (hv_Slant==HTuple("true")))
    {
        hv_Style += HTuple("Italic");
    }
    else if (0 != (hv_Slant!=HTuple("false")))
    {
        hv_Exception = "Wrong value of control parameter Slant";
        throw HException(hv_Exception);
    }
    if (0 != (hv_Style==HTuple("")))
    {
        hv_Style = "Normal";
    }
    QueryFont(hv_WindowHandle, &hv_AvailableFonts);
    hv_Font = "";
    {
        HTuple end_val50 = (hv_Fonts.TupleLength())-1;
        HTuple step_val50 = 1;
        for (hv_Fdx=0; hv_Fdx.Continue(end_val50, step_val50); hv_Fdx += step_val50)
        {
            hv_Indices = hv_AvailableFonts.TupleFind(HTuple(hv_Fonts[hv_Fdx]));
            if (0 != ((hv_Indices.TupleLength())>0))
            {
                if (0 != (HTuple(hv_Indices[0])>=0))
                {
                    hv_Font = HTuple(hv_Fonts[hv_Fdx]);
                    break;
                }
            }
        }
    }
    if (0 != (hv_Font==HTuple("")))
    {
        throw HException("Wrong value of control parameter Font");
    }
    hv_Font = (((hv_Font+"-")+hv_Style)+"-")+hv_Size;
    return;
}

// Chapter: Develop
// Short Description: Changes the size of a graphics window with a given maximum and minimum extent such that it preserves the aspect ratio of the given image
void dev_resize_window_fit_image (HObject ho_Image, HTuple hv_Row, HTuple hv_Column,
                                  HTuple hv_WidthLimit, HTuple hv_HeightLimit)
{
    HTuple  hv_MinWidth, hv_MaxWidth, hv_MinHeight;
    HTuple  hv_MaxHeight, hv_ResizeFactor, hv_Pointer, hv_Type;
    HTuple  hv_ImageWidth, hv_ImageHeight, hv_TempWidth, hv_TempHeight;
    HTuple  hv_WindowWidth, hv_WindowHeight;

    if (0 != (HTuple((hv_WidthLimit.TupleLength())==0).TupleOr(hv_WidthLimit<0)))
    {
        hv_MinWidth = 500;
        hv_MaxWidth = 800;
    }
    else if (0 != ((hv_WidthLimit.TupleLength())==1))
    {
        hv_MinWidth = 0;
        hv_MaxWidth = hv_WidthLimit;
    }
    else
    {
        hv_MinWidth = ((const HTuple&)hv_WidthLimit)[0];
        hv_MaxWidth = ((const HTuple&)hv_WidthLimit)[1];
    }
    if (0 != (HTuple((hv_HeightLimit.TupleLength())==0).TupleOr(hv_HeightLimit<0)))
    {
        hv_MinHeight = 400;
        hv_MaxHeight = 600;
    }
    else if (0 != ((hv_HeightLimit.TupleLength())==1))
    {
        hv_MinHeight = 0;
        hv_MaxHeight = hv_HeightLimit;
    }
    else
    {
        hv_MinHeight = ((const HTuple&)hv_HeightLimit)[0];
        hv_MaxHeight = ((const HTuple&)hv_HeightLimit)[1];
    }
    hv_ResizeFactor = 1;
    GetImagePointer1(ho_Image, &hv_Pointer, &hv_Type, &hv_ImageWidth, &hv_ImageHeight);
    if (0 != (HTuple(hv_MinWidth>hv_ImageWidth).TupleOr(hv_MinHeight>hv_ImageHeight)))
    {
        hv_ResizeFactor = (((hv_MinWidth.TupleReal())/hv_ImageWidth).TupleConcat((hv_MinHeight.TupleReal())/hv_ImageHeight)).TupleMax();
    }
    hv_TempWidth = hv_ImageWidth*hv_ResizeFactor;
    hv_TempHeight = hv_ImageHeight*hv_ResizeFactor;
    if (0 != (HTuple(hv_MaxWidth<hv_TempWidth).TupleOr(hv_MaxHeight<hv_TempHeight)))
    {
        hv_ResizeFactor = hv_ResizeFactor*((((hv_MaxWidth.TupleReal())/hv_TempWidth).TupleConcat((hv_MaxHeight.TupleReal())/hv_TempHeight)).TupleMin());
    }
    hv_WindowWidth = hv_ImageWidth*hv_ResizeFactor;
    hv_WindowHeight = hv_ImageHeight*hv_ResizeFactor;
    if (HDevWindowStack::IsOpen())
        SetWindowExtents(HDevWindowStack::GetActive(),hv_Row, hv_Column, hv_WindowWidth,
                         hv_WindowHeight);
    if (HDevWindowStack::IsOpen())
        SetPart(HDevWindowStack::GetActive(),0, 0, hv_ImageHeight-1, hv_ImageWidth-1);
    return;
}

void DispImage(HImage &img, HTuple &win)
{
    try
    {
#ifdef WIN32
//        SetSystem("use_window_thread", "true");
#elif defined(__linux__) && (defined(__i386__) || defined(__x86_64__))
//        XInitThreads();
#endif
//        // file was stored with local-8-bit encoding
//        //   -> set the interface encoding accordingly
//        SetHcppInterfaceStringEncodingIsUtf8(false);

        HTuple HOIwidth, HOIheight;
//        try {ClearWindow(win);} catch (HException &){}
        GetImageSize(img, &HOIwidth, &HOIheight);
        SetPart(win, 0, 0, HOIheight - 1, HOIwidth - 1);
        DispObj(img, win);
    }
    catch (HException &/*exception*/)
    {
//        fprintf(stderr, "  Error #%u in %s: %s\n", exception.ErrorCode(),
//                exception.ProcName().Text(),
//                exception.ErrorMessage().Text());
    }
}

