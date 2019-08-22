#ifndef MATCHER_H
#define MATCHER_H

#include "feature.h"
//#include "matchingthread.h"
#include "halcon/myhalcon.h"
#include "opencv2/calib3d/calib3d.hpp"

#include <QThread>
#include <QTime>

enum class MATCHING_TYPE{BM,FLANN};

class Matcher
{
public:
    Feature feature1;           // used for template
    Feature feature2;           // used for scene
    std::vector<cv::DMatch> matches;
    std::vector<cv::DMatch> goodMatches;
    cv::Mat imageGoodMatches;
    cv::Mat H;

    int detectorType = int(DETECTOR_TYPE::SIFT);
    int descriptorType = int(DESCRIPTOR_TYPE::SIFT);
    int matchingType = int(MATCHING_TYPE::BM);
    int metric = cv::NORM_L2;
    int numMatchedPairs = 100;
    int refineMethod = cv::RANSAC;
    int reprjError = 3;

    Matcher(int, int, int, int, int, int, int);

    void templateDetect();       // Detect template keypoints
    void templateDescribe();     // Describe template keypoints
    void sceneDetect();          // Detect scene keypoints
    void sceneDescribe();        // Describe scene keypoints
    void match();
    void refine();                  // Refine match pairs
    void calHomoMat();              // Calculate homography matrix between two pictures
    void showGoodMatches();         // Show good match pairs
    void showMatchingBox();         // Show matching box
};

#endif // MATCHER_H
