#include "EyeFinder.h"


EyeFinder::EyeFinder(void)
{
	std::string classFilename = Global::Instance().getProperty("harr_eye_classifier");
	success = haarClassifier.load(classFilename);

}


EyeFinder::~EyeFinder(void)
{
}

std::vector<cv::Point2f> EyeFinder::findInImage(cv::Mat img) {
	
	std::vector<cv::Rect> eyesRect;
	std::vector<cv::Point2f> ret;
	
	ret.clear();
	eyesRect.clear();

	if(!success) {
		std::cout << "Wrong cascade classifier filename" << std::endl;
	} else {
		if(!img.empty()) {

			cv::Mat gray;
			img.copyTo(gray);
			//cv::cvtColor(img, gray, CV_BGR2GRAY);
			cv::equalizeHist(gray, gray);

			haarClassifier.detectMultiScale(gray, eyesRect, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE);

			if(eyesRect.size() == 2) {
				for(int i = 0; i < eyesRect.size(); i++) {
					cv::Rect eyeROI = eyesRect.at(i);
					float centerX = eyeROI.x + (eyeROI.width/2.0f);
					float centerY = eyeROI.y + (eyeROI.height/2.0f);
					cv::Point2f point(centerX, centerY);
					ret.push_back(point);
				}
			} else {
				std::cout << "Cannot find eyes in image" << std::endl;
			}
		}
	}

	return ret;

}