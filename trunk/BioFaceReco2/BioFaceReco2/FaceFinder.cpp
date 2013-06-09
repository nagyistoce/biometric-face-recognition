#include "FaceFinder.h"


FaceFinder::FaceFinder(void) {
	faces.clear();
	std::string classFilename = Global::Instance().getProperty("harr_face_classifier");
	success = haarClassifier.load(classFilename);
	log = Global::Instance().getLogger();
}

FaceFinder::~FaceFinder(void) {
}

std::vector<cv::Mat> FaceFinder::findInImage(cv::Mat& input) {
	
	cv::Mat image;

	if(input.channels() != 1) {
		cv::cvtColor(input, image, CV_BGR2GRAY);
	} else {
		input.copyTo(image);
	}

	faces.clear();
	std::vector<cv::Rect> facesRect;

	if(!success) {
		log->Write(ERROR, "Wrong cascade classifier filename");
	} else {
		if(!image.empty()) {
			cv::Mat gray;
			image.copyTo(gray);
			cv::equalizeHist(gray, gray);

			haarClassifier.detectMultiScale(gray, facesRect, 1.2, 3, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT);

			for(int i = 0; i < facesRect.size(); i++) {
				cv::Mat faceROI = input(facesRect[i]);
				this->faces.push_back(faceROI);
			}
		}
	}

	return this->faces;

}