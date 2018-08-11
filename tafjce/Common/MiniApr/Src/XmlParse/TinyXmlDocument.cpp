#include "MiniAprPch.h"
#include "TinyXmlDocument.h"

using namespace MINIAPR;
using namespace std;

TinyXmlDocument::TinyXmlDocument(XmlHandle& handler, const std::string& filename)
{
	m_handler = &handler;

	std::ifstream strm(filename.c_str() );
	if(!strm.good() )
	{
		return;
	}

	if(!strm.rdbuf() )
		return;
	
	ifstream::off_type pos = strm.tellg();
	strm.seekg(0, ios::end);
	ifstream::off_type size = strm.tellg();
	strm.seekg(pos, ios::beg);
	char* buf = new char[size + 1];
	strm.read(buf, size);
	buf[size] = 0;

	// Parse the document
	TiXmlDocument doc;
	doc.Parse((const char*)buf);
	const TiXmlElement* currElement = doc.RootElement();
	if (currElement)
	{
		// function called recursively to parse xml data
		processElement(currElement);
	} // if (currElement)
	
	// Free memory
	delete [] buf;
}


void TinyXmlDocument::processElement(const TiXmlElement* element)
{
	// build attributes block for the element
	XmlAttributes attrs;

	const TiXmlAttribute *currAttr = element->FirstAttribute();
	while (currAttr)
	{
		attrs.add(currAttr->Name(), currAttr->Value());
		currAttr = currAttr->Next();
	}

	// start element
	m_handler->elementStart(element->Value(), attrs);

	// do children
	const TiXmlNode* childNode = element->FirstChild();
	while (childNode)
	{
		switch(childNode->Type())
		{
		case TiXmlNode::ELEMENT:
			processElement(childNode->ToElement());
			break;
		case TiXmlNode::TEXT:
			//if (childNode->ToText()->Value() != '\0')
			//	m_handler->text(childNode->ToText()->Value());
			break;

			// Silently ignore unhandled node type
		};
		childNode = childNode->NextSibling();
	}

	// end element
	m_handler->elementEnd(element->Value());
}

