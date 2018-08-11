/********************************************************************
created:	2012-12-07
author:		pelog (唐超)
summary:	客户端的玩家
*********************************************************************/
#pragma once
#include "FmEntity.h"
#include "NetWork/GameServer.pb.h"
#include "FmBagItem.h"
#include "FmGodAnimal.h"
#include "HeroSoul/FmHeroSoul.h"
#include "FmPlatform.h"


NS_FM_BEGIN
class FightSoulBag;
class Hero;
class EquipBag;

class Player : public Entity
{
public:
	class PlayerCreateOpt : public EntityCreateOpt
	{
	public:
		bool m_IsLocalPlayer;

		PlayerCreateOpt()
		{
			m_IsLocalPlayer = false;
		}
	};
protected:
	bool	m_IsLocalPlayer;	// 是否是本机玩家
	int	m_PreDupScene;		// 用来标记前一个场景是类型

	vector<uint> m_HeroList;
	vector<uint> m_FormationList; //0表示无英雄
	vector<BagItem*> m_playerBagItems;//玩家背包容器

	vector<GodAnimal*> m_GodAnimalList;		//神兽列表

	int	m_fightValue;
	int	m_formationLimit;
	map<uint,uint> m_tempPlayerBagItems;

protected:
	Player( uint8 entityType, uint entityId, const string& name, bool isLocalPlayer );
public:
	virtual ~Player();

