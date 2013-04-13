#include "biofacereco2.h"

BioFaceReco2::BioFaceReco2(QWidget *parent)
	: QMainWindow(parent)
{
	Global::Instance().init();

	ui.setupUi(this);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openImage()));
	connect(ui.nextImageBtn, SIGNAL(clicked()), this, SLOT(nextImage()));
	connect(ui.prevImageBtn, SIGNAL(clicked()), this, SLOT(prevImage()));

	currentImage = -1;
	data.clear();

	bac = new BioAttributesContainer();
	bac->load();

	classDb = new ClassifierDatabase(*bac);
	classDb->train();

	classifier = new FaceClassifier(*bac, *classDb);

}

BioFaceReco2::~BioFaceReco2()
{
	delete classifier;
}

void BioFaceReco2::openImage() {
	QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp *.pgm)"));
	this->img = cv::imread(filename.toStdString());

	processImage();	
}

QImage BioFaceReco2::convert(cv::Mat img) {
	
	cv::Mat convertedImage;
	img.copyTo(convertedImage);
	cv::cvtColor(convertedImage, convertedImage, CV_BGR2RGB);
	QImage qimg((const unsigned char*)(convertedImage.data), img.rows, img.cols, QImage::Format_RGB888);

	return qimg;
}

void BioFaceReco2::processImage() {
	
	int dstWidth = atoi(Global::Instance().getProperty("img_width").c_str());
	int dstHeight = atoi(Global::Instance().getProperty("img_height").c_str());

	//find faces on picture
	FaceFinder ff;
	std::vector<cv::Mat> faces = ff.findInFile(img);

	//convert to gray, align and resize
	for(int i = 0; i < faces.size(); i++) {
		cv::Mat m = faces.at(i);
		cv::cvtColor(m, m, CV_BGR2GRAY);

		cv::resize(m, m, cv::Size(dstWidth, dstHeight), cv::INTER_LINEAR);

		FaceData fd = classifier->classifyFace(m);

		data.push_back(fd);
	}

	showData(0);
}

void BioFaceReco2::showData(int index) {

	if(data.size() > 0 && index < data.size()) {
		FaceData fd = data.at(index);

		cv::Mat convertedImage;
		fd.image.copyTo(convertedImage);
		cv::cvtColor(convertedImage, convertedImage, CV_GRAY2RGB);
		QImage qimg((const unsigned char*)(convertedImage.data), convertedImage.cols, convertedImage.rows, QImage::Format_RGB888);

		QPixmap map = QPixmap::fromImage(qimg);
		ui.label->setPixmap(map);
		QSize qsize = ui.label->pixmap()->size();
		ui.label->resize(qsize);

		std::map<std::string,int>::iterator it;
		std::stringstream ss;
		for ( it = fd.charactreistic.begin(); it != fd.charactreistic.end(); it++) {
			ss << it->first << " => " << it->second << '\n';
		}

		ui.text->clear();
		ui.text->append(QString(ss.str().c_str()));

		this->currentImage = index;

		std::stringstream ss2;
		ss2 << currentImage << '/' << data.size() - 1;
		ui.imageLabel->setText(QString(ss2.str().c_str()));
	}
}

void BioFaceReco2::nextImage() {
	if(currentImage >= 0 && currentImage < data.size()) {
		showData(++currentImage);
	}
}

void BioFaceReco2::prevImage() {
	if(currentImage > 0 && currentImage <= data.size()) {
		showData(--currentImage);
	}
}