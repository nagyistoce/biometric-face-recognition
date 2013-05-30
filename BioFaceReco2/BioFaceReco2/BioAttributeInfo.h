#pragma once

#include <map>
#include <string>

class BioAttributeInfo
{
public:
	BioAttributeInfo(void);
	~BioAttributeInfo(void);

	std::string name;
	std::map<int, std::string> attribMap;

};

