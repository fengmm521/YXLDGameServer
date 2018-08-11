/*********************************************************
*
*	名称: Iprofiler.h
*	作者: wuxf
*	时间: 2011-04-18
*	描述: 性能监控接口
*********************************************************/
#ifndef __IPROFILERMANAGER_H__
#define __IPROFILERMANAGER_H__

#include <typeinfo>

#define IID_IProfilerManager	MAKE_RID('p','r', 'm', 'g')

BEGIN_MINIAPR_NAMESPACE

class IProfilerManager:public IComponent
{
public:

	/*!
	\brief 
		开始性能监控
	\param[in]const std::string& strProfileName 
		性能监控项名字
	\return void 
	*/
	virtual void beginProfiler(const std::string& strProfileName) = 0;

	/*!
	\brief 
		完成性能监控
	\param[in]const std::string& strProfilerName
		监控项名字
	\return void 
	*/
	virtual void endProfiler(const std::string& strProfilerName) = 0;


	virtual void incObjCount(const std::string& strClassName, int iSingleSize) = 0;

	virtual void decObjCount(const std::string& strClassName) = 0;
};

class  ProfileHelp
{
public:
	
	ProfileHelp(const char* profileName);
	~ProfileHelp();

private:
	std::string	m_strProfileName;
	static IProfilerManager* m_pProfileManager;
};


template<class T>
class ObjCounter
{
public:

	ObjCounter()
	{
		IProfilerManager* pProfileManager = getComponent<IProfilerManager>("ProfileManager", IID_IProfilerManager);
		if(pProfileManager)
		{
			pProfileManager->incObjCount(typeid(T).name(), (int)sizeof(T) );
		}
	}

	~ObjCounter()
	{
		IProfilerManager* pProfileManager = getComponent<IProfilerManager>("ProfileManager", IID_IProfilerManager);
		if(pProfileManager)
		{
			pProfileManager->decObjCount(typeid(T).name() );
		}
	}
};



#define PROFILE_MONITOR(name)		ProfileHelp stackObject(name);
#define PROFILE_MONITOR_INDEX(index, name)		ProfileHelp stackObject##index(name);
#define PROFILE_OBJ_COUNTER(name)     ObjCounter<name> m_hideObjCounter;


END_MINIAPR_NAMESPACE

#endif
