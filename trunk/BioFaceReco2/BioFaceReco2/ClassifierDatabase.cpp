#include "ClassifierDatabase.h"

ClassifierDatabase::ClassifierDatabase(BioAttributesContainer container) {
	this->container = container;
	log = Global::Instance().getLogger();
}


ClassifierDatabase::~ClassifierDatabase(void) {
}

void ClassifierDatabase::train() {
	std::string dbFile = Global::Instance().getProperty("database_xml");
	std::string dbFolder = Global::Instance().getProperty("database_folder");
	int dstWidth = atoi(Global::Instance().getProperty("img_width").c_str());
	int dstHeight = atoi(Global::Instance().getProperty("img_height").c_str());

	std::vector<FaceData> trainingFaces;
	std::vector<std::string> values;
	cv::Size dstSize(dstWidth, dstHeight);

	//images for training
	std::vector<cv::Mat> faceImages;

	//make list of values to read
	std::vector<BioAttributeInfo>::iterator it;
	for(it = container.attrList.begin(); it != container.attrList.end(); it++) {
		BioAttributeInfo info = *it;
		values.push_back(info.name);
	}

	//read database xml document
	log->Write(INFO, "ClassifierDatabase:Parsing database images");
	XMLDocument doc;
	XMLError er = doc.LoadFile(dbFile.c_str());

	if(er == XML_SUCCESS) {
		XMLElement * root = doc.RootElement();

		for(XMLElement * e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
			FaceData fd = parse(e, dbFolder, values);
			if(fd.image.total() != 0) {
				trainingFaces.push_back(fd);
			}
		}
	}
	log->Write(INFO, "ClassifierDatabase:Parsing end");


	// Features vector
	log->Write(INFO, "Building features vector begin");
	for(int i = 0; i < trainingFaces.size(); i++) {
		cv::Mat m = trainingFaces.at(i).image;
		trainingFaces.at(i).features = featuresBuilder.detectFeatures(m);
	}

	// FisherFaces 
	log->Write(INFO, "ClassifierDatabase:Preparing images");
	for(int i = 0; i < trainingFaces.size(); i++) {
		cv::Mat m = trainingFaces.at(i).image;
		//cv::equalizeHist(m, m);
		//cv::normalize(m, m, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		cv::resize(m, m, dstSize);
		cv::cvtColor(m, m, CV_BGR2GRAY);
		//std::stringstream ss;
		//ss << "TrImg" << i;
		//cv::imshow(ss.str(), m);
		faceImages.push_back(m);
	}

	log->Write(INFO, "ClassifierDatabase:Trainig recognizers");
	//build facerecoginzers
	if(trainingFaces.size() > 0) {
		for(int i = 0; i < values.size(); i++) {
			std::string name = values.at(i);
			cv::Ptr<cv::FaceRecognizer> model = cv::createFisherFaceRecognizer();

			std::vector<int> labels;
			//create data
			for(int i = 0; i < trainingFaces.size(); i++) {
				int tmp = trainingFaces.at(i).charactreistic.find(name)->second;
				labels.push_back(tmp);
			}

			//train
			model->train(faceImages, labels);

			//save
			trainedRecognizers.insert(std::pair<std::string, cv::Ptr<cv::FaceRecognizer>>(name, model));
			log->Printf(INFO, "Trained FaceRecognizer: %s", name.c_str());
		}
	}
}

FaceData ClassifierDatabase::parse(XMLElement * e, std::string folder, std::vector<std::string>& values) {
	FaceData data;
	std::stringstream ss;

	//read image
	const char * filename = e->Attribute("file");
	log->Printf(INFO, "Parsing image: %s", filename);

	ss << folder << "//" << filename;
	cv::Mat image = cv::imread(ss.str());

	if(image.total() == 0) {
		log->Printf(ERROR, "Failed to load image %s", filename);
	} else {
		data.image = image;

		for(int i = 0; i < values.size(); i++) {
			std::string attr =  values.at(i);
			int index = atoi(e->Attribute(attr.c_str()));
			log->Printf(INFO, "%s -> %i", attr.c_str(), index);
			data.charactreistic.insert(std::pair<std::string, int>(attr, index));
		}
	}

	return data;
}

cv::Mat ClassifierDatabase::norm_0_255(cv::InputArray _src) {
	cv::Mat src = _src.getMat();
	// Create and return normalized image:
	cv::Mat dst;
	switch(src.channels()) {
	case 1:
		cv::normalize(_src, dst, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(_src, dst, 0, 255, cv::NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}
