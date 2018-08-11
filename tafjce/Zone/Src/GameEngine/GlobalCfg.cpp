#include "GameEnginePch.h"
#include "GlobalCfg.h"
#include "util/tc_file.h"


extern bool isGameServer();

extern "C" IComponent* createGlobalCfg(Int32)
{
	return new GlobalCfg;
}

GlobalCfg::GlobalCfg()
{
}

GlobalCfg::~GlobalCfg()
{
}

bool GlobalCfg::initlize(const PropertySet& propSet)
{
    //m_cfgFile.parseFile(ServerConfig::BasePath+ServerConfig::ServerName + ".conf");
    m_cfgFile.parseFile(ServerConfig::ServerName + ".conf");
	
	loadGlobalCfg();

	return true;
}

void GlobalCfg::loadGlobalCfg()
{
	ITable* pTable = getCompomentObjectManager()->findTable("GlobalConfig");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		string strKey = pTable->getString(i, "名字");
		string strValue = pTable->getString(i, "数值");
		std::pair<CfgMap::iterator, bool> ret = m_globalCfg.insert(std::make_pair(strKey, strValue) );
		assert(ret.second);
	}
}

TC_Config& GlobalCfg::getGlobalCfg()
{
	return m_cfgFile;
}

Uint32 GlobalCfg::getPartID()
{	
	vector<unsigned short> vPartGrp = TC_Common::sepstr<unsigned short>(m_cfgFile.get("/Zone<cfg-part-id>"),".");
	Uint32 cfg_part_id = SvrIDToInt((char)vPartGrp[0],(char)vPartGrp[1],(char)vPartGrp[2],(char)vPartGrp[3]);

	return cfg_part_id;
}

Int32 GlobalCfg::getInt(const std::string& strKey, Int32 iDef)
{
	CfgMap::iterator it = m_globalCfg.find(strKey);
	if(it == m_globalCfg.end() )
	{
		return iDef;
	}
	string strValue = it->second;
	
	return atoi(strValue.c_str() );
}

std::string GlobalCfg::getString(const std::string& strKey, const std::string& strDef)
{
	CfgMap::iterator it = m_globalCfg.find(strKey);
	if(it == m_globalCfg.end() )
	{
		return strDef;
	}

	string strValue = it->second;
	
	return strValue;
}





