#ifndef __ISKILLFACTORY_H__
#define __ISKILLFACTORY_H__

//#include "IFightFactory.h"
//#include "ISkillSystem.h"

#define IID_ISkillFactory		MAKE_RID('s', 'k', 'f', 't')

#define DEFAULT_SKILLID		1

//#define MAX_BATTLE_MEMBER_SIZE 18


// 目标选择策略
enum
{
	en_SkillTargetSelect_Begin,
	en_SkillTargetSelect_Normal,
	en_SkillTargetSelect_SelfAll,
	en_SkillTargetSelect_TargetAll,
	en_SkillTargetSelect_TargetRow,
	en_SkillTargetSelect_TargetCol,
	en_SkillTargetSelect_TargetLast,     // 目标后排单体
	en_SkillTargetSelect_TargetLastRow, // 后排全体目标
	en_SkillTargetSelect_TargetRand,    // 目标随机一人
	en_SkillTargetSelect_Self,

	en_SkillTargetSelect_TargetAndAround, // 目标极其周围的人
	en_SkillTargetSelect_RandAndAround, // 随机一人及其周围人
	en_SkillTargetSelect_MinLife,  // 敌方生命最低的一人
	en_SkillTargetSelect_MaxLife, // 敌方生命最大的一人

	en_SkillTargetSelect_Female,  // 攻击女性角色

	en_SkillTargetSelect_End,
	

	/*en_SkillTargetSelect_Begin,
	en_SkillTargetSelect_Self = 1,		// 自身
	en_SkillTargetSelect_Target = 2,		// 目标敌人(一个人)

	// 下面的暂未实现
	en_SkillTargetSelect_TargetAll = 3, // 目标全体
	en_SkillTargetSelect_SelfMinHP = 4, // 我放生命最少一人
	en_SkillTargetSelect_LastTarget = 5, //目标最后一人
	en_SkillTargetSelect_TargetLine = 6, // 纵向
	en_SkillTargetSelect_TargetRow = 7,  // 横向
	en_SkillTargetSelect_SelfRand = 8,  // 我方随机XX人(可带参数)
	en_SkillTargetSelect_SelfAll = 9,   // 我方全体
	en_SkillTargetSelect_ColExceptTarget = 10, // 纵向除了目标外
	en_SkillTargetSelect_BehindTarget = 11,  // 打目标身后一人
	en_skillTargetSelect_SelfHPLostMax = 12, // 我方HP损失最多的XX人(可带参数)
	
	en_skillTargetSelect_TargetSmallStar = 13, // 目标小十字
	en_skillTargetSelect_TargetBigStar = 14, // 目标大十字
	
	en_skillTargetSelect_TargetFrontRow = 15, // 目标前排
	en_skillTargetSelect_TargetMiddleRow = 16, // 目标中排
	en_skillTargetSelect_TargetBackRow = 17, // 目标后排

	en_skillTargetSelect_SelfFrontRow = 18, // 我方前排
	en_skillTargetSelect_SelfMiddleRow = 19, // 我方中排
	en_skillTargetSelect_SelfBackRow = 20, // 我方后排

	en_skillTargetSelect_SelfRow = 21, // 自身所在排
	en_skillTargetSelect_SelfCol = 22, // 自身所在列
	en_skillTargetSelect_SelfSmallStar = 23, // 自身小十字
	en_skillTargetSelect_SelfBigStar = 24, // 自身大十字
	en_skillTargetSelect_TargetRand = 25, // 目标随机XX人(可带参数)

	en_skillTargetSelect_SelfMinHP = 26, // 我方生命最少的XX人(可带参数)
	en_skillTargetSelect_TargetMinHP = 27, // 对方生命最少的XX人(可带参数)
	
	en_skillTargetSelect_SelfMaxHP = 28, // 我方生命最多的XX人(可带参数)
	en_skillTargetSelect_TargetMaxHP = 29, // 对方生命最多的XX人(可带参数)

	en_skillTargetSelect_SelfMaxAnger = 30, // 我方怒气最多的XX人(可带参数)
	en_skillTargetSelect_TargetMaxAnger = 31, // 对方怒气最多的XX人(可带参数)

	en_skillTargetSelect_TargetMaxAttFightValue = 32, // 敌方攻击战力最高的XX人
	en_skillTargetSelect_TargetMaxDefFightValue = 33, // 敌方防御战力最高的XX人
	en_skillTargetSelect_TargetMaxFightValue = 34, // 敌方综合战力最高的XX人

	en_skillTargetSelect_SelfMaxAttFightValue = 35, // 我方攻击战力最高的XX人
	en_skillTargetSelect_SelfMaxDefFightValue = 36, // 我方防御战力最高的XX人
	en_skillTargetSelect_SelfMaxFightValue = 37, // 我方综合战力最高的XX人

	en_skillTargetSelect_TargetHPLostMax = 38,  // 目标HP损失最多的XX人(可带参数)
	en_skillTargetSelect_SelfLostMaxHPSmallStar= 39, // 己方损血最多目标为中心的小十字
	en_skillTargetSelect_TargetLostMaxHPSmallStar = 40, // 敌方损血最多目标为中心的小十字
	en_skillTargetSelect_BuffTarget = 41,  // 状态的目标
	
	en_SkillTargetSelect_End,*/
};



