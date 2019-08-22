#include "matcher.h"

//Matcher::Matcher(){}

Matcher::Matcher(int _detectorType, int _descriptorType,
                 int _matchingType, int _metric, int _numMatchedPairs, int _refineMethod, int _reprjError)
{
    detectorType = _detectorType;
    descriptorType = _descriptorType;
    matchingType = _matchingType;
    metric = _metric;
    numMatchedPairs = _numMatchedPairs;
    refineMethod = _refineMethod;
    reprjError = _reprjError;
}

void Matcher::templateDetect() {feature1.detect(detectorType);}

void Matcher::templateDescribe() {feature1.describe(descriptorType);}

void Matcher::sceneDetect() {feature2.detect(detectorType);}

void Matcher::sceneDescribe() {feature2.describe(descriptorType);}

void Matcher::match()
{
    switch (matchingType)
    {
    case int(MATCHING_TYPE::BM):
    {
        cv::BFMatcher matcher(metric);
        matcher.match(feature1.descriptors, feature2.descriptors, matches);
        break;
    }
    case int(MATCHING_TYPE::FLANN):
    {
        cv::FlannBasedMatcher matcher;
        matcher.match(feature1.descriptors, feature2.descriptors, matches);
        break;
    }
    default:
        break;
    }
}

void Matcher::refine()
{
    goodMatches.clear();

    std::sort(matches.begin(), matches.end());
    int num = matches.size() >= numMatchedPairs ? numMatchedPairs : matches.size();
    for (int i = 0; i < num; i++)
    {
        goodMatches.push_back(matches[i]);
    }
}

void Matcher::calHomoMat()
{
    try
    {
        std::vector<cv::Point2f> templatePoints, scenePoints;
        for(int i = 0; i < goodMatches.size(); i++)
        {
            templatePoints.push_back(feature1.keypoints[goodMatches[i].queryIdx].pt);
            scenePoints.push_back(feature2.keypoints[goodMatches[i].trainIdx].pt);
        }
        H = cv::findHomography(templatePoints, scenePoints, refineMethod, reprjError);
    }
    catch (const std::exception& e)
    {
        H = cv::Mat();
        std::cerr << e.what() << std::endl;
    }
}

void Matcher::showMatchingBox()
{
    if(H.dims == 0)
        return;

    int width = cvRound(std::size(feature1.src).width);
    int height = cvRound(std::size(feature1.src).height);

    std::vector<cv::Point2f> templateCorners(4);
    templateCorners[0] = cv::Point2f(0, 0);
    templateCorners[1] = cv::Point2f(width, 0);
    templateCorners[2] = cv::Point2f(width, height);
    templateCorners[3] = cv::Point2f(0, height);

    std::vector<cv::Point2f> sceneCorners(4);
    cv::perspectiveTransform(templateCorners, sceneCorners, H);

    line(imageGoodMatches, sceneCorners[0] + cv::Point2f(width, 0), sceneCorners[1] + cv::Point2f(width, 0), cv::Scalar(0, 255, 0), 4);
    line(imageGoodMatches, sceneCorners[1] + cv::Point2f(width, 0), sceneCorners[2] + cv::Point2f(width, 0), cv::Scalar(0, 255, 0), 4);
    line(imageGoodMatches, sceneCorners[2] + cv::Point2f(width, 0), sceneCorners[3] + cv::Point2f(width, 0), cv::Scalar(0, 255, 0), 4);
    line(imageGoodMatches, sceneCorners[3] + cv::Point2f(width, 0), sceneCorners[0] + cv::Point2f(width, 0), cv::Scalar(0, 255, 0), 4);

    try {
        if(coordinates.empty())
        {
            qDebug() << "'coordinates' is empty!";
            return;
        }

        for(int i = 0; i < sceneCorners.size(); i++)
        {
            if(sceneCorners[i].x < 10 || sceneCorners[i].x >= feature2.src.cols-10 ||
                    sceneCorners[i].y < 10 || sceneCorners[i].y >= feature2.src.rows-10)
            {
                qDebug() << "coordinates are out of range!";
                return;
            }
        }
        cv::Point2f center = (sceneCorners[0] + sceneCorners[1] + sceneCorners[2] + sceneCorners[3])/4;

        qDebug() << "center point" << center.x << center.y;

        int num = 49;
        cv::Vec3f arg = cv::Vec3f(0,0,0);
        for(int i = 0; i < 7; i++)
        {
            for(int j = 0; j < 7; j++)
            {
                cv::Vec3f point = coordinates.at<cv::Vec3f>(int(i+center.y-3), int(i+center.x-3));
                if(point[2]>100000)
                    num -= 1;
                else
                    arg += point;
            }
        }
        if(num == 0)
        {
            qDebug() << "No valid data!";
            return;
        }
        arg = arg / num;
        qDebug() << arg[0] << arg[1] << arg[2];
    } catch (std::exception&) {}
}

void Matcher::showGoodMatches()
{
    drawMatches(feature1.src, feature1.keypoints, feature2.src, feature2.keypoints,
                goodMatches, imageGoodMatches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); //定位“好”的匹配点
}

