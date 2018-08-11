/*********************************************************
*
*	名称: ComponentManager_XmlHander.h
*	作者: wuxf
*	时间: 2011-04-17
*	描述: 组件管理Xml解析类
*********************************************************/

#ifndef __COMPONENT_MANAGER_XML_HANDLER_H
#define __COMPONENT_MANAGER_XML_HANDLER_H

#include "XmlParse/XmlParse.h"

BEGIN_MINIAPR_NAMESPACE
class ComponentManager_XmlHandler:public XmlHandle
{
public:
	
	virtual void elementStart(const std::string& element, const XmlAttributes& attributes);
	
	virtual void elementEnd(const std::string& element);

private:

	void elementStartScheme(const XmlAttributes& attribute);
	void elementStartComponent(const XmlAttributes& attribute);
	void elementStartDependency(const XmlAttributes& attribute);
	void elementStartProperty(const XmlAttributes& attribute);
	void elementStartObject(const XmlAttributes& attribute);
	void elementStartInject(const XmlAttributes& attribute);
	void elementStartTable(const XmlAttributes& attribute);
	void elemenrStartPropDef(const XmlAttributes& attribute);

	void elementEndScheme();
	void elementEndComponent();
	void elementEndComponents();
	void elementEndDependency();
	void elementEndProperty();
	void elementEndObject();
	void elementEndInject();
	void elementEndObjects();

	static const std::string schemeElement;
	static const std::string fileNameAttribute;
	
	static const std::string componentElement;
	static const std::string componentNameAttribute;
	static const std::string dllAttribute;
	static const std::string creatFunAttribute;
	static const std::string versionAttribute;

	static const std::string componentsElement;

	static const std::string tableElement;
	static const std::string tablename;
	static const std::string tableFileType;
	static const std::string tableFile;
	static const std::string tableIndex;


	static const std::string objectElement;
	static const std::string objectNameAttribute;
	static const std::string objectdllAttribute;
	static const std::string objectCreatFunAttribute;

	static const std::string objectsElement;

	static const std::string objectInjectElement;
	static const std::string objectInjectNameAttribute;


	static const std::string dependencyElement;
	static const std::string dependencyComponentName;
	
	static const std::string propertyNameElement;
	static const std::string propertyTypeAttribute;
	static const std::string propertyNameAttribute;
	static const std::string propertyValueAttribute;
	static const std::string propertyDefAttribute;
	
	static const std::string propDefine;
	static const std::string propDefineFile;
	static const std::string propDefineName;
};

END_MINIAPR_NAMESPACE

#endif
