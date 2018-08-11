/*********************************************************
*
*	名称: IComponent.h
*	作者: wuxf
*	时间: 2011-04-16
*	描述: 组件接口类
*********************************************************/

#ifndef __ICOMPONENT_H__
#define __ICOMPONENT_H__

#include "PropSet/PropSet.h"
#include "RefCountAutoPtr/RefCountAutoPtr.h"

#define MAKE_RID(a,b,c,d)		(Uint32)( (a<<24)|(b<<16)|(c<<8)|d)

BEGIN_MINIAPR_NAMESPACE

typedef	Uint32	IID;

#define IID_IComponent	1

class IComponentObjectManager;

class IRecordData
{
public:

	enum
	{
		en_FieldType_Invalid,
		en_FieldType_Int,
		en_FieldType_String,
		en_FieldType_Float,
		en_FieldType_Macro,
	};

	typedef RefcountAutoPtr<IRecordData, true>  Handle;
	
	virtual Int32 getInt(const std::string& strFieldName, Int32 nDef = 0) = 0;
	
	virtual float getFloat(const std::string& strFieldName, float fDef = 0.0f) = 0;

	virtual std::string getString(const std::string& strFieldName, const char* ="") = 0;

	virtual Int32 getRawInt(Int32 nCol, Int32 nDef = 0) = 0;

	virtual float getRawFloat(Int32 nCol, float fDef = 0.0f) = 0;

	virtual std::string getRawString(Int32 nCol, const char* strDef = "") = 0;

	virtual ~IRecordData(){}
};

class IPropDefine;
class ITable
{
public:

	virtual std::string getTableName() = 0;
	
	virtual Int32 findRecord(Int32 nIndexValue) = 0;

	virtual IRecordData::Handle getRecord(Int32 nRow) = 0;

	virtual Int32 getRecordCount() = 0;

	virtual Int32 getFieldCount() = 0;

	virtual std::string getFieldName(int nFileIndex) = 0;

	virtual Int32 getInt(Int32 nRow, const std::string& strFieldName, Int32 nDef = 0) = 0;

	virtual float getFloat(Int32 nRow, const std::string& strFieldName, float fDef = 0.0f) = 0;

	virtual std::string getString(Int32 nRow, const std::string& strFieldName, const char* strDef ="") = 0;

	virtual Int32 getRawInt(Int32 nRow, Int32 nCol, Int32 nDef = 0) = 0;

	virtual float getRawFloat(Int32 nRow, Int32 nCol, float fDef = 0.0f) = 0;

	virtual std::string getRawString(Int32 nRow, Int32 nCol, const char* strDef = "") = 0;

	virtual Int32 getFieldType(Int32 nFiledIndex) = 0;

	virtual bool initPropSet(IPropDefine* pPropDef, int nRow, PropertySet& propSet, bool bErrLog) = 0;

    virtual bool reload() = 0;
    virtual ITable* cloneTableFromStorage() = 0;
    
	virtual ~ITable(){}
};

class IPropDefine
{
public:

	virtual ~IPropDefine(){}
		
	virtual int getPropKey(const std::string& strKeyName, int nDef = -1) = 0;

	virtual std::string getPropType(const std::string& strKeyName) = 0;

	virtual IPropDefine* clone() const = 0;

	virtual string getPropName(int iKey) = 0;
};

class IComponent
{
public:

	virtual bool initlize(const PropertySet& propSet) = 0;
	
	virtual bool inject(const std::string& componentName, IComponent* pComponent) = 0;

	virtual void* queryInterface(IID interfaceId) = 0;

	virtual IPropDefine* getPropDefine(const std::string& strName) const = 0;

	virtual void setPropDefineMap(const std::map<std::string, IPropDefine*>& propDefMap) = 0;

	virtual PropertySet& getPropertySet() = 0;

	virtual Int32 getProperty(PropertySet::PropertyKey key, Int32 nDef = 0) = 0;

	virtual const char* getProperty(PropertySet::PropertyKey key, const char* pDef = "") = 0;

	virtual float getProperty(PropertySet::PropertyKey key, float fDef = 0.0f) = 0;

	virtual Int64 getInt64Property(PropertySet::PropertyKey key, Int64 nDef = 0) = 0;

	virtual void setProperty(PropertySet::PropertyKey key, Int32 nValue) = 0;

	virtual void setProperty(PropertySet::PropertyKey key, const char* strValue) = 0;

	virtual void setProperty(PropertySet::PropertyKey key, float fValue) = 0;

	virtual void setInt64Property(PropertySet::PropertyKey key, Int64 nValue) = 0;

	virtual IID getInterfaceID() const = 0;

	virtual bool unLoadComponent() = 0;

	virtual void Release() = 0;

protected:
	virtual ~IComponent(){}
};

class IObject
{
public:
	virtual bool initlize(const PropertySet& propSet) = 0;

	virtual bool inject(const std::string& strClassName, IObject* pObject) = 0;
	
	virtual PropertySet& getPropertySet() = 0;

	virtual Int32 getProperty(PropertySet::PropertyKey key, Int32 nDef = 0) = 0;

	virtual const char* getProperty(PropertySet::PropertyKey key, const char* pDef = "") = 0;

	virtual float getProperty(PropertySet::PropertyKey key, float fDef = 0.0f) = 0;

	virtual Int64 getInt64Property(PropertySet::PropertyKey key, Int64 nDef = 0) = 0;

	virtual void setProperty(PropertySet::PropertyKey key, Int32 nValue) = 0;

	virtual void setProperty(PropertySet::PropertyKey key, const char* strValue) = 0;

	virtual void setProperty(PropertySet::PropertyKey key, float fValue) = 0;

	virtual void setInt64Property(PropertySet::PropertyKey key, Int64 nValue) = 0;

	virtual void Release() = 0;

protected:
	virtual ~IObject(){}
};

class IComponentObjectManager
{
public:

	virtual ~IComponentObjectManager(){}

	virtual IComponent* find(const std::string& strComponentName) = 0;

	virtual IObject* createObject(const std::string& strClassName) = 0;

	virtual ITable* findTable(const std::string& strTableName) = 0;

	virtual bool loadScheme(const std::string& strSchemeFile) = 0;

	virtual void unLoadAllComponent() = 0;

    //会重新加载组件管理器中相应的csv表内容
    //慎重使用,需要避免以下情况:
    //         比如代码里有同时直接使用表数据,另外也有使用根据表数据生成的组织过的数据
    //         此时如果只重新加载了表数据但未更新组织过的数据，有特殊数据可能导致冲突
	virtual bool reloadTable(const std::string& strTableName) = 0;
    //根据己有表的路径重新读取最新文件生成一份拷贝，不调整组件管理器里己有的表内容
    //返回的指针需要调用方自行处理内存释放
	virtual ITable* cloneTableFromStorage(const std::string& strTableName) = 0;

	// 功能: 禁止某些组建(这些组建能查询到，但是初始化直接返回,组件在多个服务器公用容易出现这类问题)
	virtual void disableComponent(const string& strComponent) = 0;
};

END_MINIAPR_NAMESPACE

MINIAPR::IComponentObjectManager* getCompomentObjectManager();

template<class T>
T* getComponent(const std::string& strComponentName, MINIAPR::IID rID)
{
	MINIAPR::IComponent* pComponent = getCompomentObjectManager()->find(strComponentName);
	if(!pComponent) return NULL;

	T* pInterface = static_cast<T*>(pComponent->queryInterface(rID) );
	return pInterface;
}


#endif

