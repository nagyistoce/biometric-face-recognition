#pragma once

#include <string>
#include <map>
#include <opencv2\opencv.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <tinyxml2.h>
#include "Global.h"
#include "BioAttributesContainer.h"
#include "FaceData.h"
#include "Log.h"
#include "FeatureVectorBuilder.h"

using namespace tinyxml2;

class ClassifierDatabase {
public:
	ClassifierDatabase(BioAttributesContainer container);
	~ClassifierDatabase(void);

	std::map<std::string, cv::Ptr<cv::FaceRecognizer>> trainedRecognizers;

	void train();

private:
	Log * log;
	FaceData parse(XMLElement * e, std::string folder, std::vector<std::string>& values);
	BioAttributesContainer container;
	FeatureVectorBuilder featuresBuilder;

	cv::Mat norm_0_255(cv::InputArray _src);
};

