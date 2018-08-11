/********************************************************************
created:	2012-11-17
author:		Fish (于国平)
summary:	组件id
*********************************************************************/
#pragma once
#include "FmConfig.h"

NS_FM_BEGIN


enum DataFileType
{
	DataFileHeroBorn = 0,
	DataFileHeroCreate,
	DataFileCreateName,
	DataFileScene,
	DataFileSkill,
	DataFileSkillEffect,
	DataFileSceneSection,
	DataFileItem,
	DataFileFightSoul,
	DataFileFightSoulExchange,
	DataFileGodAniaml,
	DataFileLegionContribute,
	DataFileLegionBless,
	DataFileHonorGoods,
	DataFileMonster,
	DataFileLegionCity,
	DataFileGift,
	DataFilePerfectPassAward,
	DataFileBuff,
	DataFileOpenFunction,
	DataFileAnnnouceMent,
	DataFileTask,
	DataFileVip,
	DataFileEquip,
	//DataFileEquipCombine,
	DataFileEquipSuit,
	DataFileDreamLandSection,
	DataFileHeroTallentBorn,
	DataFileHeroLevelStepGrow,
	DataFileHeroLevelExp,
	DataFileActorSkill,
	DataFileFavorite,
	DataFileHeroQuality,
	DataFileQualityProp,
	DataFileManorResLevel,
	DataFileManorTieJiangPu,
	DataFileManorWuHunDian,
	DataFileManorProtect,
	DataFileArenaAward,
	DataFilePayment,
	DataFileCheckIn,
	DataFileOperateActive,
	DataFileAccumulatePayMent,
	DataFileAccumlateLogin,
	DataFilePushMent,
	DATA_FILE_COUNT,   // data数量一定在最后
};

NS_FM_END