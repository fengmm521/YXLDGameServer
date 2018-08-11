#include "MiniAprPch.h"
#include "ComponentManagerImp.h"
#include "ComponentManager_XmlHandler.h"
#include "../XmlParse/TinyXmlParse.h"
#include "errno.h"

#ifndef WIN32
	#include <dlfcn.h>
#endif

using namespace MINIAPR;
using namespace std;

#if defined(__WIN32__) || defined(_WIN32)
#	define DYNLIB_HANDLE hInstance
#	define DYNLIB_LOAD( a ) LoadLibrary( a )
#	define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#	define DYNLIB_UNLOAD( a ) !FreeLibrary( a )
struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif defined(__linux__)
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY )
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )
#    define DYNLIB_ERROR( ) dlerror( )

#elif defined(__APPLE_CC__)
#    define DYNLIB_HANDLE CFBundleRef
#    define DYNLIB_LOAD( a ) mac_loadExeBundle( a )
#    define DYNLIB_GETSYM( a, b ) mac_getBundleSym( a, b )
#    define DYNLIB_UNLOAD( a ) mac_unloadExeBundle( a )
#    define DYNLIB_ERROR( ) mac_errorBundle()
#endif


typedef IObject* (*createObjectFun)();
typedef IComponent* (*createComponentFun)(Int32 nVersion);

ComponentManagerImp	g_ComponentManagerImp;

IComponentObjectManager* getCompomentObjectManager()
{
	return &g_ComponentManagerImp;
}

#ifdef _STATIC_MINIAPR_
extern "C" IComponent* createComponentStatic(const std::string& strComponentName, Int32 nVersion);
extern "C" IObject* createObjectStatic(const std::string& strObjectName);

#endif

IComponent* ComponentManagerImp::find(const std::string& strComponentName)
{
	ComponentMap::iterator it = m_componentMap.find(strComponentName);
	if(it != m_componentMap.end() )
		return it->second;

	return NULL;
}

IObject* ComponentManagerImp::createObject(const std::string& strClassName)
{
	IObject* pObject = NULL;
	SchemeMap::iterator it = m_objectSchemeMap.find(strClassName);
	if(it == m_objectSchemeMap.end() ) return pObject;

	#ifdef _STATIC_MINIAPR_
		pObject = createObjectStatic(strClassName);
	#else	
		DYNLIB_HANDLE hHandle = DYNLIB_LOAD(it->second.strDllName.c_str() );
		if(!hHandle) return pObject;
		
		createObjectFun pFun = (createObjectFun)DYNLIB_GETSYM(hHandle, it->second.strCreateFun.c_str() );
		if(!pFun) return pObject;
		pObject = (*pFun)();
	#endif

	pObject->getPropertySet() += it->second.propertySet;
	
	std::vector<IObject*> objectList;
	for(std::vector<std::string>::iterator dependIt = it->second.dependencyList.begin(); dependIt != it->second.dependencyList.end(); dependIt++)
	{
		IObject* pInjectobj = createObject(*dependIt);
		if(pInjectobj)
		{
			objectList.push_back(pInjectobj);
			pObject->inject(*dependIt, pInjectobj);
			continue;
		}
		
		for(size_t i = 0; i < objectList.size(); i++)
		{
			objectList[i]->Release();
		}
		pObject->Release();
		return NULL;
	}

	if(!pObject->initlize(pObject->getPropertySet() ) )
	{
		for(size_t i = 0; i < objectList.size(); i++)
		{
			objectList[i]->Release();
		}
		pObject->Release();
		return NULL;
	}
	
	return pObject;
}

bool ComponentManagerImp::loadScheme(const std::string& strSchemeFile)
{
	ComponentManager_XmlHandler xmlHandle;
	TinyXmlParse xmlParse;
	xmlParse.parseXMLFile(xmlHandle, strSchemeFile);

	return true;
}

IComponent* ComponentManagerImp::createComponent(const schemeInfo& componentScheme)
{
	if(m_componentMap.find(componentScheme.strName) != m_componentMap.end() )
		return m_componentMap[componentScheme.strName];

	bool bDisable = m_disableComponent.find(componentScheme.strName) != m_disableComponent.end();
	#ifdef _STATIC_MINIAPR_
		IComponent* pComponent = createComponentStatic(componentScheme.strName, componentScheme.nVersion);
	#else
		DYNLIB_HANDLE hModule = DYNLIB_LOAD(componentScheme.strDllName.c_str() );
		if(!hModule)
		{
			const char* strErrMsg = DYNLIB_ERROR();
			MINIAPR_ERROR("error:%s", strErrMsg);
			MINIAPR_ERROR("组件:[%s]创建失败,加载:[%s]失败",componentScheme.strName.c_str(), componentScheme.strDllName.c_str() );
			return NULL;
		}

		createComponentFun pFUN = (createComponentFun)DYNLIB_GETSYM(hModule, componentScheme.strCreateFun.c_str() );
		if(!pFUN)
		{
			MINIAPR_ERROR("获取组件:[%s]创建接口:[%s]失败", componentScheme.strName.c_str(), componentScheme.strCreateFun.c_str() );
			return NULL;
		}

		IComponent* pComponent = (*pFUN)(componentScheme.nVersion);
	#endif

	pComponent->setPropDefineMap(componentScheme.mapPropDef);
	pComponent->getPropertySet() += componentScheme.propertySet;
	for(size_t i = 0; i < componentScheme.dependencyList.size(); i++)
	{
		SchemeMap::iterator it = m_componentSchemeMap.find(componentScheme.dependencyList[i]);
		if(it == m_componentSchemeMap.end() )
		{
			MINIAPR_ERROR("获取组件:[%s]的依赖组件:[%s]配置失败", componentScheme.strDllName.c_str(), componentScheme.dependencyList[i].c_str() );
			return NULL;
		}		

		IComponent* pInjectComponent = createComponent(it->second);
		if(!pInjectComponent)
		{
			MINIAPR_ERROR("组件:[%s]创建注入组件:[%s]失败", componentScheme.strDllName.c_str(), componentScheme.dependencyList[i].c_str() );
			return NULL;
		}

		if(!pComponent->inject(it->first, pInjectComponent) )
		{
			MINIAPR_ERROR("组件:[%s]inject:[%s]失败", componentScheme.strDllName.c_str(), componentScheme.dependencyList[i].c_str() );
			return NULL;
		}
	}

	
	if(!bDisable && !pComponent->initlize(componentScheme.propertySet) )
	{
		MINIAPR_ERROR("组件:[%s]initlize失败", componentScheme.strDllName.c_str() );
		return NULL;
	}	
	m_componentMap[componentScheme.strName] = pComponent;
	return pComponent;
}

