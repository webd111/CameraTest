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
    int length = pImage.rows * pImage.cols;
    cv::Mat pImageRed, pImageGreen, pImageBlue;
    std::vector<cv::Mat> sbgr;
    cv::Mat img_rgb = cv::Mat(height/3, width, CV_8UC3, pImage.data);
    split(img_rgb, sbgr);
    pImageRed = sbgr[0];
    pImageGreen = sbgr[1];
    pImageBlue = sbgr[2];
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
    return;
}

void IplImageRGBDSplitToHImage(cv::Mat& pImage, HImage& img1, HImage& img2, HImage& imgd)
{
    int height = pImage.rows/7;
    int width = pImage.cols;
    int length_std = height * width;
    // Split RGB
//    qDebug() << "Split RGB";
    cv::Mat pImageRed, pImageGreen, pImageBlue;
    std::vector<cv::Mat> sbgr;
    cv::Mat img_rgb = cv::Mat::zeros(height*2, width, CV_8UC3);
    memcpy(img_rgb.data, pImage.data, length_std*6);
    split(img_rgb, sbgr);
    pImageRed = sbgr[0];
    pImageGreen = sbgr[1];
    pImageBlue = sbgr[2];

//    // White Balance
//    //求原始图像的RGB分量的均值
//    double R, G, B;
//    B = mean(pImageBlue)[0];
//    G = mean(pImageGreen)[0];
//    R = mean(pImageRed)[0];

//    qDebug()  << "Gray" <<  R*0.299 + G*0.587 + B*0.114;

//    //需要调整的RGB分量的增益
//    double KR, KG, KB;
//    KB = (R + G + B) / (3 * B);
//    KG = (R + G + B) / (3 * G);
//    KR = (R + G + B) / (3 * R);

//    qDebug() << "KB, KG, KR" << KB << KG << KR;
    double KR = 1.3, KG = 0.9, KB = 0.9;

    //调整RGB三个通道各自的值
    pImageRed = pImageRed * KR;
    pImageGreen = pImageGreen * KG;
    pImageBlue = pImageBlue * KB;
    // White Balance

    uchar *dataBlue = new uchar[length_std];
    uchar *dataGreen = new uchar[length_std];
    uchar *dataRed = new uchar[length_std];
    for (int i = 0; i < 2; ++i) {
        memcpy(dataRed, pImageRed.data+i*length_std, length_std);
        memcpy(dataGreen, pImageGreen.data+i*length_std, length_std);
        memcpy(dataBlue, pImageBlue.data+i*length_std, length_std);
        if (i==0)
            img1.GenImage3(HString("byte"), width, height, dataRed, dataGreen, dataBlue);
        else
            img2.GenImage3(HString("byte"), width, height, dataRed, dataGreen, dataBlue);
    }
    delete[] dataRed;
    delete[] dataGreen;
    delete[] dataBlue;
    // Split D
//    qDebug() << "Split RGB";
    cv::Mat img_depth = cv::Mat(pImage, cv::Range(height*6, height*7));
    uchar *dataGray = new uchar[length_std];
    memcpy(dataGray, img_depth.data, length_std);
    imgd.GenImage1(HString("byte"), width, height, dataGray);
    delete[] dataGray;

    return;
}
