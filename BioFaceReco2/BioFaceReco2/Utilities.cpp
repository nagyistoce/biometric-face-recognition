#include "Utilities.h"


Utilities::Utilities(void)
{
}


Utilities::~Utilities(void)
{
}

void Utilities::cropFace(cv::Mat& image, cv::Point2d eyeLeft, cv::Point2d eyeRight,cv::Point2d offsetPct, cv::Size destSize) {

	cv::Mat test;
	image.copyTo(test);
	cv::circle(test, eyeLeft, 5, cv::Scalar(255, 0, 0)); 
	cv::circle(test, eyeRight, 5, cv::Scalar(255, 0, 0));
	std::stringstream ss;
	ss << eyeLeft.x << "_" << eyeLeft.y;
	cv::imshow(ss.str(), test);

	double offsetH = std::floor(offsetPct.x * (double)destSize.width);
	double offsetV = std::floor(offsetPct.y * (double)destSize.height);

	cv::Point2d eyeDirection((eyeRight.x - eyeLeft.x), (eyeRight.y - eyeLeft.y));
	
	double rotation = -std::atan2(eyeDirection.y, eyeDirection.x);
	double dist = distance(eyeLeft, eyeRight);

	double reference = destSize.width - 2.0 * offsetH;
	double scale = dist / reference;

	scaleRotateTranslate(image, rotation, eyeLeft, cv::Point2d(-1.0,-1.0), 0, cv::INTER_CUBIC);

	cv::Point cropXY(eyeLeft.x - scale * offsetH, eyeLeft.y - scale	* offsetV);
	cv::Size cropSize(destSize.width * scale, destSize.height * scale);

	cv::Rect roi(cropXY.x, cropXY.y, (cropXY.x + cropSize.width), (cropXY.y + cropSize.height));
	//image = image(roi);

	cv::resize(image, image, destSize, cv::INTER_LINEAR);
}

double Utilities::distance(cv::Point2d p1, cv::Point2d p2) {
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	return std::sqrt(dx * dx + dy * dy);
}

void Utilities::scaleRotateTranslate(cv::Mat& image, double angle, cv::Point2d center, cv::Point2d newCenter, double scale, int resample) {

	if (scale == 0 && center.x) {
			return imageRotate(image, cv::Point(0, 0), angle, 1.0, resample);
		}

		double nx = center.x;
		double ny = center.y;
		double x = center.x;
		double y = center.y;
		double sx = 1.0;
		double sy = 1.0;

		if (newCenter.x != -1.0) {
			nx = newCenter.x;
			ny = newCenter.y;
		}

		if (scale != 0) {
			sx = scale;
			sy = scale;
		}

		double cosine = std::cos(angle);
		double sine = std::sin(angle);
		double a = cosine / sx;
		double b = sine / sx;
		double c = x - nx * a - ny * b;
		double d = -sine / sy;
		double e = cosine / sy;
		double f = y - nx * d - ny * e;

		cv::Mat trans(2, 3, CV_32FC1);
		
		trans.at<float>(0,0) = a;
		trans.at<float>(0,1) = b;
		trans.at<float>(0,2) = c;
		trans.at<float>(1,0) = d;
		trans.at<float>(1,1) = e;
		trans.at<float>(1,2) = f;

		transform(image, image.size(), trans, resample);
}

void Utilities::imageRotate(cv::Mat& image, cv::Point2d center, double angle, double scale, int resample) {
	cv::Mat rotMat = cv::getRotationMatrix2D(center, angle, scale);
	cv::warpAffine(image, image, rotMat, image.size());
}

void Utilities::transform(cv::Mat& image, cv::Size size, cv::Mat transform, int resample) {
	cv::warpAffine(image, image, transform, size);
}