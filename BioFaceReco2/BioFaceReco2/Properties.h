#pragma once

#include <string>
#include <QSettings>

class Properties
{
public:
	Properties(std::string filename);
	~Properties(void);

	std::string getProperty(std::string key);
private:
	QSettings* settings;
};

