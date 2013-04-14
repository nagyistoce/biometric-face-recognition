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
	log = new Log("BFR.log");
	destroyed = false;
}

void Global::destroy() {

	if(log != NULL) {
		delete log;
	}
	destroyed = true;
}

std::string Global::getProperty(std::string key) {
	
	return properties->getProperty(key);
}