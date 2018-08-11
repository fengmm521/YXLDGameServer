#ifndef __ENTITY_EVENT_DEF_H__
#define __ENTITY_EVENT_DEF_H__

enum
{
	EVENT_ENTITY_BEGIN,
	EVENT_ENTITY_PROPCHANGE,			// ÊôÐÔ¸Ä±äÊÂ¼þ
	EVENT_ENTITY_PRE_RELEASE,  // Ô¤Ïú»ÙÊÂ¼þ
	EVENT_ENTITY_PREBEGIN_FIGHT,
	EVENT_ENTITY_BEGIN_FIGHT,		// ¿ªÊ¼Õ½¶·
	EVENT_ENTITY_END_FIGHT,			// ½áÊøÕ½¶·
	EVENT_ENTITY_BEGIN_ROUND,		// ¿ªÊ¼»ØºÏ
	EVENT_ENTITY_END_ROUND,		// ÍêÕû»ØºÏ½áÊø
	EVENT_ENTITY_USESKILL,
	EVENT_ENTITY_USESKILL_FINISH,
	EVENT_ENTITY_PRE_ATTACK,			// ¹¥»÷Ç°
	EVENT_ENTITY_POST_ATTACK,
	EVENT_ENTITY_DEAD_WITHCONTEXT,
	EVENT_ENTITY_SENDACTOR_TOCLIENT, // Í¨Öª¿Í»§¶Ë½ÇÉ«ÐÅÏ¢ÊÂ¼þ
	EVENT_ENTITY_SENDACTOR_TOCLIENT_POST,
	EVENT_ENTITY_NEWHERO,   // ÐÂÔöÓ¢ÐÛ
	EVENT_ENTITY_DELHERO,   // É¾³ýÓ¢ÐÛ
	EVENT_ENTITY_RELOGIN,   // ÖØµÇÂ¼

	EVENT_ENTITY_CREATED,   // ËùÓÐÊµÌå´´½¨Íê³Éºó¹«ÓÃÊÂ¼þ
	EVENT_ENTITY_DODAMAGE,
	
	EVENT_ENTITY_BEDAMAGE,
	
	EVENT_ENTITY_PREDODAMAGE,
	EVENT_ENTITY_PREBEDAMAGE,
	EVENT_ENTITY_POSTDODAMAGE,
	EVENT_ENTITY_POSTBEDAMAGE,
	
	EVENT_ENTITY_LEVELUP,
	EVENT_ENTITY_POSTLEVELUP,
	
	EVENT_ENTITY_LEVELSTEPCHG, // µÈ½×Éý¼¶,±ä»¯
	EVENT_ENTITY_CALCGROW, // Í¨ÖªÊµÌåÖØÐÂ¼ÆËã³É³¤

	EVENT_ENTITY_DOFIGHTPROPCHG, // Õ½¶·ÖÐÊôÐÔ±ä»¯Result
	EVENT_ENTITY_POSTDOFIGHTPROPCHG,

	EVENT_ENTITY_BEFIGHTPROPCHG,
	EVENT_ENTITY_POSTBEFIGHTPROPCHG,

	EVENT_ENTITY_EFFECT_TOCHDOKILL,  // ´¥Åöµ½»÷É±
	EVENT_ENTITY_EFFECT_TOCHBEKILL,
	
	EVENT_ENTITY_EFFECTDOKILL,
	EVENT_ENTITY_EFFECTBEKILL,

	EVENT_ENTITY_CONTINUESKILL,  // Á¬Ðø¼¼ÄÜ¿ªÊ¼
	EVENT_ENTITY_RELIVE,  // ¸´»î

	EVENT_ENTITY_FIGHTVALUECHG, // Ó¢ÐÛÕ½Á¦¸Ä±ä

	EVENT_ENTITY_NEWBUFF,
	EVENT_ENTITY_RELEASEBUFF,

	EVENT_ENTITY_PASSSCENE,  // Í¨¹Ø¹Ø¿¨

	EVENT_ENTITY_FINISGUIDE,

	EVENT_ENTITY_OPENFUNCTION,

	EVENT_ENTITY_PERFECT_PASSSECTION, // Í¨¹ØÕÂ½Ú

	EVENT_ENTITY_FINISHCONDITION, //Íê³ÉÀÛ»ýÈÎÎñÌõ¼þ

