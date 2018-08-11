#ifndef __ITEM_SYSTEM_H__
#define __ITEM_SYSTEM_H__

#include "ITimeRangeValue.h"
#include "ConvertCode.h"


class PhyStrengthResume:public ITimeRangeTask
{
public:

	PhyStrengthResume(){};
	void init(HEntity hEntity){m_hEntity = hEntity;}
	virtual TimeRangeExecute getExecuteDelegate();
	virtual TimeRangeCheck getCheckDelegate();
	virtual int checkInteval(){return 10*1000;}

	bool checkResume(Uint32 dwTime);
	void doResume(Uint32 dwTime);

private:

	HEntity m_hEntity;
};

class ItemSystem:public ObjectBase<IItemSystem>, public Detail::EventHandle
{
public:

	ItemSystem();
	~ItemSystem();

	// IEntitySubsystem interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	
	// ItemSystem Interface
	virtual bool addItem(int iItemID, int iCount, int iReason);
	virtual bool removeItem(int iItemID, int iItemCount, int iReason);
	virtual int getItemCount(int iItemID);
	virtual bool buyPhyStength();
	virtual void onReqBuyPhyStength();
	
	void onEventSend2Client(EventArgs& args);
	void onEventLevelUp(EventArgs& args);
	
	void onItemUseMsg(const GSProto::CSMessage& msg);
	void onChatMsg(const GSProto::CSMessage& msg);
	void onWhisperMsg(const GSProto::CSMessage& msg);
	void onReqBuyPhyStrength(const GSProto::CSMessage& msg);
	void onReqQueryActorInfo(const GSProto::CSMessage& msg);
	void onReqQuerySaveMoney(const GSProto::CSMessage& msg);
	void onGetTBTUUID(const GSProto::CSMessage& msg);
	
	
	int getBuyPhyCountLimit(int iVIPLevel);
	bool getBuyCostGold(int iCount, int& iCostGold);
	int getBagType(int iItemType);

	void onReqSellItem(const GSProto::CSMessage& msg);
	void onChgActorHead(const GSProto::CSMessage& msg);

	void onQueryChannel(const GSProto::CSMessage& msg);

	void onSendConvertCode(const GSProto::CSMessage& msg);
	//virtual void onConvertDrop(string strConvertCode, string ConvertCode);
	virtual void onConvertGift(string strConvertCode, string ConvertCode);
	virtual void GMOnlineGetBagData(string& strJson);
	
	
private:

	void initItemData(const ServerEngine::ItemSystemData& itemData);
	bool removeItemByHandle(HItem hItem, int iReason);
	void checkChatReset();

private:

	HEntity m_hEntity;
	vector<HItem> m_itemList;
	ServerEngine::TimeResetValue m_buyPhyStrengthCount;
	PhyStrengthResume m_phyStrengthResume;
	ServerEngine::TimeResetValue chatResetVal;
	map<string,bool> m_ConvertMap;
};


#endif
