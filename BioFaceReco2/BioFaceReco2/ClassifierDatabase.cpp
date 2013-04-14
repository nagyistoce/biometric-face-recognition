#include "ClassifierDatabase.h"


ClassifierDatabase::ClassifierDatabase(BioAttributesContainer container) {
	this->container = container;
}


ClassifierDatabase::~ClassifierDatabase(void)
{
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
	XMLDocument doc;
	XMLError er = doc.LoadFile(dbFile.c_str());

	if(er == XML_SUCCESS) {
		XMLElement * root = doc.RootElement();

		for(XMLElement * e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
			FaceData fd = parse(e, dbFolder, values);
			trainingFaces.push_back(fd);
		}
	}

	//std::vector<cv::Mat>::iterator it = faceImages.begin();
	for(int i = 0; i < trainingFaces.size(); i++) {
		cv::Mat m = trainingFaces.at(i).image;
		cv::resize(m, m, dstSize);
		faceImages.push_back(m);
	}

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

			/*
			// Here is how to get the eigenvalues of this Eigenfaces model:
			cv::Mat eigenvalues = model->getMat("eigenvalues");
			// And we can do the same to display the Eigenvectors (read Eigenfaces):
			cv::Mat W = model->getMat("eigenvectors");
			// Get the sample mean from the training data
			cv::Mat mean = model->getMat("mean");

			int height = faceImages.at(0).rows;
			imshow(name+"_mean", norm_0_255(mean.reshape(1, height)));

			for (int j = 0; j < std::min(16, W.cols); j++) {
				// get eigenvector #i
				cv::Mat ev = W.col(j).clone();
				// Reshape to original size & normalize to [0...255] for imshow.
				cv::Mat grayscale = norm_0_255(ev.reshape(1, height));
				// Show the image & apply a Bone colormap for better sensing.
				cv::Mat cgrayscale;
				cv::applyColorMap(grayscale, cgrayscale, cv::COLORMAP_BONE);
				std::stringstream ss;
				ss << name << "fisherfaces" << j;
				cv::imshow(ss.str(), cgrayscale);
			}

			for(int num_component = 0; num_component < std::min(16, W.cols); num_component++) {
				// Slice the Fisherface from the model:
				cv::Mat ev = W.col(num_component);
				cv::Mat projection = subspaceProject(ev, mean, faceImages[0].reshape(1,1));
				cv::Mat reconstruction = subspaceReconstruct(ev, mean, projection);
				// Normalize the result:
				reconstruction = norm_0_255(reconstruction.reshape(1, faceImages[0].rows));
				// Display or save:
				std::stringstream ss;
				ss << name << "fisherface_reconstruction" << num_component;
				imshow(ss.str(), reconstruction);
				
			}

			//cv::Mat ev = W.col(i).clone();
			//cv::Mat grayscale = norm_0_255(ev.reshape(1, trainingFaces.at(0).image.rows));

			//cv::imshow(name+"_eigenvalues", eigenvalues);
			//cv::imshow(name+"_eigenvectors", W);
			//cv::imshow(name+"_mean", mean);
			*/
			//save
			trainedRecognizers.insert(std::pair<std::string, cv::Ptr<cv::FaceRecognizer>>(name, model));
		}
		
	}
}

FaceData ClassifierDatabase::parse(XMLElement * e, std::string folder, std::vector<std::string>& values) {

	FaceData data;
	std::stringstream ss;
	//read image
	const char * filename = e->Attribute("file");
	ss << folder << "//" << filename;
	cv::Mat image = cv::imread(ss.str());
	cv::cvtColor(image, image, CV_BGR2GRAY);
	data.image = image;

	for(int i = 0; i < values.size(); i++) {
		std::string attr =  values.at(i);
		int index = atoi(e->Attribute(attr.c_str()));

		data.charactreistic.insert(std::pair<std::string, int>(attr, index));
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