	EVENT_ENTITY_FINISHDUNGEONCONDITION,   //¸±±¾Íê³ÉÌõ¼þ
	
	EVENT_ENTITY_ACTIVEGODANIMAL_CHG, // ³öÕ½ÉñÊÞ±ä»¯

	EVENT_ENTITY_PREEXE_EFFECT,						

	EVENT_ENTITY_TASKFINISH_LOOT,					//ÂÓ¶á´ÎÊý

	EVENT_ENTITY_TASKFINISH_HEROLEVEL,				//Ó¢ÐÛµÈ¼¶

	EVENT_ENTITY_TASKFINISH_SOUL,					//Îä»êµÈ¼¶

	EVENT_ENTITY_TASKFINISH_GODANIMLLEVEL,			//sÉñÊÞµÈ¼¶
	
	EVENT_ENTITY_TASKFINISH_CLIMBTOWER,				//ÅÀËþ

	EVENT_ENTITY_TASKFINISH_GOLD,					//½ð¿ó¼ÓËÙ´ÎÊý
	
	EVENT_ENTITY_TASKFINISH_BATTLEANYDUNGEON,        //ÌôÕ½ÈÎÒâ¸±±¾Íê³É

	EVENT_ENTITY_TASKFINISH_WORLDBOSS,               //²Î¼ÓÊÀ½çBOSSÍê³É
	
	EVENT_ENTITY_TASKFINISH_JOINCAMPBATTLE,			//²Î¼ÓÕóÓªÕ½Íê³É

	EVENT_ENTITY_TASKFINISH_GETHERO,               //ÕÐÄ¼Ó¢ÐÛÍê³É

	EVENT_ENTITY_TASKFINISH_GODANIMALTRAIN,			//ÉñÊÞÅàÑø

	EVENT_ENTITY_TASKFINISH_FIGHTSOULTRAIN,				//Îä»êÅàÑø

	EVENT_ENTITY_TASK_HAVEREWARD,					//ÈÎÎñÍê³ÉÁì½±

	EVENT_ENTITY_TASK_HEROLEVELUP,					//Ó¢ÐÛÉý¼¶

	EVENT_ENTITY_TASK_PLAYGIRL,						//µ÷Ï·´ÎÊý 

	EVENT_ENTITY_TAKS_SOULQUALITY,					//Îä»êÆ·ÖÊ

	EVENT_ENTITY_TASK_FightGoldMonster,  				//»÷É±Ôª±¦¹Ö

	EVENT_ENTITY_TASK_FightHeroExpMonster,				//»÷É±ÐÞÎª¹Ö

	EVENT_ENTITY_TASK_FightHeroMonster,					//»÷É±Ó¢ÐÛ¹Ö

	EVENT_ENTITY_ITEMCHG, 

	EVENT_ENTITY_TASK_HEROSKILL_LEVELUP,				//¼¼ÄÜÉý¼¶	

	EVENT_ENTITY_TASK_CRUSH_DREAMLAND_SCENE,			//Í¨¹ý¾ÅÌì»Ã¾³¹Ø¿¨

	EVENT_ENTITY_ZIBAO, // ×Ô±¬

	EVENT_ENTITY_TASK_SILVER_RES_LEVELUP,			//Í­¿óÉý¼¶

	EVENT_ENTITY_TASK_HEROEXP_RES_LEVELUP,			//ÐÞÎª¿óÉý¼¶

	EVENT_ENTITY_PASSSCENE_COUNT,   					//Í¨¹Ø¹Ø¿¨Êý

	EVENT_ENTITY_YAOQIANSHU_USETIMES,						//Ò¡Ç®Ê÷Ê¹ÓÃ´ÎÊý

	EVENT_ENTITY_TASK_MANOR_HARVERST,					//ÁìµØÊÕ»ñ¡

	EVENT_ENTITY_TASK_MANOR_WuHunJiLian,					//Îä»ê¼ÀÁ¶Ôì

	EVENT_ENTITY_TASK_MANOR_ITEMDaZao,					//ÎïÆ·´òÔì

	EVENT_ENTITY_TASK_GIVE_Strength,						//ÔùËÍÌåÁ¦

	EVENT_ENTITY_ADDHEROSOUL_FROMHERO,   // Ìí¼ÓÓ¢ÐÛ»ñµÃÎä»êÊÂ¼þÍ¨Öª

