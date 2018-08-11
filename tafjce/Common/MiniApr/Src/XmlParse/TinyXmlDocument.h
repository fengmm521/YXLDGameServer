#ifndef __TITY_XML_DOCUMENT_H__
#define __TITY_XML_DOCUMENT_H__

#include "XmlParse/XmlParse.h"
#include "TinyXml/tinyxml.h"

BEGIN_MINIAPR_NAMESPACE
class TinyXmlDocument:public TiXmlDocument
{
public:
	TinyXmlDocument(XmlHandle& handler, const std::string& filename);
	~TinyXmlDocument(){};
	
protected:
	void processElement(const TiXmlElement* element);

private:
	XmlHandle* m_handler;
};
END_MINIAPR_NAMESPACE

#endif
