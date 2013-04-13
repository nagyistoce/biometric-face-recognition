#pragma once

#include <string>

#include "TSingleton.h"
#include "Properties.h"

class Global : public TSingleton<Global>
{
	friend TSingleton<Global>;

public:
	void init();
	void destroy();

	std::string getProperty(std::string key);

private:
	Global(void);
	~Global(void);
	bool destroyed;

	Properties* properties;

};

