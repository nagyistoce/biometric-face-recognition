#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>
#include "BioAttributeInfo.h"
#include "Global.h"
#include "Log.h"


using namespace tinyxml2;

class BioAttributesContainer
{
public:
	BioAttributesContainer(void);
	~BioAttributesContainer(void);

	void load();

	void add(BioAttributeInfo val);
	void remove(int index);
	void clear();
	BioAttributeInfo * find(std::string name) const;

	std::vector<BioAttributeInfo> attrList;

private:
	BioAttributeInfo parse(XMLElement * elem);
	Log * log;
};

