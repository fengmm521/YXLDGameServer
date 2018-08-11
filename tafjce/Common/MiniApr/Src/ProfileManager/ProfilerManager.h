#ifndef __PROFILER_MANAGER_H__
#define __PROFILER_MANAGER_H__

BEGIN_MINIAPR_NAMESPACE

class IProfilerStrategy;
class IProfilerTimer;
class ITimerComponent;
class ProfilerManager:public ComponentBase<IProfilerManager, IID_IProfilerManager>
{
public:

	ProfilerManager();
	~ProfilerManager();
	
	virtual bool initlize(const PropertySet& propSet);
	virtual bool inject(const std::string& componentName, IComponent* pComponent);

	virtual void beginProfiler(const std::string& strProfileName);
	virtual void endProfiler(const std::string& strProfilerName);
	virtual void incObjCount(const std::string& strClassName, int iSingleSize);
	virtual void decObjCount(const std::string& strClassName);
	
private:
	
	typedef std::vector<IProfilerStrategy*>	ProfileStrategyList;
	ProfileStrategyList		m_profilerStrategyList;
	IProfilerTimer*			m_pProfilerTimer;
};

END_MINIAPR_NAMESPACE

extern "C" MINIAPR::IProfilerManager* cast2ProfileManager(MINIAPR::IComponent* pCom);

#endif
