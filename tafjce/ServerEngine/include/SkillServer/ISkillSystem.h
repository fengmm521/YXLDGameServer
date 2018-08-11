#ifndef __ISKILL_SYSTEM_H__
#define __ISKILL_SYSTEM_H__

#include "ISkillFactory.h"


enum EnCheckOperateSkillResult
{
    en_ret_checkOperateSkill_ok,        //对技能的操作检查通过
    
    en_ret_checkAddSkill_notexist,      //欲添加的技能配置不存在
    en_ret_checkAddSkill_hasown,        //欲添加的技能用户己有
    en_ret_checkAddSkill_typeinvalid,   //该类型的技能不能新增加
    en_ret_checkAddSkill_reachmax,      //用户拥有的技能数己超过最大值

    
    en_ret_checkDelSkill_notexist,      //欲删除的技能用户不拥有
    en_ret_checkDelSkill_typeinvalid,   //该类型的技能不能删除
    en_ret_checkDelSkill_selectforbid,  //当前选中的主动技能不能删除
    en_ret_checkDelSkill_needConfirm,   // 需要客户端确认
    en_ret_checkDelSkill_limitis0, 
};


class ISkillSystem:public IEntitySubsystem
{
public:

	// 功能: 使用技能
	// 参数: [targetContext] 目标现场信息
	// 参数: [in|out|actionInfo] 单次攻击结果
	virtual bool useSkill(const vector<HEntity>& memberList) = 0;

	// 功能: 获取当前选择的主动技能
	virtual int getSelectedActiveSkill() = 0;


	// 功能: 使用临时技能
	// 参数: [iSkillID] 临时技能ID
	// 参数: [memberList] 参战成员
	virtual bool useTempSkill(int iSkillID, const vector<HEntity>& memberList, const EffectContext& preContext=EffectContext()) = 0;

	// 功能: 添加临时技能
	// 参数: [iSkillID] 技能ID
	virtual bool addTempSkill(int iSkillID) = 0;


	// 功能: 获取天赋技能列表
	virtual void getTallentSkillList(vector<int>& skillList) = 0;

	// 功能: 获取登场技能
	virtual int getStageSkill() = 0;

	// 功能: 反击
	virtual void backAttack(HEntity hTarget) = 0;

	// 功能: 获取所有技能列表
	virtual vector<ISkill*> getSkillList(bool bOnlyActive) = 0;

	virtual ISkill* getSkill(int iSkillID) = 0;

	//virtual vector<int> getSkillListByType(int iSkillType, bool bOnlyActive) = 0;
};

#endif
