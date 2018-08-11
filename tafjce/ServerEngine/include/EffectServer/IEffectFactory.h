#ifndef __IEFFECTFACTORY_H__
#define __IEFFECTFACTORY_H__

#include "EffectDefine.h"


class ISkillEffect:public IObject
{
public:

	// 功能: 执行效果
	// 参数: [hEntity] 效果执行者。注意:效果如果有给予者，在效果参数里获取
	// 参数: [effectContext] 效果现场参数
	// 参数: [actionInfo] 效果执行结果
	virtual bool doEffect(HEntity hEntity, const EffectContext& effectContext) = 0;

	// 功能: 解析执行效果
	// 参数: [in|strEffectCmd] 效果字符串
	// 参数: [out|context] 输出现场数据
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context) = 0;

	// 功能: 获取效果类型
	virtual int getEffectType() = 0;

	// 功能:获取策划可识别的文本
	virtual vector<string> getTypeString() = 0;

	// 功能: 判断是否免疫状态
	// 参数: [iBuffID] 状态ID
	virtual bool isImMunityBuff(int iBuffID, const EffectContext& effectContext) = 0;
};


#define IID_IEffectFactory	MAKE_RID('e','f','f','t')

class IEffectFactory:public IComponent
{
public:

	// 功能: 解析效果
	// 参数: [strEffectCmd] 效果字符串
	// 参数: [context] 效果现场
	virtual bool parseEffect(const string& strEffectCmd, EffectContext& context) = 0;

	// 功能: 执行效果
	// 参数: [hEntity] 作用实体
	// 参数: [iEffectID] 效果ID
	// 参数: [effectContext] 效果执行现场
	// 参数: [resultList] 执行结果列表
	virtual bool doEffect(HEntity hEntity, int iEffectID, const EffectContext& effectContext) = 0;

	// 功能: 判断是否免疫状态
	// 参数: [iBuffID] 状态ID
	// 参数: [strEffectName] 效果名字
	// 参数: [effectContext] 效果现场
	virtual bool isImMunityBuff(int iBuffID, int iEffectID, const EffectContext& effectContext) = 0;

	// 功能: 判断属性是否下发客户端
	virtual bool isPropNotify(int iPropID, int& iShowPropID) = 0;
};

#endif
