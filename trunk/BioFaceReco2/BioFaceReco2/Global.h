#pragma once

#include <string>

#include "TSingleton.h"
#include "Properties.h"
#include "Log.h"

enum LogLevel {
	DBG = 0,
	INFO = 1,
	WARN = 2,
	ERROR = 3
};

class Global : public TSingleton<Global> {
	friend TSingleton<Global>;

public:
	Global(void);
	~Global(void);

	std::string getProperty(std::string key);
	Log * getLogger() const;

private:
	Properties* properties;
	Log * log;

};

inline Log * Global::getLogger() const {
	return this->log;
}

