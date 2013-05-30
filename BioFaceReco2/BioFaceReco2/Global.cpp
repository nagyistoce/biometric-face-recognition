#include "Global.h"


Global::Global(void) {
	properties = new Properties("bio.ini");
	log = new Log("app.log");
}


Global::~Global(void) {
	if(log != NULL) {
		delete log;
	}
}

std::string Global::getProperty(std::string key) {
	return properties->getProperty(key);
}