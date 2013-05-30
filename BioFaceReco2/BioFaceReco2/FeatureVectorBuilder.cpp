#include "FeatureVectorBuilder.h"


FeatureVectorBuilder::FeatureVectorBuilder(void) {
	log = Global::Instance().getLogger();

	eyes.load(Global::Instance().getProperty("harr_eye_classifier"));
	nose.load(Global::Instance().getProperty("harr_nose_classifier"));
	mouth.load(Global::Instance().getProperty("harr_mouth_classifier"));
}

FeatureVectorBuilder::~FeatureVectorBuilder(void)
{
}

std::vector<float> FeatureVectorBuilder::detectFeatures(const cv::Mat& input) {

	//For saving puropse
	static int imgNumber = 0;

	assert(input.data != NULL);
	
	//Finding face size
	int rightFace, leftFace;
	int size = faceSize(input, leftFace, rightFace);

	//Eyes section
	int eyeLine = findEyesLine(input);

	cv::Mat rEye, lEye;
	cv::Rect rEyeRect, lEyeRect;

	findEyes(input, eyeLine, rEye, lEye, rEyeRect, lEyeRect);

	assert(rEye.data != NULL);
	assert(lEye.data != NULL);

	cv::Point rEyeCenter, lEyeCenter;
	int rEyeRadius, lEyeRadius;

	findEyeCenter(rEye, rEyeCenter, rEyeRadius);
	findEyeCenter(lEye, lEyeCenter, lEyeRadius);

	transformPoint(rEyeCenter, rEyeRect);
	transformPoint(lEyeCenter, lEyeRect);

	//nose
	cv::Mat nose;
	cv::Rect noseRect = findNose(input);
 	input(noseRect).copyTo(nose);

	assert(noseRect.width > 0 && noseRect.height > 0);
	assert(nose.data != NULL);

	cv::Point noseLeft, noseRight;
	findNosePoints(nose, noseLeft, noseRight);

	transformPoint(noseLeft, noseRect);
	transformPoint(noseRight, noseRect);


	//mouth

	cv::Mat mouth;
	cv::Point left, right, up, down;
	cv::Rect mouthRect = findMouth(input);
	input(mouthRect).copyTo(mouth);

	assert(mouthRect.width > 0 && mouthRect.height > 0);
	assert(mouth.data != NULL);

	findMouthPoints(mouth, left, right, up, down);

	transformPoint(left, mouthRect);
	transformPoint(right, mouthRect);
	transformPoint(up, mouthRect);
	transformPoint(down, mouthRect);

#ifdef _DEBUG
	cv::Mat cpy;
	input.copyTo(cpy);

	cv::circle(cpy, lEyeCenter, 4, cv::Scalar(0, 0, 255));
	cv::circle(cpy, rEyeCenter, 4, cv::Scalar(0, 0, 255));
	cv::circle(cpy, noseLeft, 4, cv::Scalar(0, 0, 255));
	cv::circle(cpy, noseRight, 4, cv::Scalar(0, 0, 255));
	cv::circle(cpy, left, 4, cv::Scalar(0, 0, 255));
	cv::circle(cpy, right, 4, cv::Scalar(0, 0, 255));
	cv::circle(cpy, up, 4, cv::Scalar(0, 0, 255));
	cv::circle(cpy, down, 4, cv::Scalar(0, 0, 255));

	std::stringstream ss;
	ss << "img" << imgNumber << ".jpg";
	cv::imwrite(ss.str(), cpy);
	imgNumber++;
#endif

	//wektor cech
	float x1, x2, x3, x4, x5, x6, x7;

	//odlegosc miedzy srodkami oczy
	x1 = distance(lEyeCenter, rEyeCenter);

	//szerokosc nosa
	x2 = distance(noseLeft, noseRight);

	//szerokosc ust
	x3 = distance(left, right);

	//wysokosc ust
	x4 = distance(up, down);

	//szerokosc twarzy
	x5 = (float)size;

	//odgleglosc miedzy linia oczu a lina nosa
	float tmp1, tmp2;
	tmp1 = distance(lEyeCenter, noseLeft);
	tmp2 = distance(rEyeCenter, noseRight);
	x6 = (tmp1 + tmp2) / 2.0f;

	//odleglosc miedzy linia oczu a linia ust
	tmp1 = distance(lEyeCenter, left);
	tmp2 = distance(rEyeCenter, right);
	x7 = (tmp1 + tmp2) / 2.0f;

	std::vector<float> characteristic;
	characteristic.push_back(x1/x2);
	characteristic.push_back(x1/x3);
	characteristic.push_back(x2/x3);
	characteristic.push_back(x1/x5);
	characteristic.push_back(x1/x6);
	characteristic.push_back(x1/x7);
	characteristic.push_back(x4/x5);

	return characteristic;
}

