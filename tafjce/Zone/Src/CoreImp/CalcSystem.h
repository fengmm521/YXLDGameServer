#ifndef __CALC_SYSTEM_H__
#define __CALC_SYSTEM_H__

#include "IEntity.h"


class CalcSystem;

// 属性计算绑定类接口
class PropCalcHelperBase
{
public:
	virtual  ~PropCalcHelperBase(){}
	virtual void doPropCalc(CalcSystem* pCalcPropSystem, const EventArgs& args) = 0;
};


class CommonPropCalc:public PropCalcHelperBase
{
public:

	CommonPropCalc(int iTargetPropID, int iBasePropID, int iAddVPropID, int iAddPercentPropID, int iGrowPropID)
		:m_iTargetPropID(iTargetPropID), m_iBasePropID(iBasePropID), m_iAddVPropID(iAddVPropID),
		m_iAddPercentPropID(iAddPercentPropID), m_iGrowPropID(iGrowPropID){}

	void doPropCalc(CalcSystem* pCalcPropSystem, const EventArgs& args);

private:

	int m_iTargetPropID;
	int m_iBasePropID;
	int m_iAddVPropID;
	int m_iAddPercentPropID;
	int m_iGrowPropID;
};

class FightValuePropCalc:public PropCalcHelperBase
{
public:

	void doPropCalc(CalcSystem* pCalcPropSystem, const EventArgs& args);
};

class VIPPropCalc:public PropCalcHelperBase
{
public:

	VIPPropCalc(int iPropID, int iVIPPropID):m_iPropID(iPropID), m_iVIPPropID(iVIPPropID){}

	void doPropCalc(CalcSystem* pCalcPropSystem, const EventArgs& args);

private:

	int m_iPropID;
	int m_iVIPPropID;
};

class CalcSystem:public ObjectBase<IEntitySubsystem>, public Detail::EventHandle
{
public:

	CalcSystem();
	~CalcSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage){}
	virtual void packSaveData(string& data){}
	void onEventPropChg(EventArgs& args);

	void calcAllProperty();

	static void bindCommPropCalc(int iTargetPropID, int iBasePropID, int iAddValuePropID, int iAddPercentPropID, int iGrowPropID);
	static void bindFightValueCalc();
	static void bindPropInfo();
	static void bindVIPProp();
	

private:

	HEntity m_hEntity;
};


#endif

