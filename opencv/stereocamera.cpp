#include "opencv/stereocamera.h"

StereoCamera::StereoCamera()
{
    resetCameraParams();
    rectify(cv::Size(1920, 1080));      // size of test images
}

void StereoCamera::resetCameraParams()
{
    cameraMatrix1 = (cv::Mat_<double>(3, 3) << 1.415123049671082e+03, 0, 0,
                     0, 1.414954143927605e+03, 0,
                     9.929454139215076e+02, 5.112310201960978e+02, 1);
    cameraMatrix1 = cameraMatrix1.t();
    distCoffs1 = (cv::Mat_<double>(1, 5) << -0.111056936133422, -0.217190554735444,
                  6.094631721034751e-05, 1.796910452452146e-05, 0.339662604994211);
    cameraMatrix2 = (cv::Mat_<double>(3, 3) << 1.421923511042942e+03, 0, 0,
                     0, 1.420159536231503e+03, 0,
                     9.583451553109910e+02, 4.762824159150644e+02, 1);
    cameraMatrix2 = cameraMatrix2.t();
    distCoffs2 = (cv::Mat_<double>(1, 5) << -0.133180789157152, -0.074379009557110,
                  -6.732732709133293e-05, 0.001625212638169, 0.121632332140773);
    R12 = (cv::Mat_<double>(3, 3) << 0.999801807816275, -1.100615251640996e-04, 0.019908113264906,
           -2.387380575372597e-05, 0.999977371089032, 0.006727312978461,
           -0.019908403184310, -0.006726454960040, 0.999779180762593);
    T12 = (cv::Mat_<double>(3, 1) << -1.205769421790057e+02, 0.313982796965114, 0.338439688817049);
}

void StereoCamera::setCameraParams(cv::Mat _cameraMatrix1, cv::Mat _distCoffs1,
                                   cv::Mat _cameraMatrix2, cv::Mat _distCoffs2,
                                   cv::Mat _R12, cv::Mat _T12)
{
    cameraMatrix1 = _cameraMatrix1;
    cameraMatrix2 = _cameraMatrix2;
    distCoffs1 = _distCoffs1;
    distCoffs2 = _distCoffs2;
    R12 = _R12;
    T12 = _T12;
}

void StereoCamera::rectify(cv::Size size)
{
    stereoRectify(cameraMatrix1, distCoffs1, cameraMatrix2, distCoffs2, size, R12, T12, R1, R2, P1, P2, Q);
}

void StereoCamera::undist(int interpMethod)
{
    cv::Mat map11, map12, map21, map22;
    cv::initUndistortRectifyMap(cameraMatrix1, distCoffs1, R1, P1, cv::Size(std::size(src1).width, std::size(src1).height), CV_32FC1, map11, map12);
    cv::initUndistortRectifyMap(cameraMatrix2, distCoffs2, R2, P2, cv::Size(std::size(src2).width, std::size(src2).height), CV_32FC1, map21, map22);
    cv::remap(src1, dst1, map11, map12, interpMethod);
    cv::remap(src2, dst2, map21, map22, interpMethod);

//    cv::Mat canvas;
//    int wid, hei;
//    wid = cvRound(std::size(src1).width);
//    hei = cvRound(std::size(src1).height);
//    canvas.create(hei, wid*2, CV_8UC3);
//    for (int k = 0; k < 2; k++)
//    {
//        cv::Mat canvasPart = canvas(cv::Rect(wid*k, 0, wid, hei));
//        cv::Mat src = k == 0 ? dst1: dst2;
//        cv::resize(src, canvasPart, canvasPart.size(), 0, 0, cv::INTER_AREA);
//    }
//    for (int j = 0; j < canvas.rows; j += 60)
//        line(canvas, cv::Point(0, j), cv::Point(canvas.cols, j), cv::Scalar(0, 255, 0), 3, 8);
//    namedWindow("RectifiedStereoCamera", WINDOW_NORMAL);
//    imshow("RectifiedStereoCamera", canvas);
}

