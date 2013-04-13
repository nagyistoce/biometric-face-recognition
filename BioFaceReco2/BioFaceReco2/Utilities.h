#pragma once

#include <cmath>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class Utilities
{
public:
	Utilities(void);
	~Utilities(void);

	static void cropFace(cv::Mat& image, cv::Point2d eyeLeft, cv::Point2d eyeRight,cv::Point2d offsetPct, cv::Size destSize);
	static double distance(cv::Point2d p1, cv::Point2d p2);
	static void scaleRotateTranslate(cv::Mat& image, double angle, cv::Point2d center, cv::Point2d newCenter, double scale, int resample);
	static void imageRotate(cv::Mat& image, cv::Point2d center, double angle, double scale, int resample);
	static void transform(cv::Mat& image, cv::Size size, cv::Mat transform, int resample);
};

