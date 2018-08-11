/********************************************************************
created:	2012-05-20
author:		pelog
summary:	skill
*********************************************************************/
#pragma once

#include "FmConfig.h"
#include "Numeric/SkillData.h"
#include "GameData/DataManager.h"
#include "Numeric/SkillEffectData.h"


NS_FM_BEGIN
struct SkillTargetData;

struct SkillResultData
{
	int skillId;
	int type;
	int angerAfterSkill;

	int aGodAnimalAnger;
	int tGodAnimalAnger;
	int buttletIndex;
	bool isHurt;
	bool bIsStageSkill;
	bool bContinueSkill;
	bool bContinueKill;
	
	vector<SkillTargetData> skillTargetList;

	SkillResultData()
	{
		init();
	}

	void init()
	{
		bIsStageSkill = false;
		bContinueSkill = false;
		bContinueKill = false;
		type = 0;
		skillId = 0;
		skillTargetList.clear();
		buttletIndex = 0;
		angerAfterSkill = 0;
		aGodAnimalAnger = 0;
		tGodAnimalAnger = 0;
		isHurt = false;
	}
};

struct SkillTargetData
{
	uint entityId;
	int damageHP;// 正数为伤害，负数为治疗
	int hitResult;
	int curHP;
	int curAnger;
	int skipType;

	SkillTargetData()
	{
		init();
	}

	void init()
	{
		entityId = 0;
		damageHP = 0;
		hitResult = 0;
		curHP = 0;
		curAnger = 0;
		skipType = 0;
	}
};

static stSkillData* GetSkillDataByID(int id)
{
	return CDataManager::GetInstance().GetGameData<stSkillData>(DataFileSkill, id);
}

static stSkillEffectData* GetSkillEffectDataByID(int id)
{
	return CDataManager::GetInstance().GetGameData<stSkillEffectData>(DataFileSkillEffect, id);
}

static stSkillEffectData* GetSkillEffectDataBySkillID(int id)
{
	stSkillData* skillData = GetSkillDataByID(id);
	if (skillData)
	{
		return CDataManager::GetInstance().GetGameData<stSkillEffectData>(DataFileSkillEffect, skillData->m_skillEffect);
	}
	return NULL;
}

NS_FM_END