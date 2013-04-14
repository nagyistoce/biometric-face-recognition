#pragma once

#include <vector>
#include <opencv2\opencv.hpp>
#include "Global.h"

class EyeFinder
{
public:
	EyeFinder(void);
	~EyeFinder(void);

	std::vector<cv::Point2f> findInImage(cv::Mat img);

private:
	cv::CascadeClassifier haarClassifier;
	bool success;
};

