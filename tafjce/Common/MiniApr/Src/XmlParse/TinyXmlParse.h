#ifndef __TINY_XML_PARSE_H__
#define __TINY_XML_PARSE_H__

#include "XmlParse/XmlParse.h"

BEGIN_MINIAPR_NAMESPACE
class TinyXmlParse:public XmlParse
{
public:
	
	TinyXmlParse();
	~TinyXmlParse();

	void parseXMLFile(XmlHandle& handler, const std::string& filename);
};
END_MINIAPR_NAMESPACE

#endif
