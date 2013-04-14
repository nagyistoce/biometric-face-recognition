#include "BioAttributesContainer.h"


BioAttributesContainer::BioAttributesContainer(void)
{
}


BioAttributesContainer::~BioAttributesContainer(void)
{
}


void BioAttributesContainer::load() {
	std::string xmlFile = Global::Instance().getProperty("keymap_xml");

	XMLDocument doc;
	XMLError er = doc.LoadFile(xmlFile.c_str());

	if(er == XML_SUCCESS) {
		XMLElement * root = doc.RootElement();

		for(XMLElement * e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
			BioAttributeInfo info = parse(e);
			attrList.push_back(info);
		}
	}
}

void BioAttributesContainer::add(BioAttributeInfo val) {
	attrList.push_back(val);
}

void BioAttributesContainer::remove(int index) {

}

void BioAttributesContainer::clear() {
	attrList.clear();
}

BioAttributeInfo BioAttributesContainer::find(std::string name) {

	std::vector<BioAttributeInfo>::iterator it;

	for(it = attrList.begin(); it != attrList.end(); it++) {
		BioAttributeInfo bai = *it;

		if(bai.name.compare(name) == 0) {
			return bai;
		}
	}

	BioAttributeInfo bai;
	bai.name = "NULL";
	return bai;

}

BioAttributeInfo BioAttributesContainer::parse(XMLElement * elem) {
	BioAttributeInfo info;

	const char * value = elem->Attribute("value");
	info.name = std::string(value);

	for(XMLElement * v = elem->FirstChildElement(); v != NULL; v = v->NextSiblingElement()) {
		std::string attVal(v->GetText());
		int index = atoi(v->Attribute("index"));
		info.attribMap.insert(std::pair<int, std::string>(index, attVal));
	}

	return info;
}