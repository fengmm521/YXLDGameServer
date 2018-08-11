#ifndef __GLOBAL_CFG__H__
#define __GLOBAL_CFG__H__

class GlobalCfg:public ComponentBase<IGlobalCfg, IID_IGlobalCfg>
{
public:

	GlobalCfg();
	~GlobalCfg();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IGlobalCfg Interface
	virtual TC_Config& getGlobalCfg();
	virtual Uint32 getPartID();
	virtual Int32 getInt(const std::string& strKey, Int32 iDef);
	virtual std::string getString(const std::string& strKey, const std::string& strDef);
	
private:
	
	void loadGlobalCfg();
	
private:

	TC_Config	m_cfgFile;

	typedef map<string, string> CfgMap;
	CfgMap m_globalCfg;
};

#endif
