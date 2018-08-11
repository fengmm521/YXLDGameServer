#ifndef __GLOBAL_CFG_H__
#define __GLOBAL_CFG_H__


#define IID_IGlobalCfg		MAKE_RID('g', 'b', 'c', 'g')

class IGlobalCfg:public IComponent
{
public:

	virtual TC_Config& getGlobalCfg() = 0;
	virtual Uint32 getPartID() = 0;
	virtual Int32 getInt(const std::string& strKey, Int32 iDef) = 0;
	virtual std::string getString(const std::string& strKey, const std::string& strDef) = 0;
};


#endif
