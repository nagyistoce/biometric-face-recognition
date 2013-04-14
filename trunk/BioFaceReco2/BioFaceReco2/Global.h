#pragma once

#include <string>

#include "TSingleton.h"
#include "Properties.h"
#include "Log.h"

class Global : public TSingleton<Global>
{
	friend TSingleton<Global>;

public:
	void init();
	void destroy();

	std::string getProperty(std::string key);
	Log * getLogger() const;

private:
	Global(void);
	~Global(void);
	bool destroyed;

	Properties* properties;
	Log * log;

};

inline Log * Global::getLogger() const {
	return this->log;
}

