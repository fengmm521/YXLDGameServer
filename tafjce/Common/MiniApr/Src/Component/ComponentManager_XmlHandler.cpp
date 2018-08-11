#include "MiniAprPch.h"
#include "../XmlParse/TinyXmlParse.h"
#include "ComponentManager_XmlHandler.h"
#include "ComponentManagerImp.h"
#include "TableImp.h"

using namespace MINIAPR;

//#define DEBUG_PREFIX	"_d"
#define DEBUG_PREFIX	""

extern ComponentManagerImp	g_ComponentManagerImp;


const std::string ComponentManager_XmlHandler::schemeElement("scheme");
const std::string ComponentManager_XmlHandler::fileNameAttribute("file");

const std::string ComponentManager_XmlHandler::componentElement("component");
const std::string ComponentManager_XmlHandler::componentNameAttribute("name");
const std::string ComponentManager_XmlHandler::dllAttribute("dll");
const std::string ComponentManager_XmlHandler::creatFunAttribute("createFun");
const std::string ComponentManager_XmlHandler::versionAttribute("version");

const std::string ComponentManager_XmlHandler::componentsElement("components");

const std::string ComponentManager_XmlHandler::tableElement("table");
const std::string ComponentManager_XmlHandler::tablename("name");
const std::string ComponentManager_XmlHandler::tableFileType("filetype");
const std::string ComponentManager_XmlHandler::tableFile("file");
const std::string ComponentManager_XmlHandler::tableIndex("indexfield");


const std::string ComponentManager_XmlHandler::objectElement("object");
const std::string ComponentManager_XmlHandler::objectNameAttribute("name");;
const std::string ComponentManager_XmlHandler::objectdllAttribute("dll");
const std::string ComponentManager_XmlHandler::objectCreatFunAttribute("createFun");

const std::string ComponentManager_XmlHandler::objectsElement("objects");

const std::string ComponentManager_XmlHandler::objectInjectElement("inject");
const std::string ComponentManager_XmlHandler::objectInjectNameAttribute("name");

const std::string ComponentManager_XmlHandler::dependencyElement("dependency");
const std::string ComponentManager_XmlHandler::dependencyComponentName("name");

const std::string ComponentManager_XmlHandler::propertyNameElement("property");
const std::string ComponentManager_XmlHandler::propertyTypeAttribute("type");
const std::string ComponentManager_XmlHandler::propertyNameAttribute("name");
const std::string ComponentManager_XmlHandler::propertyValueAttribute("value");
const std::string ComponentManager_XmlHandler::propertyDefAttribute("defname");


const std::string ComponentManager_XmlHandler::propDefine("propdef");
const std::string ComponentManager_XmlHandler::propDefineFile("file");
const std::string ComponentManager_XmlHandler::propDefineName("name");


void ComponentManager_XmlHandler::elementStart(const std::string& element, const XmlAttributes& attributes)
{
	if(element == schemeElement)
	{
		elementStartScheme(attributes);
	}
	else if(element == componentElement)
	{
		elementStartComponent(attributes);
	}
	else if(element == objectElement)
	{
		elementStartObject(attributes);
	}
	else if(element == dependencyElement)
	{
		elementStartDependency(attributes);
	}
	else if(element == propertyNameElement)
	{
		elementStartProperty(attributes);
	}
	else if(element == objectInjectElement)
	{
		elementStartInject(attributes);
	}
	else if(element == tableElement)
	{
		elementStartTable(attributes);
	}
	else if(element == componentsElement)
	{
		// noting
	}
	else if(element == propDefine)
	{
		elemenrStartPropDef(attributes);
	}
	else
	{
		MINIAPR_ERROR("Unknow ElementStart:[%s]\n", element.c_str() );
	}
}

void ComponentManager_XmlHandler::elementStartTable(const XmlAttributes& attribute)
{
	std::string strTableName = attribute.getValueAsString(tablename, "");
	std::string fileType = attribute.getValueAsString(tableFileType, "");
	std::string filePath = attribute.getValueAsString(tableFile, "");
	std::string tableIndexName = attribute.getValueAsString(tableIndex, "");
	if(fileType == "csv")
	{
		CsvFile* pCsvFile = new CsvFile;
		string strFullPath = ServerConfig::DataPath + filePath;
		pCsvFile->open(strFullPath.c_str() );
		if(tableIndexName.size() > 0)
		{
			pCsvFile->setIndexField(tableIndexName);
		}
		ITable* pTable = new TableImp_Csv(pCsvFile, strTableName);
		g_ComponentManagerImp.m_tableMap[strTableName] = pTable;
	}
	else if(fileType == "dbc")
	{
		DbcFile* pDbcFile = new DbcFile;
		pDbcFile->open(filePath.c_str(), true);
		ITable* pTable = new TableImp_Dbc(pDbcFile, strTableName);
		g_ComponentManagerImp.m_tableMap[strTableName] = pTable;
	}
}

void ComponentManager_XmlHandler::elementStartScheme(const XmlAttributes& attribute)
{
	TinyXmlParse xmlParse;
	std::string strSchemeFile = attribute.getValue(fileNameAttribute);
	xmlParse.parseXMLFile(*this, strSchemeFile);
}

void ComponentManager_XmlHandler::elementStartComponent(const XmlAttributes& attribute)
{
	g_ComponentManagerImp.m_currentScheme = ComponentManagerImp::schemeInfo();
	g_ComponentManagerImp.m_currentScheme.strName = attribute.getValue(componentNameAttribute);
	g_ComponentManagerImp.m_currentScheme.strDllName = attribute.getValue(dllAttribute);
	g_ComponentManagerImp.m_currentScheme.strCreateFun = attribute.getValue(creatFunAttribute);

#ifdef _DEBUG
	g_ComponentManagerImp.m_currentScheme.strDllName += DEBUG_PREFIX;
#endif

	std::istringstream stm(attribute.getValue(versionAttribute) );
	stm>>g_ComponentManagerImp.m_currentScheme.nVersion;
}

