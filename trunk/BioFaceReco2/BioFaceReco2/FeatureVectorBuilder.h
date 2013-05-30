#pragma once

#include <vector>
#include <string>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include "Log.h"
#include "Global.h"
#include <cassert>

class FeatureVectorBuilder
{
public:
	FeatureVectorBuilder(void);
	~FeatureVectorBuilder(void);

	std::vector<float> detectFeatures(const cv::Mat& input);

private:
	Log * log;
	bool success;
	cv::CascadeClassifier eyes;
	cv::CascadeClassifier nose;
	cv::CascadeClassifier mouth;

	void createSingleHistogramImage(cv::Mat& histogram, cv::Mat& output, int histSize, int histW = 512, int histH = 400);
	void createProjectionHistogram(cv::Mat& projection, cv::Mat& output, int outSize, int type);
	void createProjections(const cv::Mat& input, cv::Mat& hProjection, cv::Mat& vProjection);
	void changeContrastBrightness(cv::Mat& input, double contrast, int brightness);

	int faceSize(const cv::Mat& faceImage, int& leftLine, int& rightLine);
	void createHistogram(cv::Mat input, cv::Mat& output);

	cv::Rect findNose(const cv::Mat& input);

	int findEyesLine(const cv::Mat& input);
	void findEyes(const cv::Mat& input, int eyeLine, cv::Mat& outRightEye, cv::Mat& outLeftEye, cv::Rect& rectRightEye, cv::Rect& rectLeftEye);
	void findEyeCenter(const cv::Mat& input, cv::Point& center, int& radius);

	cv::Rect findMouth (const cv::Mat& input);
	void findMouthPoints(const cv::Mat& input, cv::Point& left, cv::Point& right, cv::Point& up, cv::Point& down);

	void transformPoint(cv::Point& p, const cv::Rect& r);

	void findNosePoints(const cv::Mat& input, cv::Point& left, cv::Point& right);

	float distance(const cv::Point a, const cv::Point b);
};