void FeatureVectorBuilder::createSingleHistogramImage(cv::Mat& histogram, cv::Mat& output, int histSize, int histW, int histH) {
	int bin_w = cvRound((double) histW / histSize);

	cv::Mat histImage(histH, histW, CV_8UC3, cv::Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	cv::normalize(histogram, histogram, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++) {
		cv::line(histImage, cv::Point(bin_w * (i - 1), histH - cvRound(histogram.at<float>(i - 1))),
				cv::Point(bin_w * (i), histH - cvRound(histogram.at<float>(i))), cv::Scalar(255, 0, 0), 2, 8, 0);
	}

	histImage.copyTo(output);
}

void FeatureVectorBuilder::createProjectionHistogram(cv::Mat& projection, cv::Mat& output, int outSize, int type) {

	if (type == 0) {
		cv::Mat histImage(outSize, projection.cols, CV_8UC1, cv::Scalar(0, 0, 0));
		cv::normalize(projection, projection, 0, outSize, cv::NORM_MINMAX, -1, cv::Mat());

		for (int i = 1; i < projection.cols; i++) {
			float val = projection.at<float>(0, i);
			cv::circle(histImage, cv::Point(i, val), 1, cv::Scalar(255, 255, 255), 1, 1, 0);
		}
		cv::flip(histImage, histImage, 0);
		histImage.copyTo(output);
	}

	if (type == 1) {
		cv::Mat histImage(projection.rows, outSize, CV_8UC1, cv::Scalar(0, 0, 0));
		cv::normalize(projection, projection, 0, outSize, cv::NORM_MINMAX, -1, cv::Mat());

		for (int i = 1; i < projection.rows; i++) {
			float val = projection.at<float>(i, 0);

			cv::circle(histImage, cv::Point(val, i), 1, cv::Scalar(255, 255, 255), 1, 1, 0);
		}
		histImage.copyTo(output);
	}
}

void FeatureVectorBuilder::createHistogram(cv::Mat input, cv::Mat& output) {
	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true;
	bool accumulate = false;

	cv::Mat b_hist;

	/// Compute the histograms:
	cv::calcHist(&input, 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	cv::Mat histogram;
	createSingleHistogramImage(b_hist, histogram, histSize);

	histogram.copyTo(output);
}

void FeatureVectorBuilder::createProjections(const cv::Mat& input, cv::Mat& hProjection, cv::Mat& vProjection) {
	cv::reduce(input, hProjection, 0, CV_REDUCE_SUM, CV_32FC1);
	cv::reduce(input, vProjection, 1, CV_REDUCE_SUM, CV_32FC1);

}

int FeatureVectorBuilder::faceSize(const cv::Mat& faceImage, int& leftLine, int& rightLine) {
	double min, max;
	cv::Mat hProjection;
	cv::Point minLoc, maxLoc;
	cv::Point minLoc2, maxLoc2;

	cv::Mat cpy;
	faceImage.copyTo(cpy);

	std::vector<cv::Mat> splits;
	cv::cvtColor(cpy, cpy, CV_BGR2HSV);
	cv::split(cpy, splits);
	cpy = splits.at(1);

	//cv::blur(cpy, cpy, cv::Size(3,3));
	cv::Sobel(cpy, cpy, cpy.depth(), 1, 0);

	cv::reduce(cpy, hProjection, 0, CV_REDUCE_SUM, CV_32FC1);

	cv::Mat m1 = hProjection.colRange(0, hProjection.cols / 4);
	cv::Mat m2 = hProjection.colRange(hProjection.cols - hProjection.cols / 4, hProjection.cols);

	cv::minMaxLoc(m1, &min, &max, &minLoc, &maxLoc);
	cv::minMaxLoc(m2, &min, &max, &minLoc2, &maxLoc2);

	rightLine = maxLoc2.x + hProjection.cols - hProjection.cols / 4;
	leftLine = maxLoc.x;

	return rightLine - leftLine;
}

cv::Rect FeatureVectorBuilder::findNose(const cv::Mat& input) {

	std::vector<cv::Rect> noseROI;
	int third = input.rows / 3;

	//cv::Rect middle(0, third + third / 3, input.cols, third);
	//cv::Mat middleFace;
	//input(middle).copyTo(middleFace);
	nose.detectMultiScale(input, noseROI, 1.2, 3, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FEATURE_MAX | CV_HAAR_FIND_BIGGEST_OBJECT);

	if (noseROI.size() != 0) {
		cv::Rect r = noseROI.at(0);
		//r.x; += middle.x - 5;
		//r.y; += middle.y - 5;
		//r.width += 10;
		//r.height += 10;
		return r;
	}

	return cv::Rect();
}

int FeatureVectorBuilder::findEyesLine(const cv::Mat& input) {

	cv::Mat img;
	cv::Mat vProjection;

	cv::cvtColor(input, img, CV_BGR2GRAY);
	cv::Rect r(0, 0, img.cols, img.rows / 2);
	img(r).copyTo(img);

	cv::Sobel(img, img, input.depth(), 1, 0, 3);
	cv::reduce(img, vProjection, 1, CV_REDUCE_SUM, CV_32FC1);

	double min, max;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(vProjection, &min, &max, &minLoc, &maxLoc);
	return maxLoc.y;
}

void FeatureVectorBuilder::findEyes(const cv::Mat& input, int eyeLine, cv::Mat& outRightEye, cv::Mat& outLeftEye, cv::Rect& rectRightEye, cv::Rect& rectLeftEye) {
	std::vector<cv::Rect> eyesROI;
	cv::Mat eyesRegion;
	cv::Rect upper;

	if (eyeLine == 0) {
		int third = input.rows / 3;
		upper = cv::Rect(0, third - third / 3, input.cols, third);
		input(upper).copyTo(eyesRegion);
	} else {
		int val = 0.12f * input.rows;
		upper = cv::Rect(0, eyeLine - val, input.cols, (2 * val));
		input(upper).copyTo(eyesRegion);
	}

	//cv::imshow("eyes", eyesRegion);
	//eyes.detectMultiScale(eyesRegion, eyesROI, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE);
	eyes.detectMultiScale(input, eyesROI, 1.2, 3, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FEATURE_MAX);

	int mid = input.cols / 2;
	bool leftEye = false, rightEye = false;

	for(int i = 0; i < eyesROI.size(); i++) {
		cv::Rect r = eyesROI.at(i);
		//r.x += upper.x;
		//r.y += upper.y;

		if(!leftEye && r.x < mid) {
			input(r).copyTo(outLeftEye);
			rectLeftEye = r;
			leftEye = true;
		}

		if(!rightEye && r.x >= mid) {
			input(r).copyTo(outRightEye);
			rectRightEye = r;
			rightEye = true;
		}

		if(leftEye && rightEye) {
			break;
		}
	}

	//if (eyesROI.size() >= 2) {
	//	cv::Rect r1 = eyesROI.at(0);
	//	cv::Rect r2 = eyesROI.at(1);

	//	r1.x += upper.x;
	//	r1.y += upper.y; // - 5;
	//	//r1.width += 10;
	//	//r1.height += 10;

	//	r2.x += upper.x;
	//	r2.y += upper.y; // - 5;
	//	//r2.width += 10;
	//	//r2.height += 10;

	//	if (r1.x < r2.x) {
	//		input(r1).copyTo(outLeftEye);
	//		input(r2).copyTo(outRightEye);

	//		rectLeftEye = r1;
	//		rectRightEye = r2;
	//	} else {
	//		input(r2).copyTo(outLeftEye);
	//		input(r1).copyTo(outRightEye);

	//		rectLeftEye = cv::Rect(r2);
	//		rectRightEye = cv::Rect(r1);
	//	}
	//}
}

void FeatureVectorBuilder::findEyeCenter(const cv::Mat& input, cv::Point& center, int& radius) {
	
	cv::Mat cpy, corners;
	input.copyTo(cpy);

	if(cpy.channels() != 1) {
		cv::cvtColor(cpy, cpy, CV_BGR2GRAY);
	}

	cv::goodFeaturesToTrack(cpy, corners, 1, 0.1, 2.0);
	center = cv::Point(corners.at<cv::Vec2f>(0, 0));
	
}

cv::Rect FeatureVectorBuilder::findMouth(const cv::Mat& input) {
	std::vector<cv::Rect> mouthROI;

	//int third = input.rows / 3;

	//cv::Rect lower(0, third * 2, input.cols, third);
	//cv::Mat lowerFace;

	//input(lower).copyTo(lowerFace);
	mouth.detectMultiScale(input, mouthROI, 1.2, 3, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FEATURE_MAX | CV_HAAR_FIND_BIGGEST_OBJECT);

	if (mouthROI.size() != 0) {
		cv::Rect r = mouthROI.at(0);
		//r.x += lower.x - 10;
		//r.y += lower.y - 10;
		//r.width += 20;
		//r.height += 20;
		return r;
	}

	return cv::Rect();
}

void FeatureVectorBuilder::changeContrastBrightness(cv::Mat& input, double contrast, int brightness) {
	for( int y = 0; y < input.rows; y++ ) {
		for( int x = 0; x < input.cols; x++ ) {
			if(input.channels() == 3) {
				for( int c = 0; c < 3; c++ ) { 
					input.at<cv::Vec3b>(y,x)[c] = cv::saturate_cast<uchar>( contrast *( input.at<cv::Vec3b>(y,x)[c] ) + brightness );
				}
			} else {
				input.at<uchar>(y,x) = cv::saturate_cast<uchar>( contrast *( input.at<uchar>(y,x)) + brightness );
			}
		}
	}
}

void FeatureVectorBuilder::findMouthPoints(const cv::Mat& input, cv::Point& left, cv::Point& right, cv::Point& up, cv::Point& down) {
	cv::Mat mouth;
	input.copyTo(mouth);

	//changeContrastBrightness(mouth, 1.1, 0);

	if(mouth.channels() != 1) {
		cv::cvtColor(mouth, mouth, CV_BGR2GRAY);
	}

	cv::equalizeHist(mouth, mouth);
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * 2 + 1, 2 * 2 + 1), cv::Point(2, 2));
	cv::morphologyEx(mouth, mouth, cv::MORPH_OPEN, element);
	//cv::erode(mouth, mouth, element);
	cv::blur(mouth, mouth, cv::Size(7,7));
	//cv::adaptiveThreshold(mouth, mouth, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 9, 10);
	cv::threshold(mouth, mouth, 50, 255, CV_THRESH_BINARY);
	

	bool rFound = false;
	bool lFound = false;

	//image scanning
	for(int x = 0; x < mouth.cols / 2; x++) {
		cv::Mat fromLeft = mouth.col(x);
		cv::Mat fromRight = mouth.col(mouth.cols - x - 1);

		if(!lFound) {
			for(int y = 0; y < fromLeft.rows; y++) {
				if(fromLeft.at<uchar>(y, 0) == 0) {
					left.x = x;
					left.y = y;
					lFound = true;
				}
			}
		}

		if(!rFound) {
			for(int y = 0; y < fromRight.rows; y++) {
				if(fromRight.at<uchar>(y, 0) == 0) {
					right.x = mouth.cols - x - 1;
					right.y = y;
					rFound = true;
				}
			}
		}
	}


	if(!rFound) {
		right.x = 0;
		right.y = mouth.rows / 2;
	}

 	if(!lFound) {
		left.x = mouth.cols;
		left.y = mouth.rows / 2;
	}

	int mxMiddle = ((right.x - left.x) / 2) + left.x;

	cv::Mat vProjection;
	double min, max;
	cv::Point minLoc, maxLoc;
	cv::Point minLoc2, maxLoc2;

	cv::reduce(mouth, vProjection, 1, CV_REDUCE_SUM, CV_32FC1);

	cv::Mat m1 = vProjection.rowRange(0, vProjection.rows / 2);
	cv::Mat m2 = vProjection.rowRange(vProjection.rows / 2, vProjection.rows);

	cv::minMaxLoc(m1, &min, &max, &minLoc, &maxLoc);
	cv::minMaxLoc(m2, &min, &max, &minLoc2, &maxLoc2);

	up.x = mxMiddle;
	up.y = minLoc.y;

	down.x = mxMiddle;
	down.y = minLoc2.y + vProjection.rows / 2;
	
}

