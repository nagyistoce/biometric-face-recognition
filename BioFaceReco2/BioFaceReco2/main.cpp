#include "biofacereco2.h"
#include <QtWidgets/QApplication>

#include <iostream>
#include "Global.h"
#include "BioAttributesContainer.h"
#include "ClassifierDatabase.h"
#include "FaceClassifier.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BioFaceReco2 w;
	w.show();

	return a.exec();
}
