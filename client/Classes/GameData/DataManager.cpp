#include "DataManager.h"
#include "Enum/FmNumricEnum.h"
#include "../CCFileUtils.h"
#include "Numeric/HeroBornData.h"
#include "Numeric/HeroCreateData.h"
#include "Numeric/CreateNameData.h"
#include "Numeric/SceneData.h"
#include "Numeric/SkillData.h"
#include "Numeric/SkillEffectData.h"
#include "Numeric/SceneSectionData.h"
#include "Numeric/ItemData.h"
#include "Numeric/FightSoulData.h"
#include "Numeric/GodAnimalData.h"
#include "Numeric/FightSoulExchangeData.h"
#include "Numeric/LegionContributeData.h"
#include "Numeric/LegionBlessData.h"
#include "Numeric/HonorGoodsData.h"
#include "Numeric/PaymentData.h"
#include "Numeric/MonsterData.h"
#include "Numeric/LegionCityData.h"
#include "Numeric/GiftData.h"
#include "Numeric/PerfectPassAwardData.h"
#include "Numeric/BuffData.h"
#include "Numeric/OpenFunctionData.h"
#include "Numeric/AnnounceMentData.h"
#include "Numeric/TaskData.h"
#include "Numeric/VipData.h"
#include "Numeric/EquipData.h"
#include "Numeric/EquipSuitData.h"
#include "Numeric/DreamLandSectionData.h"
#include "Numeric/HeroTallentBornData.h"
#include "Numeric/LevelStepGrowData.h"
#include "Numeric/HeroLevelExpData.h"
#include "Numeric/ActorSkillData.h"
#include "Numeric/FavoriteData.h"
#include "Numeric/HeroQualityData.h"
#include "Numeric/QualityPropData.h"
#include "Numeric/ManorResLevelData.h"
#include "Numeric/ManorTieJiangPuData.h"
#include "Numeric/ManorWuHunDianData.h"
#include "Numeric/ManorProtectData.h"
#include "Numeric/CheckInData.h"
#include "Numeric/ArenaAwardData.h"
#include "Numeric/OperateActiveData.h"
#include "Numeric/AccumulatePaymentData.h"
#include "Numeric/AccumlateLoginData.h"
#include "Numeric/PushMentData.h"
USING_NS_CC;

NS_FM_BEGIN
#define  LOAD_DATA(dataType, dataStruct, fileName) \
{\
	CGameData<dataStruct, DefaultMkKey>* gamedata = new CGameData<dataStruct, DefaultMkKey>;\
	unsigned long size = 0;\
	unsigned char *pBytes = NULL;\
	std::string numericPath = cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(fileName);\
	pBytes = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(numericPath.c_str(), "rb", &size);\
	MemStream stream(pBytes, size);\
	gamedata->LoadData(stream);\
	if (NULL != m_DataStorage && dataType >= 0 && dataType < DATA_FILE_COUNT && NULL == m_DataStorage[dataType])\
	{\
		m_DataStorage[dataType] = gamedata;\
	}\
	else\
	{\
		gamedata->Dispose<dataStruct>();\
		(gamedata);\
	}\
}

#define  UNLOAD_DATA(dataStruct, pGameData)\
{\
	if (NULL != pGameData)\
	{\
		CGameData<dataStruct, DefaultMkKey>*gamedata = static_cast<CGameData<dataStruct, DefaultMkKey>* >(pGameData);\
		gamedata->Dispose<dataStruct>(); \
		FM_SAFE_DELETE(gamedata);\
	}\
}

CDataManager::CDataManager()
{
	// 初始化数组
	m_DataStorage = new IGameData*[DATA_FILE_COUNT];
	memset(m_DataStorage, 0, sizeof(int) * DATA_FILE_COUNT);
}

CDataManager::~CDataManager()
{
	//add by jesson memory leak
	delete[] m_DataStorage;
}

