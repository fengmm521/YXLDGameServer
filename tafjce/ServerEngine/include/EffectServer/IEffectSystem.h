#ifndef __IEFFECT_SYSTEM_H__
#define __IEFFECT_SYSTEM_H__


#define IID_IEffectSystem		MAKE_RID('e','f','s','t')

class IEffectSystem:public IEntitySubsystem
{
public:

	// 功能: 执行效果
	// 参数: [effect] 效果现场信息
	virtual bool addEffect(EffectContext& effect) = 0;

	// 功能: 执行效果
	// 参数: [iEffectID] 效果ID
	// 参数: [context]效果所需现场
	virtual bool addEffect(int iEffectID, const EffectContext& context) = 0;
};


#endif