void ComponentManager_XmlHandler::elementStartDependency(const XmlAttributes& attribute)
{
	g_ComponentManagerImp.m_currentScheme.dependencyList.push_back(attribute.getValue(dependencyComponentName) );
}

void ComponentManager_XmlHandler::elementStartProperty(const XmlAttributes& attribute)
{
	//PropertySet::PropertyKey key;
	//std::istringstream strm(attribute.getValue(propertyNameAttribute) );
	//strm>>key;

	string propDefName = attribute.getValue(propertyDefAttribute);
	PropertySet::PropertyKey key = 0;
	//IPropDefine* pPropDef = g_ComponentManagerImp.m_currentScheme.pPropDef;
	std::map<std::string, IPropDefine*>& mapPropDef = g_ComponentManagerImp.m_currentScheme.mapPropDef;
	if(mapPropDef.find(propDefName) != mapPropDef.end() )
	{
		IPropDefine* pPropDef = mapPropDef[propDefName];
		std::string strKeyName = attribute.getValue(propertyNameAttribute);
		key = pPropDef->getPropKey(strKeyName);
	}
	else
	{
		std::istringstream strm(attribute.getValue(propertyNameAttribute) );
		strm>>key;
	}
	std::string strType = attribute.getValue(propertyTypeAttribute);
	std::istringstream valueStrm(attribute.getValue(propertyValueAttribute) );
	if(strType == "int")
	{
		Int32 nValue = 0;
		valueStrm>>nValue;
		g_ComponentManagerImp.m_currentScheme.propertySet.setInt(key, nValue);
	}
	else if(strType == "string")
	{
		std::string strValue = attribute.getValue(propertyValueAttribute);
		g_ComponentManagerImp.m_currentScheme.propertySet.setString(key, strValue.c_str() );
	}
	else if(strType == "float")
	{
		float fValue = 0;
		valueStrm>>fValue;
		g_ComponentManagerImp.m_currentScheme.propertySet.setFloat(key, fValue);
	}
}

void ComponentManager_XmlHandler::elemenrStartPropDef(const XmlAttributes& attribute)
{
	std::string strFile = attribute.getValue(propDefineFile);
	std::string strName = attribute.getValue(propDefineName);
	strFile = ServerConfig::DataPath + strFile;
	g_ComponentManagerImp.m_currentScheme.mapPropDef[strName] = new PropDefineImp(strFile);
}

void ComponentManager_XmlHandler::elementStartObject(const XmlAttributes& attribute)
{
	g_ComponentManagerImp.m_currentScheme = ComponentManagerImp::schemeInfo();
	g_ComponentManagerImp.m_currentScheme.strName = attribute.getValue(objectNameAttribute);
	g_ComponentManagerImp.m_currentScheme.strDllName = attribute.getValue(objectdllAttribute);
	g_ComponentManagerImp.m_currentScheme.strCreateFun = attribute.getValue(objectCreatFunAttribute);

#ifdef _DEBUG
	g_ComponentManagerImp.m_currentScheme.strDllName += DEBUG_PREFIX;
#endif
}

void ComponentManager_XmlHandler::elementStartInject(const XmlAttributes& attribute)
{
	g_ComponentManagerImp.m_currentScheme.dependencyList.push_back(attribute.getValue(objectInjectNameAttribute) );
}

void ComponentManager_XmlHandler::elementEndComponents()
{
	g_ComponentManagerImp.initAllComponent();
}

void ComponentManager_XmlHandler::elementEndScheme()
{
}

void ComponentManager_XmlHandler::elementEndComponent()
{
	g_ComponentManagerImp.m_componentSchemeMap[g_ComponentManagerImp.m_currentScheme.strName] = g_ComponentManagerImp.m_currentScheme;
}

void ComponentManager_XmlHandler::elementEndDependency()
{
}

void ComponentManager_XmlHandler::elementEndProperty()
{
}

void ComponentManager_XmlHandler::elementEndObject()
{
	
	for(std::map<string, IPropDefine*>::iterator it = g_ComponentManagerImp.m_currentScheme.mapPropDef.begin(); 
		it != g_ComponentManagerImp.m_currentScheme.mapPropDef.end(); it++)
	{
		delete it->second;
	}
	g_ComponentManagerImp.m_currentScheme.mapPropDef.clear();
	g_ComponentManagerImp.m_objectSchemeMap[g_ComponentManagerImp.m_currentScheme.strName] = g_ComponentManagerImp.m_currentScheme;
}

void ComponentManager_XmlHandler::elementEndInject()
{
}

void ComponentManager_XmlHandler::elementEndObjects()
{

}

void ComponentManager_XmlHandler::elementEnd(const std::string& element)
{
	if(element == schemeElement)
	{
		elementEndScheme();
	}
	else if(element == componentElement)
	{
		elementEndComponent();
	}
	else if(element == objectElement)
	{
		elementEndObject();
	}
	else if(element == dependencyElement)
	{
		elementEndDependency();
	}
	else if(element == propertyNameElement)
	{
		elementEndProperty();
	}
	else if(element == objectInjectElement)
	{
		elementEndInject();
	}
	else if(element == componentsElement)
	{
		elementEndComponents();
	}
	else if(element == objectsElement)
	{
		elementEndObjects();
	}
	else if(element == propDefine)
	{
		// nothing
	}
	else if(element == tableElement)
	{
		// do nothing!
	}
	else
	{
		MINIAPR_ERROR("Unknow Element End:[%s]\n", element.c_str() );
	}
}
