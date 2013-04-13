#include "Global.h"


Global::Global(void)
{
}


Global::~Global(void)
{
	if(!destroyed) {
		destroy();
	}
}

void Global::init() {

	properties = new Properties("bio.ini");

	destroyed = false;
}

void Global::destroy() {

	destroyed = true;
}

std::string Global::getProperty(std::string key) {
	
	return properties->getProperty(key);
}