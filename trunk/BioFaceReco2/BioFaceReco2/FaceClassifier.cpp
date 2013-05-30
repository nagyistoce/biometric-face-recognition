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

		if(rec != NULL) {
			int predict = rec->predict(face);
			log->Printf(INFO, "%s -> %i", name.c_str(), predict);
			fd.charactreistic.insert(std::pair<std::string, int>(name, predict));
		}
	}

	log->Write(INFO, "FaceClassifier: Classifying end");

	return fd;
}