#pragma once

#include <vector>
#include <string>
#include <opencv2\opencv.hpp>
#include "BioAttributesContainer.h"
#include "FaceData.h"
#include "ClassifierDatabase.h"

class FaceClassifier
{
public:
	FaceClassifier(BioAttributesContainer container, ClassifierDatabase database);
	~FaceClassifier(void);

	FaceData classifyFace(cv::Mat face);

	BioAttributesContainer container;
	ClassifierDatabase database;

};

