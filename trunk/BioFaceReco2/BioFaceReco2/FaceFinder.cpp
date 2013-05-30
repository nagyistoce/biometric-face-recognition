#include "FaceFinder.h"


FaceFinder::FaceFinder(void) {
	faces.clear();
	std::string classFilename = Global::Instance().getProperty("harr_face_classifier");
	success = haarClassifier.load(classFilename);
	log = Global::Instance().getLogger();
}

FaceFinder::~FaceFinder(void) {
}

std::vector<cv::Mat> FaceFinder::findInImage(cv::Mat image) {
	
	faces.clear();
	std::vector<cv::Rect> facesRect;

	if(!success) {
		log->Write(ERROR, "Wrong cascade classifier filename");
	} else {
		if(!image.empty()) {
			cv::Mat gray;
			image.copyTo(gray);
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