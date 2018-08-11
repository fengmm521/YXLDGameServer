/*********************************************************
*
*	名称: ComponentBase.h
*	作者: wuxf
*	时间: 2011-04-17
*	描述: 组件管理类
*********************************************************/

#ifndef __COMPONENT_MANAGER_IMP_H__
#define __COMPONENT_MANAGER_IMP_H__

#include "Component/IComponent.h"

BEGIN_MINIAPR_NAMESPACE

class ComponentManagerImp:public IComponentObjectManager
{
public:

	struct schemeInfo
	{
		schemeInfo(){}
		schemeInfo(const schemeInfo& rhs)
		{
			*this = rhs;
		}

		void operator = (const schemeInfo& rhs)
		{
			for(std::map<string, IPropDefine*>::iterator it = mapPropDef.begin(); it != mapPropDef.end(); it++)
			{
				delete it->second;
			}
			mapPropDef.clear();
		
			strName = rhs.strName;
			strDllName = rhs.strDllName;
			strCreateFun = rhs.strCreateFun;
			nVersion = rhs.nVersion;
			dependencyList = rhs.dependencyList;
			propertySet = rhs.propertySet;
		
			for(std::map<string, IPropDefine*>::const_iterator it = rhs.mapPropDef.begin(); it != rhs.mapPropDef.end(); it++)
			{
				string strName = it->first;
				IPropDefine* pNewPropDef = it->second->clone();
				mapPropDef[strName] = pNewPropDef;
			}
		}
		
		~schemeInfo()
		{
			for(std::map<string, IPropDefine*>::iterator it = mapPropDef.begin(); it != mapPropDef.end(); it++)
			{
				delete it->second;
			}
			mapPropDef.clear();
		}
		std::string strName;
		std::string strDllName;
		std::string strCreateFun;
		Int32		nVersion;
		std::vector<std::string>	dependencyList;
		PropertySet propertySet;
		std::map<std::string, IPropDefine*> mapPropDef;
	};
	
	friend class ComponentManager_XmlHandler;

	ComponentManagerImp(){}

	// 暂时不处理Table的删除
	~ComponentManagerImp(){}
	
	virtual IComponent* find(const std::string& strComponentName);

	virtual IObject* createObject(const std::string& strClassName);

	virtual ITable* findTable(const std::string& strTableName);
	
	virtual bool loadScheme(const std::string& strSchemeFile);

	virtual void unLoadAllComponent();

	virtual void _doComponentUnload(IComponent* pComponent);

    //会重新加载组件管理器中相应的csv表内容
    //慎重使用,需要避免以下情况:
    //         比如代码里有同时直接使用表数据,另外也有使用根据表数据生成的组织过的数据
    //         此时如果只重新加载了表数据但未更新组织过的数据，有特殊数据可能导致冲突
    virtual bool reloadTable(const std::string& strTableName);
    //根据己有表的路径重新读取最新文件生成一份拷贝，不调整组件管理器里己有的表内容
    //返回的指针需要调用方自行处理内存释放
	virtual ITable* cloneTableFromStorage(const std::string& strTableName);

	virtual void disableComponent(const string& strComponent);

protected:

	bool initAllComponent();
	IComponent* createComponent(const schemeInfo& componentScheme);
	IComponent* getNextUnloadComponent();
	bool _getComponentName(IComponent* pComponent, std::string& strComponentName);
	bool _removeComponent(const std::string& strComponentName);

private:
	typedef std::map<std::string, IComponent*>	ComponentMap;
	typedef std::map<std::string, schemeInfo>	SchemeMap;
	typedef std::map<std::string, ITable*>	TableMap;
	ComponentMap	m_componentMap;
	SchemeMap		m_componentSchemeMap;
	SchemeMap		m_objectSchemeMap;
	schemeInfo		m_currentScheme;
	TableMap		m_tableMap;

	set<string>		m_disableComponent;
};

END_MINIAPR_NAMESPACE

#endif
