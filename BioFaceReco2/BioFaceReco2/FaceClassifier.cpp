#include "FaceClassifier.h"


FaceClassifier::FaceClassifier(BioAttributesContainer container, ClassifierDatabase database) 
	:container(container), database(database)
{
	log = Global::Instance().getLogger();
}


FaceClassifier::~FaceClassifier(void) {
}


FaceData FaceClassifier::classifyFace(cv::Mat face) {
	log->Write(INFO, "FaceClassifier: Classifying image");
	int featureVecSize = atoi(Global::Instance().getProperty("featureVecSize").c_str());

	FaceData fd;
	std::vector<std::string> values;

	//make list of values to read
	std::vector<BioAttributeInfo>::iterator it;
	for(it = container.attrList.begin(); it != container.attrList.end(); it++) {
		BioAttributeInfo info = *it;
		values.push_back(info.name);
	}

	fd.image = face;
	for(int i = 0; i < values.size(); i++) {
		std::string name = values.at(i);
		cv::Ptr<cv::FaceRecognizer> rec = database.trainedRecognizers.find(name)->second;
		cv::Ptr<cv::KNearest> featureRec = database.trainedFeatureRecognizers.find(name)->second;

		if(featureRec != NULL) {
			fd.features = featureBuilder.detectFeatures(face);
			cv::Mat fv(1, featureVecSize, CV_32FC1);

			for(int j = 0; j < fd.features.size(); j++) {
				fv.at<float>(0, j) = fd.features.at(j);
			}

			float predict = featureRec->find_nearest(fv, featureRec->get_max_k());
			fd.featureCharacteristic.insert(std::pair<std::string, float>(name, predict));
		}

		if(rec != NULL) {
			cv::Mat cpy;
			face.copyTo(cpy);
			cv::cvtColor(cpy, cpy, CV_BGR2GRAY);
			int predict = rec->predict(cpy);
			log->Printf(INFO, "%s -> %i", name.c_str(), predict);
			fd.charactreistic.insert(std::pair<std::string, int>(name, predict));
		}
	}

	log->Write(INFO, "FaceClassifier: Classifying end");

	return fd;
}