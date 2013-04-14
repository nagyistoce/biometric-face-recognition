#ifndef BIOFACERECO2_H
#define BIOFACERECO2_H

#include <QtWidgets/QMainWindow>
#include <QImage>
#include <qfiledialog.h>

#include <vector>

#include <opencv2\opencv.hpp>
#include "ui_biofacereco2.h"

#include "BioAttributesContainer.h"
#include "ClassifierDatabase.h"
#include "FaceClassifier.h"
#include "FaceFinder.h"
#include "FaceData.h"
#include "EyeFinder.h"
#include "Utilities.h"

class BioFaceReco2 : public QMainWindow
{
	Q_OBJECT

public:
	BioFaceReco2(QWidget *parent = 0);
	~BioFaceReco2();

private:
	Ui::BioFaceReco2Class ui;
	cv::Mat img;
	
	BioAttributesContainer * bac;
	ClassifierDatabase * classDb;
	FaceClassifier * classifier;

	std::vector<FaceData> data;
	int currentImage;

	QImage convert(cv::Mat img);
	void processImage();
	void showData(int index);

private slots:
	void openImage();
	void nextImage();
	void prevImage();

};

#endif // BIOFACERECO2_H
