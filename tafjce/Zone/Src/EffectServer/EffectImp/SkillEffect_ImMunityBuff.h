#ifndef __SKILLEFFECT_IMMUNITYBUFF_H__
#define __SKILLEFFECT_IMMUNITYBUFF_H__

#include "SkillEffect_Helper.h"

class SkillEffect_ImMunityBuff:public SkillEffect_Helper
{
public:

	SkillEffect_ImMunityBuff(){}

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_ImMunityBuff;}
	virtual vector<string> getTypeString();
	virtual bool isImMunityBuff(int iBuffID, const EffectContext& effectContext);

//private:

	//vector<int> m_imMunityTypeList;
};


bool SkillEffect_ImMunityBuff::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	return true;
}


bool SkillEffect_ImMunityBuff::isImMunityBuff(int iBuffID, const EffectContext& effectContext)
{
	IBuffFactory* pBuffFactory = getComponent<IBuffFactory>(COMPNAME_BuffFactory, IID_IBuffFactory);
	assert(pBuffFactory);

	const IEntityBuff* pBuffProtoType = pBuffFactory->getBuffPrototype(iBuffID);
	assert(pBuffProtoType);

	int iBuffType = pBuffProtoType->getBuffType();

	//bool bResult = std::find(m_imMunityTypeList.begin(), m_imMunityTypeList.end(), iBuffType) != m_imMunityTypeList.end();

	vector<int>* pImmunityList = (vector<int>*)(effectContext.getInt64(PROP_EFFECT_IMMUNITY_BUFFTYPE, 0));

	if(!pImmunityList) return false;

	bool bResult = std::find(pImmunityList->begin(), pImmunityList->end(), iBuffType) != pImmunityList->end();

	return bResult;
}


bool SkillEffect_ImMunityBuff::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList[0] == "免疫状态类型") && (paramList.size() > 2) )
	{
		vector<int>* pImmunityList = new vector<int>();
		for(size_t i = 1; i < paramList.size(); i++)
		{
			pImmunityList->push_back(AdvanceAtoi(paramList[i]));
		}

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt64(PROP_EFFECT_IMMUNITY_BUFFTYPE, (Int64)pImmunityList);

		return true;
	}

	return false;
}


vector<string> SkillEffect_ImMunityBuff::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("免疫状态类型");

	return resultList;
}


#endif