bool ComponentManagerImp::initAllComponent()
{	
	for(SchemeMap::iterator it = m_componentSchemeMap.begin(); it != m_componentSchemeMap.end(); it++)
	{
		if(!createComponent(it->second) )
		{
			string strErrCom = it->second.strName;
			MINIAPR_ERROR("Create %s Fail!!", strErrCom.c_str() );
			MINIAPR_ERRORFLUSH();
			assert(false);
			return false;
		}
		else
		{
			MINIAPR_TRACE("Create %s Success!!", it->second.strName.c_str() );
		}
	}

	return true;
}

ITable* ComponentManagerImp::findTable(const std::string& strTableName)
{
	TableMap::iterator it = m_tableMap.find(strTableName);
	if(it != m_tableMap.end() )
	{
		return it->second;
	}

	return NULL;
}

bool ComponentManagerImp::reloadTable(const std::string& strTableName)
{
	TableMap::iterator it = m_tableMap.find(strTableName);
	if(it == m_tableMap.end() )
		return false;
    ITable* iTable = it->second;
    if(!iTable)
        return false;
    return iTable->reload();
}

ITable* ComponentManagerImp::cloneTableFromStorage(const std::string& strTableName)
{
	TableMap::iterator it = m_tableMap.find(strTableName);
	if(it == m_tableMap.end() )
		return NULL;
    ITable* iTable = it->second;
    if(!iTable)
        return NULL;
    return iTable->cloneTableFromStorage();
}

bool ComponentManagerImp::_getComponentName(IComponent* pComponent, std::string& strComponentName)
{
	ComponentMap::iterator it = m_componentMap.begin();
	for(; it != m_componentMap.end(); it++)
	{
		if(it->second == pComponent)
		{
			strComponentName = it->first;
			return true;
		}
	}
	
	return false;
}

bool ComponentManagerImp::_removeComponent(const std::string& strComponentName)
{
	ComponentMap::iterator it = m_componentMap.find(strComponentName);
	if(it != m_componentMap.end() )
	{
		m_componentMap.erase(it);
		return true;
	}

	return false;
}

void ComponentManagerImp::_doComponentUnload(IComponent* pComponent)
{
	string strComponentName;
	bool bResult = _getComponentName(pComponent, strComponentName);

	if(!bResult)
	{
		MINIAPR_ERROR("试图卸载不存在的组件");
		return;
	}

	MINIAPR_TRACE("卸载组件:%s成功", strComponentName.c_str() );
	
	// 从组件对象中删除
	_removeComponent(strComponentName);
	pComponent->Release();
	
	// 从组件配置中删除
	m_componentSchemeMap.erase(strComponentName);
}

IComponent* ComponentManagerImp::getNextUnloadComponent()
{
	// 先卸载顶层没有被其他组件引用的组件
	typedef map<string, Int32>	ComponentRef;
	ComponentRef localRef;

	for(SchemeMap::iterator it = m_componentSchemeMap.begin(); it != m_componentSchemeMap.end(); it++)
	{
		if(localRef.find(it->first) == localRef.end() )
		{
			localRef[it->first] = 1;
		}
		else
		{
			localRef[it->first]++;
		}

		// 计算依赖项
		for(int i = 0; i < (int)it->second.dependencyList.size(); i++)
		{
			if(localRef.find(it->second.dependencyList[i]) == localRef.end() )
			{
				localRef[it->second.dependencyList[i] ] = 1;
			}
			else
			{
				localRef[it->second.dependencyList[i] ]++;
			}
		}
	}

	for(ComponentRef::iterator it = localRef.begin(); it != localRef.end(); it++)
	{
		if(it->second == 1)
		{
			return m_componentMap[it->first];
		}
	}

	return NULL;
}

void ComponentManagerImp::unLoadAllComponent()
{
	while(IComponent* pComponent = getNextUnloadComponent() )
	{
		bool bResult = pComponent->unLoadComponent();
		if(bResult)
		{
			_doComponentUnload(pComponent);
			continue;
		}
		
		string strComponentName;
		bool bNameResult = _getComponentName(pComponent, strComponentName);
		if(bNameResult) MINIAPR_ERROR("Unload Component:[%s] Fail", strComponentName.c_str() );	
	}
	
	// 删除table
	for(TableMap::iterator it = m_tableMap.begin(); it != m_tableMap.end(); it++)
	{
		delete it->second;
	}
	m_tableMap.clear();
}

void ComponentManagerImp::disableComponent(const string& strComponent)
{
	m_disableComponent.insert(strComponent);
}