void CDataManager::Init()
{
	CCFileUtils::sharedFileUtils()->addSearchPath("numeric/");
	LOAD_DATA(DataFileHeroBorn, stHeroBornData, "HeroBornData.bin");
	LOAD_DATA(DataFileHeroCreate, stHeroCreateData, "HeroCreateData.bin");
	LOAD_DATA(DataFileCreateName, stCreateNameData, "CreateNameData.bin");
	LOAD_DATA(DataFileScene, stSceneData, "SceneData.bin");
	LOAD_DATA(DataFileSkill, stSkillData, "SkillData.bin");
	LOAD_DATA(DataFileSkillEffect, stSkillEffectData, "SkillEffectData.bin");
	LOAD_DATA(DataFileSceneSection, stSceneSectionData, "SceneSectionData.bin");
	LOAD_DATA(DataFileItem, stItemData, "ItemData.bin");
	LOAD_DATA(DataFileFightSoul, stFightSoulData, "FightSoulData.bin");
	LOAD_DATA(DataFileFightSoulExchange, stFightSoulExchangeData, "FightSoulExchangeData.bin");
	LOAD_DATA(DataFileGodAniaml, stGodAnimalData, "GodAnimalData.bin");
	LOAD_DATA(DataFileLegionContribute, stLegionContributeData, "LegionContributeData.bin");
	LOAD_DATA(DataFileLegionBless, stLegionBlessData, "LegionBlessData.bin");
	LOAD_DATA(DataFileHonorGoods, stHonorGoodsData, "HonorGoodsData.bin");
	LOAD_DATA(DataFilePayment, stPaymentData, "PaymentData.bin");
	LOAD_DATA(DataFileMonster, stMonsterData, "MonsterData.bin");
	LOAD_DATA(DataFileLegionCity, stLegionCityData, "LegionCityData.bin");
	LOAD_DATA(DataFileGift, stGiftData, "GiftData.bin");
	LOAD_DATA(DataFilePerfectPassAward, stPerfectPassAwardData, "PerfectPassAwardData.bin");
	LOAD_DATA(DataFileBuff, stBuffData, "BuffData.bin");
	LOAD_DATA(DataFileOpenFunction, stOpenFunctionData, "OpenFunctionData.bin");
	LOAD_DATA(DataFileAnnnouceMent, stAnnounceMentData, "AnnounceMentData.bin");
	LOAD_DATA(DataFileTask, stTaskData, "TaskData.bin");
	LOAD_DATA(DataFileVip, stVipData, "VipData.bin");
	LOAD_DATA(DataFileEquip, stEquipData, "EquipData.bin");
	LOAD_DATA(DataFileEquipSuit, stEquipSuitData, "EquipSuitData.bin");
	LOAD_DATA(DataFileDreamLandSection, stDreamLandSectionData, "DreamLandSectionData.bin");
	//LOAD_DATA(DataFileEquipCombine, stEquipCombineData, "EquipCombineData.bin");
	LOAD_DATA(DataFileHeroTallentBorn, stHeroTallentBornData, "HeroTallentBornData.bin");
	LOAD_DATA(DataFileHeroLevelStepGrow, stLevelStepGrowData, "LevelStepGrowData.bin");
	LOAD_DATA(DataFileHeroLevelExp, stHeroLevelExpData, "HeroLevelExpData.bin");
	LOAD_DATA(DataFileActorSkill, stActorSkillData, "ActorSkillData.bin");
	LOAD_DATA(DataFileFavorite,stFavoriteData,"FavoriteData.bin");
	LOAD_DATA(DataFileHeroQuality,stHeroQualityData,"HeroQualityData.bin");
	LOAD_DATA(DataFileQualityProp,stQualityPropData,"QualityPropData.bin");
	LOAD_DATA(DataFileManorResLevel, stManorResLevelData, "ManorResLevelData.bin");
	LOAD_DATA(DataFileManorTieJiangPu, stManorTieJiangPuData, "ManorTieJiangPuData.bin");
	LOAD_DATA(DataFileManorWuHunDian, stManorWuHunDianData, "ManorWuHunDianData.bin");	
	LOAD_DATA(DataFileManorProtect, stManorProtectData, "ManorProtectData.bin");	
	LOAD_DATA(DataFileCheckIn, stCheckInData, "CheckInData.bin");	
	LOAD_DATA(DataFileArenaAward, stArenaAwardData, "ArenaAwardData.bin");	
	LOAD_DATA(DataFileOperateActive, stOperateActiveData, "OperateActiveData.bin");	
	LOAD_DATA(DataFileAccumulatePayMent, stAccumulatePaymentData, "AccumulatePaymentData.bin");
	LOAD_DATA(DataFileAccumlateLogin, stAccumlateLoginData, "AccumlateLoginData.bin");
	LOAD_DATA(DataFilePushMent, stPushMentData, "PushMentData.bin");
}