void FeatureVectorBuilder::transformPoint(cv::Point& p, const cv::Rect& r) {
	p.x += r.x;
	p.y += r.y;
}

void FeatureVectorBuilder::findNosePoints(const cv::Mat& input, cv::Point& left, cv::Point& right) {

	cv::Mat nose;
	input.copyTo(nose);

	if(nose.channels() != 1) {
		cv::cvtColor(nose, nose, CV_BGR2GRAY);
	}

	cv::blur(nose, nose, cv::Size(3,3));
	cv::adaptiveThreshold(nose, nose, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 9, 10);

	bool rFound = false;
	bool lFound = false;

	//image scanning
	for(int x = 0; x < nose.cols / 2; x++) {
		cv::Mat fromLeft = nose.col(x);
		cv::Mat fromRight = nose.col(nose.cols - x - 1);

		if(!lFound) {
			for(int y = 0; y < fromLeft.rows; y++) {
				if(fromLeft.at<uchar>(y, 0) == 0) {
					left.x = x;
					left.y = y;
					lFound = true;
				}
			}
		}

		if(!rFound) {
			for(int y = 0; y < fromRight.rows; y++) {
				if(fromRight.at<uchar>(y, 0) == 0) {
					right.x = nose.cols - x - 1;
					right.y = y;
					rFound = true;
				}
			}
		}
	}

	if(!rFound) {
		right.x = 0;
		right.y = nose.rows / 2;
	}

 	if(!lFound) {
		left.x = nose.cols;
		left.y = nose.rows / 2;
	}

	//cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1 * 2 + 1, 1 * 2 + 1), cv::Point(1, 1));
	//cv::blur(nose, nose, cv::Size(2, 2));
	//cv::morphologyEx(nose, nose, cv::MORPH_CLOSE, element);

	//cv::dilate(nose, nose, element);

		//cv::morphologyEx(nose, nose, cv::MORPH_CLOSE, element);

}

float FeatureVectorBuilder::distance(const cv::Point a, const cv::Point b) {
	float xd = b.x - a.x;
	float yd = b.y - a.y;

	return std::sqrtf((xd * xd) + (yd * yd));
}