	// 创建接口
	static Entity* Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt );

	// 注册对象创建接口
	static void InitInterface();

	// 本机玩家标记
	bool IsLocalPlayer() const { return m_IsLocalPlayer; }
	void SetLocalPlayer( bool isLocalPlayer ) { m_IsLocalPlayer=isLocalPlayer; }

	// 更新
	virtual void Update( uint delta );

	// 任务组件
	//Quest* GetQuest();

	// 背包
	//PlayerBag* GetBags();
	FightSoulBag* GetFightSoulBags();

	// 移动到场景中某个entity的位置
	//void MoveToTargerEntity( SceneEntity* targetEntity );

	// 获取玩家英雄
	Hero* GetPlayerHero();

	uint GetLevel();

	vector<uint>& GetHeroList(bool isfresh = true);
	void GetCanSummonHeroList(vector<uint>& cansummomHeroList);
	void GetNotSummonHeroList(vector<uint>& notcansummomHeroList);
	void GetCanSummonHeroAndHeroList(vector<uint>& summondHeroList);
	vector<uint>& GetFormationList(){
		return m_FormationList;
	}
	Bag* GetPlayerBag();
	vector<GodAnimal*>& GetGodAnimalList() { 
		return m_GodAnimalList;
	}
	vector<GodAnimal*> GetCanInheritGodAnimal(GodAnimal* animal);
	GodAnimal* GetGodAnimalByDwObject(int dwobject);
	//设定出战神兽，出战神兽默认放在第一个位置
	void SetGodAnimalActive(int dwObjctId);
	//获取当前未获得的神兽
	void GetPlayerNotHave(vector<int>& nothavegodanimal);
	//获得所有有神兽魂魄的神兽
	void GetHaveSoulGodAnimal(vector<int>& havesoulgodanimal);
	//获取英雄未出征列表，英雄会换
	void GetNotExpeditionHeroList(vector<uint>& notExpeditionHeroList);
	//获得英雄未出征列表（倒序），英雄出售
	void GetNotExpeditionHeroListByHeroSell(vector<uint>& notExpeditionHeroList);
	void GetHeroAscendingOrderList(vector<uint>& HeroAscendingOrderLis,uint entityId,uint objId);
	void RemoveHeroFromList(uint entityId);
	bool IsInFormation(Hero* hero);
	int GetHeroFormationPos(Hero* hero);

	//获得出征英雄
	void GetFormationHeroList(vector<uint>& FormationHeroList);

	// 英雄批量出售
	void EraseHerolistInHeroBatchSell(vector<uint>& heroBatchSellList,uint ObjectId);
	void AddHerolistInHeroBatchSell(vector<uint>& heroBatchSellList,uint ObjectId);

	//根据heroid 是否和出征列表中的英雄一样 
	bool IsSameAsInforMationHero(const uint HeroId);

	Hero* GetHeroByObjId(uint objId);
	int GetHeroIndexByObjId(uint objId);

	int GetHeroInFormationCount();
	void InitFormationList();
	static void HeroUpdate( int iCmd, GSProto::SCMessage& pkg );
	static void HeroDel( int iCmd, GSProto::SCMessage& pkg );
	static void HeroBatchDel(int iCmd,GSProto::SCMessage& pkg );
	static void FSContainerChg( int iCmd, GSProto::SCMessage& pkg );

	static void GodAnimalUpdate(int iCmd, GSProto::SCMessage& pkg);
	static void EquipContainerChg( int iCmd, GSProto::SCMessage& pkg );

	static void EquipQuery( int iCmd, GSProto::SCMessage& pkg );
	static void HeroSoulChg(int iCmd,GSProto::SCMessage& pkg);
	static void HeroTallenSkillChg(int iCmd,GSProto::SCMessage& pkg);
	//bool PointIsInRect(FmPoint point, FmRect rect);
	
	// 设置新名字
	//void SetName( const char* name );
	void HerolistSort();
	EquipBag* GetEquipBags();
	Bag* GetPlayerMaterail();
	//判断当前神兽是否获得了 
	bool GodAnimalisHaveByAnimalId(const int godanimalId);

	//神兽排序
	void sortGodAnimalWithId(vector<int>& AllgodanimalId);

	const uint GetGodAnimalWithId(const int godanimalId);
	GodAnimal* GetGodAnimalByAnimalId(const int godanimalId);

	//魂魄
	HeroSoulBag* GetPlayerHeroSoul();
	//获得当前魂魄的个数
	int GetPlayerHeroSoulCountById(const int herosoulID);
	//判断服务器更新的是增加还是减少 这里的魂魄一定是玩家身上的
	bool PlayerHeroSoulIsAdd(const int herosoulID,const int count);
	//判断服务器更新的是玩家有的还是没有
	bool PlayerHeroSoulIsHave(const int herosoulId);

	//判断当前英雄是否可以召唤
	bool PlayerHeroCanSummon(int heroId);
	void GetPlayerCanSummonHeroList(vector<uint> &canSummonHeroList);
	bool PlayerHeroCannotSummon(int heroId);

	void OperatorHeroSoulByShoulID(const int herosoulID,const int count);

	//判断英雄是否有喜好品
	bool PlayerHeroHaveFavorite();
	//拆分喜好品字符串
	void GetHeroQualityFavoriteItemId(std::string src,vector<int >& itemId);
	//检查是否有喜好品
	bool PlayerHaveFavorite(const int heroObjectId , const int itemID);
	//获得英雄喜好品itemID
	void GetPlayerFavoriteItemID(vector<int >& itemID,Hero *hero);
	//检查是否已经装备喜好品
	bool PlayerHaveEquipFavorite(const int heroObjectId,const int itemId);
	//获得英雄喜好品string
	string GetPlayerHeroFavorite(const int quality,const int heroId);
	//判断英雄所装备的喜好品是几号位子
	int GetPlayerHeroFavoritePosition(const int herofavoriteId,const int heroObjectId);
	int GetPlayerHeroFavoritePositionByHero(const int herofavoriteId,Hero*hero);
	//迭代查找喜好品是否满足要求
	bool PlayerHeroFavoriteCanAdmix(const int herofavoriteId);
	//检查喜好品
	void PlayerHeroFavoriteCheck();

	//临时背包信息初始化
	void InitTempPlayerBagItemWithFavorite();
	//检查当前临时背包的个数
	int CheckTempPlayerBagItemCount(const int itemId);
	//更改临时背包
	void ChangeTempPlayerBagItem(const int itemId,const int number);
	//检查单个的喜好品
	void PlayerHeroFavoriteCheckByObjectId(const int heroObjectId);
	//装备喜好品
	void EquipHeroFavorite(const int herofavoriteId,Hero *hero,const int pos);
	void EquipHeroFavoriteByHeroObject(const int herofavoriteId,const int heroObjectId,const int pos);
	//判断英雄是否喜好品都穿好了
	bool HeroHaveAllEquipFavorite(const int heroObjectId);
	//更新hero 喜好品
	void UpdateHeroFavoriteByHeroObjectId(const int heroObjectId);

	//获得喜好品合成所需材料Item
	void GetHeroFavoriteSummondItemId(const int herofavoriteId,vector<int> &favoriteId,vector<int > &favoriteMaterialNumber);
	//判断喜好品是否可以合成
	bool HeroFavoriteCanSummondByFavoriteId(const int heroFavoriteId);
	//获得所缺的物品
	int HeroFavoriteNotHave_ByCanSummondByFavoriteId(const int heroFavoriteId);
	//判断喜好品是否为基础喜好品
	bool HeroFavoriteIsBaseFavorite(const int heroFavoriteId);
	//得到喜好品的属性
	void GetHeroFavoriteNatureByItemId(vector<string>& favoriteNature,const int itemId);
	

	//天赋技能
	void UpdateHeroTallentSkillData(const uint32 heroObjId, const int32 tallenSillID,const int tallSkilllevel,const bool tallSkillCanup);
	bool CheckHeroTallentCanUpOrAct();

	//获取时间 
	long getCurrentTime();
	//获得时间的分钟
	int getCurrentMinTime();
	//判断当前时间是否在指定时间段中
	bool IsInCurTime(long timeBegin,long timeEnd);

   // 获取月份
	int getMoth();
	//
	int getDelayTime(const int hour , const int min);
};

NS_FM_END