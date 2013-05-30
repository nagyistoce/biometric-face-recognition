#include "BioAttributesContainer.h"


BioAttributesContainer::BioAttributesContainer(void) {
	log = Global::Instance().getLogger();
}


BioAttributesContainer::~BioAttributesContainer(void) {
}


void BioAttributesContainer::load() {
	log->Write(INFO, "BioAttributesContainer:Loading attributes");

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
	log->Write(INFO, "BioAttributesContainer:Loading end");
}

void BioAttributesContainer::add(BioAttributeInfo val) {
	attrList.push_back(val);
}

void BioAttributesContainer::remove(int index) {
}

void BioAttributesContainer::clear() {
	attrList.clear();
}

BioAttributeInfo * BioAttributesContainer::find(std::string name) const {
	std::vector<BioAttributeInfo>::const_iterator it;
	BioAttributeInfo bai;
	int index = 0;

	for(it = attrList.begin(); it != attrList.end(); it++, index++) {
		bai = *it;

		if(bai.name.compare(name) == 0) {
			return it._Ptr;
		}
	}

	log->Printf(WARN, "Attribute %s not founded", name.c_str());
	return NULL;

}

BioAttributeInfo BioAttributesContainer::parse(XMLElement * elem) {
	BioAttributeInfo info;

	const char * value = elem->Attribute("value");
	info.name = std::string(value);
	log->Printf(INFO, "Parsing attribute %s start", value);

	for(XMLElement * v = elem->FirstChildElement(); v != NULL; v = v->NextSiblingElement()) {
		std::string attVal(v->GetText());
		int index = atoi(v->Attribute("index"));
		info.attribMap.insert(std::pair<int, std::string>(index, attVal));

		log->Printf(INFO, "%s -> %i", attVal.c_str(), index);
	}
	log->Printf(INFO, "Parsing attribute %s end", value);
	return info;
}