void CDataManager::ShutDown()
{
	UNLOAD_DATA(stHeroBornData,m_DataStorage[DataFileHeroBorn]);
	UNLOAD_DATA(stHeroCreateData,m_DataStorage[DataFileHeroCreate]);
	UNLOAD_DATA(stCreateNameData,m_DataStorage[DataFileCreateName]);
	UNLOAD_DATA(stSceneData,m_DataStorage[DataFileScene]);
	UNLOAD_DATA(stSkillData,m_DataStorage[DataFileSkill]);
	UNLOAD_DATA(stSkillEffectData,m_DataStorage[DataFileSkillEffect]);
	UNLOAD_DATA(stSceneSectionData,m_DataStorage[DataFileSceneSection]);
	UNLOAD_DATA(stItemData,m_DataStorage[DataFileItem]);
	UNLOAD_DATA(stFightSoulData,m_DataStorage[DataFileFightSoul]);
	UNLOAD_DATA(stGodAnimalData,m_DataStorage[DataFileGodAniaml]);
	UNLOAD_DATA(stLegionContributeData,m_DataStorage[DataFileLegionContribute]);
	UNLOAD_DATA(stLegionBlessData,m_DataStorage[DataFileLegionBless]);
	UNLOAD_DATA(stHonorGoodsData,m_DataStorage[DataFileHonorGoods]);
	UNLOAD_DATA(stPaymentData,m_DataStorage[DataFilePayment]);
	UNLOAD_DATA(stMonsterData,m_DataStorage[DataFileMonster]);
	UNLOAD_DATA(stLegionCityData,m_DataStorage[DataFileLegionCity]);
	UNLOAD_DATA(stGiftData,m_DataStorage[DataFileGift]);
	UNLOAD_DATA(stPerfectPassAwardData,m_DataStorage[DataFilePerfectPassAward]);
	UNLOAD_DATA(stBuffData, m_DataStorage[DataFileBuff]);
	UNLOAD_DATA(stOpenFunctionData, m_DataStorage[DataFileOpenFunction]);
	UNLOAD_DATA(stAnnounceMentData, m_DataStorage[DataFileAnnnouceMent]);
	UNLOAD_DATA(stTaskData, m_DataStorage[DataFileTask]);
	UNLOAD_DATA(stVipData, m_DataStorage[DataFileVip]);
	UNLOAD_DATA(stEquipData, m_DataStorage[DataFileEquip]);
	UNLOAD_DATA(stEquipSuitData, m_DataStorage[DataFileEquipSuit]);
	UNLOAD_DATA(stDreamLandSectionData, m_DataStorage[DataFileDreamLandSection]);
	//UNLOAD_DATA(stEquipCombineData, m_DataStorage[DataFileEquipCombine]);
	UNLOAD_DATA(stHeroTallentBornData, m_DataStorage[DataFileHeroTallentBorn]);
	UNLOAD_DATA(stLevelStepGrowData, m_DataStorage[DataFileHeroLevelStepGrow]);
	UNLOAD_DATA(stHeroLevelExpData, m_DataStorage[DataFileHeroLevelExp]);
	UNLOAD_DATA(stActorSkillData, m_DataStorage[DataFileActorSkill]);
	UNLOAD_DATA(stFavoriteData,m_DataStorage[DataFileFavorite]);
	UNLOAD_DATA(stHeroQualityData,m_DataStorage[DataFileHeroQuality]);
	UNLOAD_DATA(stQualityPropData,m_DataStorage[DataFileQualityProp]);
	UNLOAD_DATA(stManorResLevelData, m_DataStorage[DataFileManorResLevel]);
	UNLOAD_DATA(stManorTieJiangPuData, m_DataStorage[DataFileManorTieJiangPu]);
	UNLOAD_DATA(stManorWuHunDianData, m_DataStorage[DataFileManorWuHunDian]);	
	UNLOAD_DATA(stManorProtectData, m_DataStorage[DataFileManorProtect]);	
	UNLOAD_DATA(stArenaAwardData, m_DataStorage[DataFileArenaAward]);		
	UNLOAD_DATA(stCheckInData, m_DataStorage[DataFileCheckIn]);	
	UNLOAD_DATA(stOperateActiveData, m_DataStorage[DataFileOperateActive]);
	UNLOAD_DATA(stAccumulatePaymentData, m_DataStorage[DataFileAccumulatePayMent]);
	UNLOAD_DATA(stAccumlateLoginData, m_DataStorage[DataFileAccumlateLogin]);
	UNLOAD_DATA(stPushMentData, m_DataStorage[DataFilePushMent]);
}

const vector<int>* CDataManager::GetGameDataKeyList(int storageID)
{
	if (storageID < 0 || storageID >= DATA_FILE_COUNT)
	{
		return NULL;
	}

	return m_DataStorage[storageID]->GetKeyList();
}

void CDataManager::LoadDownloadListFile( char* pData, int length )
{
	/*CGameData<stDownloadListData, DefaultMkKey>* gamedata = new CGameData<stDownloadListData, DefaultMkKey>;
	IArchive *pArchive = NULL;

	GETARCHIVEFROMDATAPTR_NOSAVE(m_pFramework, pArchive, (BYTE*)pData, length);
	pArchive->SeekTo(0, SEEK_SET);
	gamedata->LoadData(pArchive);
	DestroyArchiveObject(pArchive);
	pArchive = NULL;

	if (NULL != m_DataStorage)
	{
		if (NULL != m_DataStorage[DataFileDownloadList])
		{
			UNLOAD_DATA(stDownloadListData,m_DataStorage[DataFileDownloadList]);
		}

		m_DataStorage[DataFileDownloadList] = gamedata;
	}
	else 
	{
	gamedata->Dispose<stDownloadListData>();
	SAFE_DELETE(gamedata);
	}*/
}

NS_FM_END
