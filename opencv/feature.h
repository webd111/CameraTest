#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "myvariable.h"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"

enum class DETECTOR_TYPE{SIFT,SURF,ORB};
enum class DESCRIPTOR_TYPE{SIFT,SURF,ORB};

class Feature
{
public:
    cv::Mat src;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    Feature();
    Feature(const cv::Mat&);
    void read(cv::Mat&);
    void detect(int type = int(DETECTOR_TYPE::SIFT));
    void describe(int type = int(DETECTOR_TYPE::SIFT));
    cv::Mat showKeypoints();
    void reset();
};

#endif // DESCRIPTOR_H
