#include "feature.h"

Feature::Feature(){}

Feature::Feature(const cv::Mat& _src)
{
    src = _src;
}

void Feature::read(cv::Mat& _src){src = _src;}

void Feature::detect(int type)
{
    switch (type)
    {
    case int(DETECTOR_TYPE::SIFT):
    {
        cv::Ptr<cv::xfeatures2d::SiftFeatureDetector> detector = cv::xfeatures2d::SIFT::create();
        detector->detect(src, keypoints);
        break;
    }
    case int(DETECTOR_TYPE::SURF):
    {
        cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> detector = cv::xfeatures2d::SURF::create();
        detector->detect(src, keypoints);
        break;
    }
    case int(DETECTOR_TYPE::ORB):
    {
        cv::Ptr<cv::ORB> detector = cv::ORB::create();
        detector->detect(src, keypoints);
        break;
    }
    default:
        break;
    }
}

void Feature::describe(int type)
{
    switch (type)
    {
    case int(DESCRIPTOR_TYPE::SIFT):
    {
        cv::Ptr<cv::xfeatures2d::SiftDescriptorExtractor> extractor = cv::xfeatures2d::SIFT::create();
        extractor->compute(src, keypoints, descriptors);
        break;
    }
    case int(DESCRIPTOR_TYPE::SURF):
    {
        cv::Ptr<cv::xfeatures2d::SurfDescriptorExtractor> extractor = cv::xfeatures2d::SurfDescriptorExtractor::create();
        extractor->compute(src, keypoints, descriptors);
        break;
    }
    case int(DETECTOR_TYPE::ORB):
    {
        cv::Ptr<cv::ORB> extractor = cv::ORB::create();
        extractor->compute(src, keypoints, descriptors);descriptors.t();
        break;
    }
    default:
        break;
    }
}

cv::Mat Feature::showKeypoints()
{
    cv::Mat imageKeypoints;
    cv::drawKeypoints(src, keypoints, imageKeypoints,
                      cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
    return imageKeypoints;
}

void Feature::reset()
{
    src = cv::Mat();
    keypoints.clear();
    descriptors = cv::Mat();
}