void StereoCamera::computeSGBM(const cv::Mat& imgL, const cv::Mat& imgR, int outputImg)
{
    int numberOfDisparities = ((imgL.size().width / 8) + 15)&-16;
//    qDebug() << "numberOfDisparities" << numberOfDisparities;
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, 16, 3);
    sgbm->setPreFilterCap(32);

    int SADWindowSize = 5;
    int sgbmWinSize = SADWindowSize > 0 ? SADWindowSize : 3;
    sgbm->setBlockSize(sgbmWinSize);
    int cn = imgL.channels();

    sgbm->setP1(8 * cn*sgbmWinSize*sgbmWinSize);
    sgbm->setP2(32 * cn*sgbmWinSize*sgbmWinSize);
    sgbm->setMinDisparity(0);
    sgbm->setNumDisparities(numberOfDisparities);
    sgbm->setUniquenessRatio(10);
    sgbm->setSpeckleWindowSize(100);
    sgbm->setSpeckleRange(32);
    sgbm->setDisp12MaxDiff(1);

    cv::Mat left_gray, right_gray;
    cvtColor(imgL, left_gray, cv::COLOR_BGR2GRAY);
    cvtColor(imgR, right_gray, cv::COLOR_BGR2GRAY);

    if (outputImg == LEFT)	//计算左视差图
    {
        sgbm->compute(left_gray, right_gray, dispReal);
        dispReal.convertTo(dispReal, CV_32F, 1.0 / 16);			//除以16得到真实视差值
//        dispReal.convertTo(dispReal, CV_32F, 1.0 / numberOfDisparities);			//除以16得到真实视差值
        disp = cv::Mat(disp.rows, disp.cols, CV_8UC1);
        normalize(dispReal, disp, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }
    else if (outputImg == RIGHT)	//计算右视差图
    {
        sgbm->setMinDisparity(-numberOfDisparities);
        sgbm->setNumDisparities(numberOfDisparities);
        sgbm->compute(left_gray, right_gray, dispReal);
        dispReal.convertTo(dispReal, CV_32F, 1.0 / 16);			//除以16得到真实视差值
        disp = cv::Mat(disp.rows, disp.cols, CV_8UC1);
        normalize(dispReal, disp, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }
    else{disp = cv::Mat();dispReal = cv::Mat();}
}

void StereoCamera::saveXYZ(const char* filename, const cv::Mat& mat)
{
//    const double max_z = 1.0e4;
    FILE* fp = fopen(filename, "wt");
    for(int y = 0; y < mat.rows; y++)
    {
        for(int x = 0; x < mat.cols; x++)
        {
            cv::Vec3f point = mat.at<cv::Vec3f>(y, x);
//            if(fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
            fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
        }
    }
    fclose(fp);
}

cv::Mat StereoCamera::GenerateFalseMap()
{
    // color map
    float max_val = 255.0f;
    float map[8][4] = {{0,0,0,114},{0,0,1,185},{1,0,0,114},{1,0,1,174},
                       {0,1,0,114},{0,1,1,185},{1,1,0,114},{1,1,1,0}};
    float sum = 0;
    for (int i=0; i<8; i++)
      sum += map[i][3];
    float weights[8]; // relative   weights
    float cumsum[8];  // cumulative weights
    cumsum[0] = 0;
    for (int i=0; i<7; i++)
    {
      weights[i]  = sum/map[i][3];
      cumsum[i+1] = cumsum[i] + map[i][3]/sum;
    }
    int height_ = disp.rows;
    int width_ = disp.cols;
    // for all pixels do
    for (int v=0; v<height_; v++)
    {
        for (int u=0; u<width_; u++)
        {
            // get normalized value
            float val = min(max(disp.data[v*width_ + u]/max_val,0.0f),1.0f);
            // find bin
            int i;
            for (i=0; i<7; i++)
              if (val<cumsum[i+1])
                break;
            // compute red/green/blue values
            float   w = 1.0-(val-cumsum[i])*weights[i];
            uchar r = (uchar)((w*map[i][0]+(1.0-w)*map[i+1][0]) * 255.0);
            uchar g = (uchar)((w*map[i][1]+(1.0-w)*map[i+1][1]) * 255.0);
            uchar b = (uchar)((w*map[i][2]+(1.0-w)*map[i+1][2]) * 255.0);
            cv::Mat dispout = cv::Mat();
            //rgb内存连续存放
            disp.data[v*width_*3 + 3*u + 0] = b;
            disp.data[v*width_*3 + 3*u + 1] = g;
            disp.data[v*width_*3 + 3*u + 2] = r;
        }
    }
    return disp;
}
