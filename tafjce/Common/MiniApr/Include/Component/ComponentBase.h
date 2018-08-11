/*********************************************************
*
*	名称: ComponentBase.h
*	作者: wuxf
*	时间: 2011-04-16
*	描述: 组件接口辅助类
*********************************************************/
#ifndef __COMPONENT_BASE_H__
#define __COMPONENT_BASE_H__

#include "IComponent.h"

BEGIN_MINIAPR_NAMESPACE

template<class T, IID rID>
class ComponentBase:public T
{
public:

	ComponentBase(){}

	virtual ~ComponentBase()
	{
		for(std::map<std::string, IPropDefine*>::iterator it = m_propDefMap.begin(); it != m_propDefMap.end(); it++)
		{
			delete it->second;
		}
		m_propDefMap.clear();
	}

	virtual bool initlize(const PropertySet& /*propSet*/){return true;}

	virtual bool inject(const std::string& /*componentName*/, IComponent* /*pComponent*/){return true;}

	virtual void* queryInterface(IID interfaceId)
	{
		if(interfaceId == rID)
		{
			return static_cast<T*>(this);
		}
		else if(interfaceId == IID_IComponent)
		{
			return static_cast<IComponent*>(this);
		}

		return NULL;
	}

	virtual IPropDefine* getPropDefine(const std::string& strName) const
	{
		std::map<std::string, IPropDefine*>::const_iterator it = m_propDefMap.find(strName);
		if(it == m_propDefMap.end() )
		{
			return NULL;
		}
	
		return it->second;
	}

	virtual void setPropDefineMap(const std::map<std::string, IPropDefine*>& propDefMap)
	{
		for(std::map<std::string, IPropDefine*>::const_iterator it = propDefMap.begin(); it != propDefMap.end(); it++)
		{
			IPropDefine* pNewDefine = (it->second)->clone();
			m_propDefMap[it->first] = pNewDefine;	
		}
	}

	virtual PropertySet& getPropertySet(){return m_PropertySet;}

	virtual Int32 getProperty(PropertySet::PropertyKey key, Int32 nDef = 0)
	{
		return m_PropertySet.getInt(key, nDef);
	}

	virtual const char* getProperty(PropertySet::PropertyKey key, const char* pDef = "")
	{
		return m_PropertySet.getString(key, pDef);
	}

	virtual float getProperty(PropertySet::PropertyKey key, float fDef = 0.0f)
	{
		return m_PropertySet.getFloat(key, fDef);
	}

	virtual Int64 getInt64Property(PropertySet::PropertyKey key, Int64 nDef = 0)
	{
		return m_PropertySet.getInt64(key, nDef);
	}

	virtual void setProperty(PropertySet::PropertyKey key, Int32 nValue)
	{
		m_PropertySet.setInt(key, nValue);
	}

	virtual void setProperty(PropertySet::PropertyKey key, const char* strValue)
	{
		m_PropertySet.setString(key, strValue);
	}

	virtual void setProperty(PropertySet::PropertyKey key, float fValue)
	{
		m_PropertySet.setFloat(key, fValue);
	}

	virtual void setInt64Property(PropertySet::PropertyKey key, Int64 nValue64)
	{
		m_PropertySet.setInt64(key, nValue64);
	}

	virtual IID getInterfaceID() const
	{
		return rID;
	}

	virtual void Release(){delete this;}

	virtual bool unLoadComponent(){return true;}

private:
	PropertySet	m_PropertySet;
	std::map<std::string, IPropDefine*> m_propDefMap;
};

template<class T>
class ObjectBase:public T
{
protected:
	virtual ~ObjectBase(){}
public:

	virtual bool initlize(const PropertySet& propSet){return true;}

	virtual bool inject(const std::string& strClassName, IObject* pObject){return true;}

	virtual PropertySet& getPropertySet()
	{
		return m_PropertySet;
	}

	virtual Int32 getProperty(PropertySet::PropertyKey key, Int32 nDef = 0)
	{
		return m_PropertySet.getInt(key, nDef);
	}

	virtual const char* getProperty(PropertySet::PropertyKey key, const char* pDef = "")
	{
		return m_PropertySet.getString(key, pDef);
	}

	virtual float getProperty(PropertySet::PropertyKey key, float fDef = 0.0f)
	{
		return m_PropertySet.getFloat(key, fDef);
	}

	virtual Int64 getInt64Property(PropertySet::PropertyKey key, Int64 nDef)
	{
		return m_PropertySet.getInt64(key, nDef);
	}

	virtual void setProperty(PropertySet::PropertyKey key, Int32 nValue)
	{
		m_PropertySet.setInt(key, nValue);
	}

	virtual void setProperty(PropertySet::PropertyKey key, const char* strValue)
	{
		m_PropertySet.setString(key, strValue);
	}

	virtual void setProperty(PropertySet::PropertyKey key, float fValue)
	{
		m_PropertySet.setFloat(key, fValue);
	}

	virtual void setInt64Property(PropertySet::PropertyKey key, Int64 nValue64)
	{
		m_PropertySet.setInt64(key, nValue64);
	}

	virtual void Release(){delete this;}

private:
	PropertySet		m_PropertySet;

};

END_MINIAPR_NAMESPACE

#endif
