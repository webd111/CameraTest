#include "myopencv.h"

cv::Mat HImageToIplImage(HImage& img)
{
    cv::Mat pImage;
    HTuple htChannels;
    HString cType;
    Hlong width, height;

    ConvertImageType(img, &img, "byte");
    CountChannels(img, &htChannels);
    if (htChannels[0].I() == 1)
    {
        void* ptr = img.GetImagePointer1(&cType, &width, &height);
        pImage.create(int(height), int(width), CV_8UC1);
        memcpy(pImage.data, (uchar*)ptr, size_t(width*height));
    }
    else if (htChannels[0].I() == 3)
    {
        void *ptrR, *ptrG, *ptrB;
        img.GetImagePointer3(&ptrR, &ptrG, &ptrB, &cType, &width, &height);
        uchar *ptrRed, *ptrGreen, *ptrBlue;
        ptrRed = (uchar*)ptrR;
        ptrGreen = (uchar*)ptrG;
        ptrBlue = (uchar*)ptrB;
        pImage.create(int(height), int(width), CV_8UC3);
        std::vector<cv::Mat> mgImage(3);
        mgImage[2].create(int(height), int(width), CV_8UC1);
        mgImage[1].create(int(height), int(width), CV_8UC1);
        mgImage[0].create(int(height), int(width), CV_8UC1);
        memcpy(mgImage[2].data, ptrRed, width*height);
        memcpy(mgImage[1].data, ptrGreen, width*height);
        memcpy(mgImage[0].data, ptrBlue, width*height);
        cv::merge(mgImage, pImage);
    }
    return pImage;
}

HImage IplImageToHImage(cv::Mat& pImage)
{
    HImage img;
    int height = pImage.rows;
    int width = pImage.cols;
    if (3 == pImage.channels())
    {
        cv::Mat pImageRed, pImageGreen, pImageBlue;
        std::vector<cv::Mat> sbgr;
        split(pImage, sbgr);
        pImageRed = sbgr[2];
        pImageGreen = sbgr[1];
        pImageBlue = sbgr[0];
        int length = pImage.rows * pImage.cols;
        uchar *dataBlue = new uchar[length];
        uchar *dataGreen = new uchar[length];
        uchar *dataRed = new uchar[length];
        memcpy(dataRed, pImageRed.data, width*height);
        memcpy(dataGreen, pImageGreen.data, width*height);
        memcpy(dataBlue, pImageBlue.data, width*height);
        img.GenImage3(HString("byte"), width, height, dataRed, dataGreen, dataBlue);
        delete[] dataRed;
        delete[] dataGreen;
        delete[] dataBlue;
    }
    else if (1 == pImage.channels())
    {
        uchar *dataGray = new uchar[width * height];
        memcpy(dataGray, pImage.data, width * height);
        img.GenImage1(HString("byte"), width, height, dataGray);
        delete[] dataGray;
    }
    return img;
}

HImage IplImageRGBToHImage(cv::Mat& pImage)
{
    HImage img;
    int height = pImage.rows;
    int width = pImage.cols;
    if (3 == pImage.channels())
    {
        cv::Mat pImageRed, pImageGreen, pImageBlue;
        std::vector<cv::Mat> sbgr;
        split(pImage, sbgr);
        pImageRed = sbgr[0];
        pImageGreen = sbgr[1];
        pImageBlue = sbgr[2];
        int length = pImage.rows * pImage.cols;
        uchar *dataBlue = new uchar[length];
        uchar *dataGreen = new uchar[length];
        uchar *dataRed = new uchar[length];
        memcpy(dataRed, pImageRed.data, width*height);
        memcpy(dataGreen, pImageGreen.data, width*height);
        memcpy(dataBlue, pImageBlue.data, width*height);
        img.GenImage3(HString("byte"), width, height, dataRed, dataGreen, dataBlue);
        delete[] dataRed;
        delete[] dataGreen;
        delete[] dataBlue;
    }
    else if (1 == pImage.channels())
    {
        uchar *dataGray = new uchar[width * height];
        memcpy(dataGray, pImage.data, width * height);
        img.GenImage1(HString("byte"), width, height, dataGray);
        delete[] dataGray;
    }
    return img;
}

void IplImageRGBSplitToHImage(cv::Mat& pImage, HImage& img1, HImage& img2)
{
    int height = pImage.rows;
    int width = pImage.cols;
    if (3 == pImage.channels())
    {
        cv::Mat pImageRed, pImageGreen, pImageBlue;
        std::vector<cv::Mat> sbgr;
        split(pImage, sbgr);
        pImageRed = sbgr[0];
        pImageGreen = sbgr[1];
        pImageBlue = sbgr[2];
        int length = pImage.rows * pImage.cols;
        uchar *dataBlue = new uchar[length/2];
        uchar *dataGreen = new uchar[length/2];
        uchar *dataRed = new uchar[length/2];
        for (int i = 0; i < 2; ++i) {
            memcpy(dataRed, pImageRed.data+i*length/2, length/2);
            memcpy(dataGreen, pImageGreen.data+i*length/2, length/2);
            memcpy(dataBlue, pImageBlue.data+i*length/2, length/2);
            if (i==0)
                img1.GenImage3(HString("byte"), width, height/2, dataRed, dataGreen, dataBlue);
            else
                img2.GenImage3(HString("byte"), width, height/2, dataRed, dataGreen, dataBlue);
        }
        delete[] dataRed;
        delete[] dataGreen;
        delete[] dataBlue;
    }
    else if (1 == pImage.channels())
    {
        uchar *dataGray = new uchar[width * height / 2];
        for (int i = 0; i < 2; ++i) {
            memcpy(dataGray, pImage.data+i * width*height/2, width * height/2);
            if (i==0)
                img1.GenImage1(HString("byte"), width, height/2, dataGray);
            else
                img2.GenImage1(HString("byte"), width, height/2, dataGray);
        }
        delete[] dataGray;
    }
    return;
}
