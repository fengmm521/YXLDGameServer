#include "MiniAprPch.h"
#include "ProfilerManager.h"

using namespace MINIAPR;

extern "C"  IComponent* createProfilerManager(Int32 /*dwVersion*/)
{
	return new ProfilerManager;
}

extern "C" MINIAPR::IProfilerManager* cast2ProfileManager(MINIAPR::IComponent* pCom)
{
	IProfilerManager* pProfileManager = static_cast<IProfilerManager*>(pCom);
	return pProfileManager;
}

ProfilerManager::ProfilerManager():m_pProfilerTimer(NULL)
{
}

ProfilerManager::~ProfilerManager()
{
}

bool ProfilerManager::initlize(const PropertySet& /*propSet*/)
{
	if(!m_pProfilerTimer)
	{
		//APR_ERROR("没有找到性能监视器计时组件");
		return false;
	}

	m_pProfilerTimer->resetTimer();
	return true;
}

bool ProfilerManager::inject(const std::string& /*componentName*/, IComponent* pComponent)
{
	if(pComponent->queryInterface(IID_IProfilerStrategy) )
	{
		m_profilerStrategyList.push_back( (IProfilerStrategy*)pComponent->queryInterface(IID_IProfilerStrategy) );
	}
	else if(pComponent->queryInterface(IID_IProfilerTimer) )
	{
		m_pProfilerTimer = (IProfilerTimer*)pComponent->queryInterface(IID_IProfilerTimer);
	}

	return true;
}

void ProfilerManager::beginProfiler(const std::string& strProfileName)
{
	for(ProfileStrategyList::iterator it = m_profilerStrategyList.begin(); it != m_profilerStrategyList.end(); it++)
	{
		Uint64 ddProfilerTime = m_pProfilerTimer->getMicroseconds();
		(*it)->onBeginProfiler(strProfileName, ddProfilerTime);
	}
}

void ProfilerManager::endProfiler(const std::string& strProfilerName)
{
	for(ProfileStrategyList::iterator it = m_profilerStrategyList.begin(); it != m_profilerStrategyList.end(); it++)
	{
		Uint64 ddProfilerTime = m_pProfilerTimer->getMicroseconds();
		(*it)->onEndProfiler(strProfilerName, ddProfilerTime);
	}
}

void ProfilerManager::incObjCount(const std::string& strClassName, int iSingleSize)
{
	for(ProfileStrategyList::iterator it = m_profilerStrategyList.begin(); it != m_profilerStrategyList.end(); it++)
	{
		(*it)->onObjCountInc(strClassName, iSingleSize);
	}
}

void ProfilerManager::decObjCount(const std::string& strClassName)
{
	for(ProfileStrategyList::iterator it = m_profilerStrategyList.begin(); it != m_profilerStrategyList.end(); it++)
	{
		(*it)->onObjCountDec(strClassName);
	}
}


IProfilerManager* ProfileHelp::m_pProfileManager = NULL;

ProfileHelp::ProfileHelp(const char* profileName):m_strProfileName(profileName)
{	
	if(!m_pProfileManager)
	{
		m_pProfileManager = (IProfilerManager*)getCompomentObjectManager()->find("ProfileManager");
	}

	m_pProfileManager->beginProfiler(m_strProfileName);
}

ProfileHelp::~ProfileHelp()
{
	if(!m_pProfileManager) return;
	m_pProfileManager->endProfiler(m_strProfileName);
}
