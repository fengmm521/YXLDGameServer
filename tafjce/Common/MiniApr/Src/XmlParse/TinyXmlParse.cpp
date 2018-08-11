#include "MiniAprPch.h"
#include "TinyXmlDocument.h"
#include "TinyXmlParse.h"

using namespace MINIAPR;

TinyXmlParse::TinyXmlParse()
{
}

TinyXmlParse::~TinyXmlParse()
{
}

void TinyXmlParse::parseXMLFile(MINIAPR::XmlHandle& handler, const std::string& filename)
{
	TinyXmlDocument xmlDocument(handler, filename);
}

BEGIN_MINIAPR_NAMESPACE
XmlParse* createXmlParse()
{
	return new TinyXmlParse;
}
END_MINIAPR_NAMESPACE
