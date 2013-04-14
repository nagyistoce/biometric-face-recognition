#include "FaceFinder.h"


FaceFinder::FaceFinder(void) {
	faces.clear();
	std::string classFilename = Global::Instance().getProperty("harr_face_classifier");
	success = haarClassifier.load(classFilename);
}

FaceFinder::~FaceFinder(void)
{
	faces.clear();
}

std::vector<cv::Mat> FaceFinder::findInImage(cv::Mat image) {
	
	faces.clear();
	std::vector<cv::Rect> facesRect;

	if(!success) {
		std::cout << "Wrong cascade classifier filename" << std::endl;
	} else {
		if(!image.empty()) {

			cv::Mat gray;
			cv::cvtColor(image, gray, CV_BGR2GRAY);
			cv::equalizeHist(gray, gray);

			haarClassifier.detectMultiScale(gray, facesRect, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE);

			for(int i = 0; i < facesRect.size(); i++) {
				cv::Mat faceROI = image(facesRect[i]);
				this->faces.push_back(faceROI);
			}

		}
	}

	return this->faces;

}