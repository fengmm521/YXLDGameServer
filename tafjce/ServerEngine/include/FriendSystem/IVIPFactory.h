#ifndef IVIPFACTORY_H_
#define IVIPFACTORY_H_


enum VIPProp
{
	VIP_PROP_SPEEDPRODUCT_TIMES = 0, // 加速生产的次数                 
	VIP_PROP_PHYSTRENGTH_UPLIMIT = 1, //体力上限
	VIP_PROP_BUY_PHYSTRENGTH_TIMES = 2, //能购买体力的次数
	VIP_PROP_BUY_CHALLENGE_TIMES = 3, //能购买挑战的次数
	VIP_PROP_ADVANCED_PRACTICE = 4,   //高级修炼次数
	VIP_PROP_GIVEFRIEND_PHYSTRENGTH_TIEMS = 5, //赠送好友体力次数
	VIP_PROP_GETFRIEND_PHYSTRENGTH_TIMES= 6, //接受好友体力次数
	VIP_PROP_FRIENDCOUNT_LIMIT = 7, //好友上限
	VIP_PROP_CLIMBTOWER_TIMES = 8,  //通天塔次数

	VIP_PROP_VIPEXP = 9,

	VIP_PROP_SAODANG_TIMES = 10,  // 扫荡次数限制

	VIP_PROP_DREAMLAND_RESET = 11,	//九天幻境重置次数

	VIP_PROP_MOBAI_TIMES = 12,

	VIP_PROP_MANOR_HARVEST_TIMES = 13,	//领地收取次数

	VIP_PROP_FB_ADDEXP = 14,
	
	VIP_PROP_COUNT = 15 ,				//属性个数
};

class IVIPFactory:public IComponent
{
public:
	virtual int getVipPropByHEntity(HEntity hEntity,unsigned int propKey) = 0;
	virtual int getVipPropByVIPLevel(int vipLevel, unsigned int propKey) = 0;
	virtual int getVipTopLevel() = 0;
};

#endif