// 技能作用类型
/*enum
{
	en_SkillResultType_Begin,
	en_SkillResultType_Damage,
	en_SkillResultType_Heal,
};*/

// 技能近战目标(仅仅用于表现)
enum
{
	en_MeleeTarget_NormalTarget = 1,
	en_MeleeTarget_LastTarget,
};

// 技能伤害类型
enum
{
	en_SkillDamageType_Begin,
	en_SkillDamageType_Phy,
	en_SkillDamageType_Mgc,
};

enum
{
	en_entityWuxing_Begin,
	en_entityWuxing_Metal, // 金
	en_entityWuxing_Wood,  // 木
	en_entityWuxing_Water, // 水
	en_entityWuxing_Fire,  // 火
	en_entityWuxing_Earth, // 土
};




// 特殊技能命中
enum
{
	en_SpecialHitResult_Begin,
	en_SpecialHitResult_Punch, // 合击
	en_SpecialHitResult_Help,  // 援护
	en_SpecialHitResult_CounterAtter, // 反击
	en_SpecialHitResult_End,
};

enum
{
    //与协议内容一致
	en_SkillType_ComAttack,	// 普攻
	en_SkillType_Active,      // 主动技能
	en_SkillType_Talent,      // 天赋
	en_SkillType_Stage,       // 登场技能
	en_SkillType_SmallActive, // 小技能
};

class ISkillSystem;
class ISkill:public IObject
{
public:

	// 功能: 技能初始化
	// 参数: [pSkillSystem] 技能子系统
	virtual bool init(ISkillSystem* pSkillSystem) = 0;

	// 功能: 判断是否可以使用技能
	// 参数: [targetItem] 目标
	// 参数: [iSkillExp] 技能熟练度
	virtual bool canUseSkill(const vector<HEntity>& targetItem) = 0;

	// 功能: 使用技能
	// 参数: [targetItem] 目标信息
	// 参数: [actionInfo] 该回合结果信息
	virtual bool useSkill(const vector<HEntity>& targetItem, const EffectContext& preContext = EffectContext() ) = 0;

	// 功能: 获取技能类型
	virtual int getSkillType() const = 0;

	// 功能: 获取技能ID
	virtual int getSkillID() const = 0;

	// 功能: 获取技能组ID
	virtual int getSkillGroupID() const = 0;
	
	// 功能: 获取当前等级
	virtual int getSkillLevel() const = 0;
	
	// 功能: 获取最大等级
	virtual int getMaxLevel() const = 0;
    
	// 功能: 克隆对象
	virtual ISkill* clone() const = 0;

	// 功能: 打包技能数据
	virtual void packData(int& skillData) = 0;

	// 功能: 是否激活
	virtual bool isActive() = 0;

	// 功能: 是否连击技能
	virtual bool isBattleSkill() const = 0;

	// 功能: 是否可以被反击
	virtual bool canBackAttack() const = 0;
};

#define MAKE_SKILLID(id, lv)		( ( ( (Int64)id)<<32)|lv)


class ISkillFactory:public IComponent
{
public:

	// 功能:获取技能原型
	virtual const ISkill* getSkillPrototype(int nSillID) = 0;

	// 功能: 计算伤害命中结果
	// 参数: [hAttacker] 攻击者
	// 参数: [hTarget] 目标
	// 参数: [iDamageType] 伤害类型
	virtual int calcDamageHitResult(HEntity hAttacker, HEntity hTarget, int iDamageType) = 0;

	// 功能: 目标选择，主要是效果系统需要使用这部分内容
	// 参数: [hAttacker] 攻击者
	// 参数: [iSelectStrategy] 目标选择策略
	// 参数: [resultList] 最终目标
	// 参数: [memberList] 战斗队伍
	virtual void selectTarget(HEntity hAttacker ,int iSelectStrategy, vector<HEntity>& resultList, const vector<HEntity>& memberList) = 0;


	// 功能: 获取技能类型
	// 参数: [iSkillID] 技能ID
	virtual int getSkillType(int iSkillID) = 0;


	// 功能: 根据技能系列和等级获取技能ID
	// 参数: [iSkillGrpID] 技能系列ID
	// 参数: [iLevel] 等级
	virtual int getSkillID(int iSkillGrpID, int iLevel) = 0;
};

#endif