	EVENT_ENTITY_TASK_ARENA_BATTLE,		//²Î¼Ó¾º¼¼³¡

	EVENT_ENTITY_TASK_HEROSOUL_LEVEL,	//Îä»êµîµÈ¼¶

	EVENT_ENTITY_TASK_TIEJIANGPU_LEVEL, 	//Ìú½³ÆÌµÈ¼¶

	EVENT_ENTITY_TASK_EQUIPLOVEQUIP,		//×°±¸Ï²ºÃÆ·

	EVENT_ENTITY_TASK_HEROQUALITY,		//Ó¢ÐÛÆ·ÖÊ

	EVENT_ENTITY_TASK_FORMATION,			//ÉÏÕó

	EVENT_ENTITY_SAVEMONEY, // ³äÖµ³É¹¦
};

typedef PropertySet EffectContext;


struct EventArgs_FightPropChg
{
	EventArgs_FightPropChg():iPropID(0), iChgValue(0), iValue(0), bReboundDamage(false), effectCtx(NULL), bRelive(false), bFilledResult(false){}

	HEntity hEventTrigger;
	HEntity hEntity;
	HEntity hGiver;
	int iPropID;
	int iChgValue;
	int iValue;
	bool bReboundDamage; // ÊÇ·ñ·´µ¯ÉËº¦
	const EffectContext* effectCtx;
	bool bRelive;
	bool bFilledResult; // ÊÇ·ñÒÑ¾­Ìî³äÁËResultµ½Õ½±¨Àï£¬·ÀÖ¹ÖØ¸´Ìî³ä
};


struct EventArgs_PropChange
{
	EventArgs_PropChange():iPropID(0), iType(0), iOldValue(0), 
		fOldValue(0.0), i64OldValue(0),iValue(0), fValue(0.0), i64Value(0){}
	
	HEntity hEntity;
	int iPropID;
	int iType;

	int iOldValue;
	float fOldValue;
	string strOldValue;
	Int64 i64OldValue;

	int iValue;
	float fValue;
	string strValue;
	Int64 i64Value;
};



struct EventArgsDamageCtx
{
	EventArgsDamageCtx():iHitResult(0), iResultDamage(0), effectCtx(NULL){}

	HEntity hEntity;
	HEntity hGiver;
	int iHitResult;
	int iResultDamage;
	const EffectContext* effectCtx;
};

// Ð§¹ûÏµÍ³´¥·¢
/*struct EventArgs_SkillResult
{
	EventArgs_SkillResult():bHitResult(0),bResultType(0),iSkillID(0), pResult(NULL), bEquipTriggerSkill(false), iVamPireHP(0){}
	HEntity hGiver;
	HEntity hEntity;
	Uint8 bHitResult;
	Uint8 bResultType;
	HEntity hNormalTarget;
	int iSkillID;
	HEntity hEventTigger;  // ´¥·¢Õß¾ä±ú,ÃèÊöÊÂ¼þÔÚË­ÉíÉÏ´¥·¢
	void* pResult;
	bool bEquipTriggerSkill;
	int iVamPireHP;
	vector<HEntity> vamPireList;
};

struct EventArgs_Action
{
	EventArgs_Action():cActionType(0),pActionBuff(0) {}
	HEntity hGiver;
	HEntity hEntity;
	Uint8 cActionType;
	void* pActionBuff;
};

// ÏµÍ³/ÕæÊµµÄ³öÏÖÏûÊ§
struct EventArgs_Appear
{
	HEntity hEntity;
	HEntity hAppearEntity;
};

struct EventArgs_DissAppear
{
	HEntity hEntity;
	HEntity hDisAppearEntity;
};

struct EventArgs_Move
{
	HEntity hMoveEntity;
	void* pMoveContext;
};

struct EventArgs_ViewState
{
	HEntity hEntity;
	bool bIsFull;
};

struct EventArgs_EffectPropChange
{
	EventArgs_EffectPropChange():iPropID(0), iOrgPropID(0), iOrgDeltaValue(0), pEffectContext(NULL){}
	
	HEntity hEntity;
	int iPropID;
	int iOrgPropID;
	int iOrgDeltaValue;
	const PropertySet* pEffectContext;
};*/

#endif
