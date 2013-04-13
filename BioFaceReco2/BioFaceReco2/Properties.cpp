#include "Properties.h"


Properties::Properties(std::string filename)
{
	settings = new QSettings(QString(filename.c_str()), QSettings::IniFormat);
}


Properties::~Properties(void)
{
	delete settings;
}

std::string Properties::getProperty(std::string key) {

	return settings->value(QString(key.c_str())).toString().toStdString();
}
