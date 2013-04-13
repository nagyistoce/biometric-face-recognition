#include "FaceFinder.h"


FaceFinder::FaceFinder(void) {
	faces.clear();
}

FaceFinder::~FaceFinder(void)
{
	faces.clear();
}

std::vector<cv::Mat> FaceFinder::findInFile(cv::Mat image) {
	
	std::vector<cv::Rect> facesRect;
	std::string classFilename = Global::Instance().getProperty("harr_face_classifier");
	faces.clear();
	bool success = false;

	success = haarClassifier.load(classFilename);

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