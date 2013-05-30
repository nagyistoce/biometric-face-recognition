#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <opencv2\opencv.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "Global.h"
#include "Log.h"

class FaceFinder
{
public:
	FaceFinder(void);
	~FaceFinder(void);
	std::vector<cv::Mat> findInImage(cv::Mat image);
	std::vector<cv::Mat> getFacesVector() const;

private:
	cv::CascadeClassifier haarClassifier;
	std::vector<cv::Mat> faces;
	bool success;
	Log * log;

};

inline std::vector<cv::Mat> FaceFinder::getFacesVector() const {
	return faces;
}
