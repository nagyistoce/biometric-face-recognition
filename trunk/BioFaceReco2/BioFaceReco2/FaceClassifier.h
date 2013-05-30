#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <opencv2\opencv.hpp>
#include "BioAttributesContainer.h"
#include "FaceData.h"
#include "ClassifierDatabase.h"
#include "Global.h"
#include "Log.h"
#include "FeatureVectorBuilder.h"


class FaceClassifier
{
public:
	FaceClassifier(BioAttributesContainer container, ClassifierDatabase database);
	~FaceClassifier(void);

	FaceData classifyFace(cv::Mat face);

	BioAttributesContainer container;
	ClassifierDatabase database;
private:
	Log * log;

	FeatureVectorBuilder featureBuilder;

};

