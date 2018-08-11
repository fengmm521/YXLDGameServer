#include "MiniAprPch.h"
#include "XmlParse/XmlParse.h"

using namespace MINIAPR;

void XmlAttributes::add(const std::string& attrName, const std::string& attrValue)
{
	m_Attrs[attrName] = attrValue;
}

void XmlAttributes::remove(const std::string& attrName)
{
	AttributeMap::iterator it = m_Attrs.find(attrName);
	
	if(it != m_Attrs.end() )
		m_Attrs.erase(it);
}

bool XmlAttributes::exists(const std::string& attrName) const
{
	return m_Attrs.find(attrName) != m_Attrs.end();
}

size_t XmlAttributes::getCount(void) const
{
	return m_Attrs.size();
}

const std::string& XmlAttributes:: getName(size_t index) const
{
	static std::string strDefaultRet = "";
	if(index >= m_Attrs.size() )
		return strDefaultRet;
	
	AttributeMap::const_iterator it = m_Attrs.begin();
	std::advance(it, index);

	return it->first;
}

const std::string& XmlAttributes::getValue(size_t index) const
{
	static std::string strDefaultRet = "";
	if(index >= m_Attrs.size() )
		return strDefaultRet;

	AttributeMap::const_iterator it = m_Attrs.begin();
	std::advance(it, index);

	return it->second;
}

const std::string& XmlAttributes::getValue(const std::string& attrName) const
{
	static std::string strDefaultRet = "";
	AttributeMap::const_iterator pos = m_Attrs.find(attrName);

	if (pos != m_Attrs.end())
	{
		return (*pos).second;
	}
	
	return strDefaultRet;
}

const std::string& XmlAttributes::getValueAsString(const std::string& attrName, const std::string& def) const
{
	if(!exists(attrName) )
		return def;

	return getValue(attrName);
}

bool XmlAttributes::getValueAsBool(const std::string& attrName, bool def) const
{
	if(!exists(attrName) )
		return def;

	std::string strValue = getValue(attrName);

	if( (strValue == "false") || (strValue == "0") )
		return false;
	else if( (strValue == "true") || (strValue == "1") )
		return true;

	return def;
}

int XmlAttributes::getValueAsInteger(const std::string& attrName, int def) const
{
	if(!exists(attrName) )
		return def;
	
	int nValue = 0;
	std::istringstream strm(getValue(attrName).c_str());
	strm>>nValue;

	return nValue;
}

float XmlAttributes::getValueAsFloat(const std::string& attrName, float def) const
{
	if(!exists(attrName) )
		return def;

	float val = 0.0f;
	std::istringstream strm(getValue(attrName).c_str());

	strm >> val;
	
	return val;
}
