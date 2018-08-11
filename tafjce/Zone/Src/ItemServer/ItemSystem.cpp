#include "ItemServerPch.h"
#include "ItemSystem.h"
#include "ErrorCodeDef.h"
#include "IJZMessageLayer.h"
#include "ILegionFactory.h"
#include "LogHelp.h"
#include "IVIPFactory.h"
#include "IJZEntityFactory.h"
#include "IGodAnimalSystem.h"

extern "C" IObject* createItemSystem()
{
	return new ItemSystem;
}

extern int LifeAtt2Prop(int iLifeAtt);

#define ConvertCodeLength 10
ItemSystem::ItemSystem()
{
}

ItemSystem::~ItemSystem()
{
	for(size_t i = 0; i < m_itemList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_itemList[i]);
		if(pItem) delete pItem;
	}

	m_itemList.clear();

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &ItemSystem::onEventSend2Client);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_POSTLEVELUP, this, &ItemSystem::onEventLevelUp);
	}
}

Uint32 ItemSystem::getSubsystemID() const
{
	return IID_IItemSystem;
}

Uint32 ItemSystem::getMasterHandle()
{
	return m_hEntity;
}

bool ItemSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();

	if(strData.size() > 0)
	{
		ServerEngine::ItemSystemData itemSysData;
		ServerEngine::JceToObj(strData, itemSysData);
		initItemData(itemSysData);
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &ItemSystem::onEventSend2Client);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_POSTLEVELUP, this, &ItemSystem::onEventLevelUp);

	m_phyStrengthResume.init(m_hEntity);
	
	ITimeRangeValue* pTimeRangeValue = getComponent<ITimeRangeValue>(COMPNAME_TimeRangeValue, IID_ITimeRangeValue);
	assert(pTimeRangeValue);

	pTimeRangeValue->addRangeValue(&m_phyStrengthResume, "ResumePhyStrength");


	
	return true;
}


void ItemSystem::onEventLevelUp(EventArgs& args)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iNewLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	int iPreLv = args.context.getInt("oldlv");
	

	// 处理开启功能
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	for(int i = iPreLv+1; i <= iNewLevel; i++)
	{
		const FunctionOpenDesc* pFunctionData = pJZEntityFactory->getEnableFunctionData(i);
		if(!pFunctionData) continue;

		if(pEntity->isFunctionOpen(pFunctionData->iFunctionID) ) continue;
	
		pEntity->enableFunction(pFunctionData->iFunctionID);

		// 部分功能有特殊动作
		if(GSProto::en_Function_GodAnimal == pFunctionData->iFunctionID)
		{
			IGodAnimalSystem* pGodAnimalSys = static_cast<IGodAnimalSystem*>(pEntity->querySubsystem(IID_IGodAnimalSystem));
			assert(pGodAnimalSys);

			IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
			assert(pGlobalCfg);

			int iCfgGodAnimalID = pGlobalCfg->getInt("初始神兽", 5001);
			pGodAnimalSys->addGodAnimal(iCfgGodAnimalID, true, 0);
		}
		/*else if(GSProto::en_Function_Domain == pFunctionData->iFunctionID)
		{
			//to do 
			IDomainSystem* pDomainSys = static_cast<IDomainSystem*>(pEntity->querySubsystem(IID_IDomainSystem));
			assert(pDomainSys);
			pDomainSys->enableDomain();
		}*/
	}
}

void ItemSystem::onEventSend2Client(EventArgs& args)
{
	// 通知客户端
	GSProto::Cmd_Sc_BagData scBag;
	for(size_t i = 0; i < m_itemList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_itemList[i]);
		assert(pItem);

		
		int iItemType = pItem->getProperty(PROP_ITEM_TYPE, 0);
		
		GSProto::ItemInfo* pScItemInfo  = NULL;

		if( (iItemType == GSProto::en_ItemType_Item) || (iItemType == GSProto::en_ItemType_Favorite) )
		{
			pScItemInfo = scBag.mutable_szitemdata()->Add();
		}
		else if(iItemType == GSProto::en_ItemType_Material)
		{
			pScItemInfo = scBag.mutable_szmaterialdata()->Add();
		}
		else
		{
			assert(false);
		}

		pItem->packScItemInfo(*pScItemInfo);

		/*pScItemInfo->set_dwobjectid(pItem->getHandle() );
		pScItemInfo->set_iitemid(iBaseItemID);
		pScItemInfo->set_iitemcount(iStackCount);*/
	}

	GSProto::SCMessage scMsg;
	HelpMakeScMsg(scMsg, GSProto::CMD_BAG_DATA, scBag);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(scMsg);
}

void ItemSystem::initItemData(const ServerEngine::ItemSystemData& itemData)
{
	IItemFactory* pItemFactory = getComponent<IItemFactory>("ItemFactory", IID_IItemFactory);
	assert(pItemFactory);

	for(size_t i = 0; i < itemData.itemList.size(); i++)
	{
		IItem* pItem = pItemFactory->createItemFromDB(itemData.itemList[i]);
		assert(pItem);

		m_itemList.push_back(pItem->getHandle() );
	}

	m_buyPhyStrengthCount = itemData.buyPhyStengthCount;

	chatResetVal = itemData.iChatResetVal;
	IEntity *pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);
	int iValue = 	getDayResetValue(m_buyPhyStrengthCount).iValue;
	pActor->setProperty( PROP_ENTITY_STRENGTHBUYTIMES, iValue);

	m_ConvertMap = itemData.convertMap;
}

void ItemSystem::checkChatReset()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	if(!pZoneTime->IsInSameDay(pZoneTime->GetCurSecond(), chatResetVal.dwLastChgTime))
	{
		chatResetVal.iValue = 0;
		chatResetVal.dwLastChgTime = pZoneTime->GetCurSecond();
		IEntity *pActor = getEntityFromHandle(m_hEntity);
		assert(pActor);
		int iValue = 	getDayResetValue(m_buyPhyStrengthCount).iValue;
		pActor->setProperty( PROP_ENTITY_STRENGTHBUYTIMES, iValue);
	}
}


bool ItemSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& ItemSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_ITEM_USE);

		// 聊天处理，塞到这里吧
		resultList.push_back(GSProto::CMD_CHANNEL_CHAT);
		resultList.push_back(GSProto::CMD_WHISPER);

		// 体力购买，也塞到这里
		resultList.push_back(GSProto::CMD_BUY_PHYSTRENGTH);

		resultList.push_back(GSProto::CMD_QUERY_ACTORINFO);

		resultList.push_back(GSProto::CMD_ITEM_SELLITEM);

		//玩家头像改变

		resultList.push_back(GSProto::CMD_CHG_ACTOR_HEAD);
		resultList.push_back(GSProto::CMD_CHAT_CHANNEL_QUERY);

		resultList.push_back(GSProto::CMD_SEND_COVERT_CODE);

		resultList.push_back(GSProto::CMD_QUERY_CANSAVEMONEY);

		resultList.push_back(GSProto::CMD_TBT_GETUUID);

		
	}

	return resultList;
}

void ItemSystem::onReqQuerySaveMoney(const GSProto::CSMessage& msg)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	TC_Config cfgFile;
	cfgFile.parseFile(ServerConfig::ServerName + ".conf");
	bool bCanSaveMoney = TC_Common::strto<int>(cfgFile.get("/Zone<canSaveMoney>", "0") ) == 1;
	if(!bCanSaveMoney)
	{
		pEntity->sendErrorCode(ERROR_CANNOT_SAVEMONEY);
		return;
	}

	int iWorldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);

	GSProto::CMD_QUERY_CANSAVEMONEY_SC scCanMsg;
	scCanMsg.set_izoneid(iWorldID+1);

	pEntity->sendMessage(GSProto::CMD_QUERY_CANSAVEMONEY, scCanMsg);
}


void ItemSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);
	checkChatReset();
	switch(msg.icmd() )
	{
		case GSProto::CMD_ITEM_USE:
			onItemUseMsg(msg);
			break;

		case GSProto::CMD_CHANNEL_CHAT:
			onChatMsg(msg);
			break;

		case GSProto::CMD_WHISPER:
			onWhisperMsg(msg);
			break;

		case GSProto::CMD_BUY_PHYSTRENGTH:
			onReqBuyPhyStrength(msg);
			break;

		case GSProto::CMD_QUERY_ACTORINFO:
			onReqQueryActorInfo(msg);
			break;

		case GSProto::CMD_ITEM_SELLITEM:
			{
				onReqSellItem(msg);
			}break;

		//玩家头像改变
		case GSProto::CMD_CHG_ACTOR_HEAD:
			{
				onChgActorHead(msg);
			}break;

		case GSProto::CMD_CHAT_CHANNEL_QUERY:
			{
			
				onQueryChannel(msg);	
			}break;

		case GSProto::CMD_SEND_COVERT_CODE:
			{
				onSendConvertCode(msg);
			}break;

		case GSProto::CMD_QUERY_CANSAVEMONEY:
			onReqQuerySaveMoney(msg);
			break;

		case GSProto::CMD_TBT_GETUUID:
		       onGetTBTUUID(msg);
			break;

		
	}
}

void ItemSystem::onGetTBTUUID(const GSProto::CSMessage& msg)
{
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);

	GSProto::CMD_TBT_GETUUID_SC scMsg;
	scMsg.set_struuid(szUUIDString);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_TBT_GETUUID,  scMsg);
}

struct GetConvertCodeCb :public ServerEngine::ConvertCodePrxCallback
{
	GetConvertCodeCb(HEntity hEntity,string strConvertGiftId,string ConvertCode):
		m_hEntity(hEntity),
		m_strConvertGiftId(strConvertGiftId),
		m_ConvertCode(ConvertCode)
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;
		m_actorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	}
	void  callback_updateConvertCode(taf::Int32 ret, taf::Int32 state)
       {
       		FDLOG("GetConvertCodeCb") << m_ConvertCode<<"|" << m_strConvertGiftId<<m_actorName;
			IEntity *pEntity = getEntityFromHandle(m_hEntity);
			if(!pEntity) return;
        	if(ret ==  en_RedisRet_OK)
        	{
        		if(state == ServerEngine::en_ConvertCodeState_CanConvert)
        		{
        			IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
					assert(pItemSystem);
				 	pItemSystem->onConvertGift(m_strConvertGiftId,m_ConvertCode);
        		}
				else if(state == ServerEngine::en_ConvertCodeState_HaveConvert)
				{
				
					pEntity->sendErrorCode(ERROR_CONVERTCODE_HAVEUSE);
					return;
				}
        	}
			else 
			{
				pEntity->sendErrorCode(ERROR_CONVERTCODE_HAVENOT);
				return;
			}
			
        }

		virtual void callback_updateConvertCode_exception(taf::Int32 ret)
        {
			FDLOG("GetConvertCodeCb")<<"callback_updateConvertCode_exception |"<< ret<<endl;
		}
        virtual void callback_getConvertCode(taf::Int32 ret, taf::Int32 state)
        {
        	
        }
private:
	HEntity m_hEntity;
	string m_strConvertGiftId;
	string m_ConvertCode;
	string m_actorName;
		
};

void ItemSystem::onConvertGift(string strConvertCode, string ConvertCode)
{
	IDropFactory *pDropFactor = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactor);
	IItemFactory *pItemFactory =  getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	int iDropID = pItemFactory->getDropIdByConvertGiftId(strConvertCode);
	if(-1 == iDropID)
	{
		FDLOG("onConvertGift_Error")<< pEntity->getProperty(PROP_ACTOR_ACCOUNT, "")
			<<"|"<<pEntity->getProperty(PROP_ACTOR_WORLD, 0)<<"|"<<strConvertCode<<endl;
		pEntity->sendErrorCode(ERROR_CONVERTCODE_HAVENOT);
		return;
	}
	
	GSProto::CMD_SEND_COVERT_CODE_SC scMsg;
	GSProto::FightAwardResult& awardResult =*(scMsg.mutable_converresoult());
	bool res = pDropFactor->calcDrop( iDropID,  awardResult);
	if(!res) return;
		
	pDropFactor->excuteDrop(m_hEntity, awardResult, GSProto::en_Reason_ConvertCode_Convert);
	//
	bool bFalg = pItemFactory->bActorCanRepeatGetFlag(strConvertCode);
	if(!bFalg)
	{
		m_ConvertMap.insert(make_pair(strConvertCode,true));
	}
	
	pEntity->sendMessage(GSProto::CMD_SEND_COVERT_CODE,  scMsg);
	
}


void  ItemSystem::onSendConvertCode(const GSProto::CSMessage& msg)
{
	//是否自己已经兑换了同类型礼包?	

	GSProto::CMD_SEND_COVERT_CODE_CS csMsg;

	if(! csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	string strConvertCode = csMsg.strcovertcode();
	if(strConvertCode.size() != ConvertCodeLength)
	{
		pEntity->sendErrorCode(ERROR_CONVERTCODE_HAVENOT);
		return;
	}

	stringstream oss;
	oss<<strConvertCode[1]<<strConvertCode[7]<<strConvertCode[3]<<strConvertCode[8];
	//cout<<oss.str().c_str();
	
	IItemFactory *pItemFactory =  getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);
	bool bFalg = pItemFactory->bActorCanRepeatGetFlag(oss.str());
	if(!bFalg)
	{

		map<string ,bool >::iterator iter = m_ConvertMap.find(oss.str());
		if(iter !=  m_ConvertMap.end())
		{
			pEntity->sendErrorCode(ERROR_CONVERTCODE_ACTORHAVE);
			return;
		}
	
	}

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	string strSpecial = pGlobal->getString("微信特殊兑换码", "y04afcu1bv#21002");
	vector<string> specialVec = TC_Common::sepstr<string>(strSpecial,"#");
	assert(specialVec.size() == 2);
	if(strConvertCode == specialVec[0])
	{

		map<string ,bool >::iterator iter = m_ConvertMap.find(strConvertCode);
		if(iter !=  m_ConvertMap.end())
		{
			pEntity->sendErrorCode(ERROR_CONVERTCODE_ACTORHAVE);
			return;
		}
		
		IDropFactory *pDropFactor = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
		assert(pDropFactor);
	
		GSProto::CMD_SEND_COVERT_CODE_SC scMsg;
		GSProto::FightAwardResult& awardResult =*(scMsg.mutable_converresoult());
		int iDropID = TC_Common::TC_S2I(specialVec[1]);
		bool res = pDropFactor->calcDrop( iDropID,  awardResult);
		if(!res) return;
		
		pDropFactor->excuteDrop(m_hEntity, awardResult, GSProto::en_Reason_ConvertCode_Convert);
		//
		m_ConvertMap.insert(make_pair(oss.str(),true));
		pEntity->sendMessage(GSProto::CMD_SEND_COVERT_CODE,  scMsg);
		
		return;
	}
	
	int state = (int)ServerEngine::en_ConvertCodeState_HaveConvert;
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);
	pMsgLayer->AsyncUpdateConvertCode(strConvertCode, state, new  GetConvertCodeCb(m_hEntity, oss.str(),strConvertCode));

	//这个是否被别人使用了
}

void ItemSystem::onChgActorHead(const GSProto::CSMessage& msg)
{
	GSProto::CMD_CHG_ACTOR_HEAD_CS csMsg;
	if( !csMsg.ParseFromString( msg.strmsgbody()))
	{
		return;
	}
	GSProto::enHeadType type = csMsg.enheadtype();
	int iHeadId = csMsg.iheadid();
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	if(type == GSProto::en_HeadType_Hero)
	{
		
		IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
		assert(pHeroSys);

		vector<HEntity> hEntityVec = pHeroSys->getHeroList();
	
		for(size_t i = 0; i < hEntityVec.size(); ++i)
		{
			IEntity *pHeroEntity = getEntityFromHandle(hEntityVec[i]);
			int iTmpHeroID = pHeroEntity->getProperty(PROP_ENTITY_BASEID, 0);
			if(iTmpHeroID == iHeadId)
			{
				pEntity->setProperty(PROP_ENTITY_ACTOR_HEADTYPE, GSProto::en_HeadType_Hero);
				pEntity->setProperty(PROP_ENTITY_ACTOR_HEAD, iHeadId);
				
				pEntity->sendMessage( GSProto::CMD_CHG_ACTOR_HEAD);
				return;
			}
		}
		
	}
	else if( type == GSProto::en_HeadType_GodAnimal)
	{	
		IGodAnimalSystem* pHeroSys = static_cast<IGodAnimalSystem*>( pEntity->querySubsystem(IID_IGodAnimalSystem));
		assert(pHeroSys);

		vector<HEntity> hEntityVec = pHeroSys->getGodAnimalList();
	
		for(size_t i = 0; i < hEntityVec.size(); ++i)
		{
			IEntity *pGodAnimalEntity = getEntityFromHandle(hEntityVec[i]);
			int iTmpHeroID = pGodAnimalEntity->getProperty(PROP_ENTITY_BASEID, 0);
			if(iTmpHeroID == iHeadId )
			{
				pEntity->setProperty(PROP_ENTITY_ACTOR_HEADTYPE, GSProto::en_HeadType_Hero);
				pEntity->setProperty(PROP_ENTITY_ACTOR_HEAD, iHeadId);
				pEntity->sendMessage( GSProto::CMD_CHG_ACTOR_HEAD);
				return;
			}
		}
	}
}

void ItemSystem::onReqSellItem(const GSProto::CSMessage& msg)
{
	GSProto::CMD_ITEM_SELLITEM_CS csMsg;
	if( ! csMsg.ParseFromString( msg.strmsgbody()))
	{
		return;
	}

	int iBaseId = csMsg.iitembaseid();
	int iCount = csMsg.iitemcount();

	//1.物品可不可以出售
	IItemFactory* pItemFactory = getComponent<IItemFactory>("ItemFactory", IID_IItemFactory);
	assert(pItemFactory);
	const PropertySet* pPropSet = pItemFactory->getItemPropset(iBaseId);
	if(!pPropSet) return;

	int iItemType = pPropSet->getInt(PROP_ITEM_TYPE, 0);
	if( (GSProto::en_ItemType_Item != iItemType) && (GSProto::en_ItemType_Favorite != iItemType) )
	{
		return;
	}
	
	//2.玩家有没有这个物品
	int iActorCount = getItemCount( iBaseId);
	if(0 == iActorCount)
	{
		return;
	}
	
	//3.验证客户端上行数量
	if( ( iCount>iActorCount) || ( iCount <= 0))
	{
		return;
	}
	
	//4.删掉物品
	bool res = removeItem(iBaseId,iCount,GSProto::en_Reason_ItemSell);
	if(!res)
	{
		return;
	}

	//5.给钱
	int iItemPrice = pPropSet->getInt(PROP_ITEM_PRICE, 0);
	int iItemCostType = pPropSet->getInt(PROP_ITEM_SALETYPE, 0);

	int iSellValue = iItemPrice * iCount;
	assert(iSellValue>0);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int lifeatt = LifeAtt2Prop(iItemCostType);
	assert(lifeatt>0);
	
	if(lifeatt == PROP_ACTOR_SILVER)
	{
		pEntity->changeProperty(PROP_ACTOR_SILVER, iSellValue, GSProto::en_Reason_ItemSell);
	}
	else if(lifeatt == PROP_ACTOR_GOLD)
	{
		pEntity->changeProperty(PROP_ACTOR_GOLD, iSellValue, GSProto::en_Reason_ItemSell);
	}
	else
	{
		return;
	}

	pEntity->sendMessage( GSProto::CMD_ITEM_SELLITEM);
}

void ItemSystem::onReqQueryActorInfo(const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	m_phyStrengthResume.checkResume(time(0) );

	int iNeedSecond = pGlobalCfg->getInt("体力恢复间隔", 300);

	GSProto::CMD_QUERY_ACTORINFO_SC scMsg;
	
	int iPhyStrengthLimit = pActor->getProperty(PROP_ENTITY_PHYSTRENGTHLIMIT, 0);
	int iTkPhyStrngth = pActor->getProperty(PROP_ENTITY_PHYSTRENGTH, 0);
	Uint32 dwLastResumtTime = pActor->getProperty(PROP_ENTITY_LASTPHYSTRENGTHTIME, 0);

	// 达到上限，不恢复
	if(iTkPhyStrngth >= iPhyStrengthLimit)
	{
		scMsg.set_iphyresumecd(0);
		scMsg.set_ifullphyresumecd(0);
	}
	else
	{
		// 可能存在延迟导致该回血了，但是还没来得及(10s误差,定时器间隔),
		// 这个时候，主动调用一次检测回血, 并且刷新当前体力和上次恢复时间
		if(dwLastResumtTime + (Uint32)iNeedSecond <= (Uint32)time(0) )
		{
			m_phyStrengthResume.checkResume((Uint32)time(0) );
			iTkPhyStrngth = pActor->getProperty(PROP_ENTITY_PHYSTRENGTH, 0);
			dwLastResumtTime = pActor->getProperty(PROP_ENTITY_LASTPHYSTRENGTHTIME, 0);
		}
	
		Uint32 dwLeftSecond = dwLastResumtTime + (Uint32)iNeedSecond - (Uint32)time(0);
		scMsg.set_iphyresumecd( (int)dwLeftSecond);

		int iLeftPhy = iPhyStrengthLimit - 1 - iTkPhyStrngth;
		scMsg.set_ifullphyresumecd((int)dwLeftSecond + iLeftPhy * iNeedSecond);
	}

		//军团归属
	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory,IID_ILegionFactory);
	assert(pLegionFactory);
	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	ILegion* pLengion = pLegionFactory->getActorLegion(strName);
	string belongName = "";
	if(pLengion)
	{
		belongName = pLengion->getLegionName();
		scMsg.set_strlegionname(belongName);
	}
	pActor->sendMessage(GSProto::CMD_QUERY_ACTORINFO, scMsg);
}

void ItemSystem::onQueryChannel(const GSProto::CSMessage& msg)
{	
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	GSProto::CMD_CHAT_CHANNEL_QUERY_SC scMsg;
	int iTotalHave = pGlobal->getInt("世界聊天次数", 5);
	if(chatResetVal.iValue >= iTotalHave )
	{
		scMsg.set_iremaindtimes(0);
		int iCost = pGlobal->getInt("世界聊天花费元宝", 5);
		scMsg.set_icost( iCost);
	}
	else
	{
		scMsg.set_iremaindtimes(iTotalHave - chatResetVal.iValue);
		scMsg.set_itotaltimes(iTotalHave);
	}
	IEntity *pEntity =getEntityFromHandle(m_hEntity);
	pEntity->sendMessage(GSProto::CMD_CHAT_CHANNEL_QUERY,  scMsg);
}

void ItemSystem::onChatMsg(const GSProto::CSMessage& msg)
{
	PROFILE_MONITOR("onChatMsg");
	GSProto::CMD_CHANNEL_CHAT_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iChannel = req.ichancel();
	string strMsg = req.strmsg();
	//敏感词过滤
	IReserveWordCheck* pReserveWordCheck = getComponent<IReserveWordCheck>("ReserveWordCheck", IID_IReserveWordCheck);
	assert(pReserveWordCheck);
	int iWordCount = pReserveWordCheck->calLength(strMsg.c_str());
	
	if( (iWordCount > GSProto::MAX_CHAT_LEN) || (iWordCount < 0) )
	{
		pEntity->sendErrorCode(ERROR_CHAT_MSG_TOOLONG);
		return ;
	}
	
	bool bReserverRes = pReserveWordCheck->hasReserveWord(req.strmsg());
	if(bReserverRes)
	{
		pEntity->sendErrorCode(ERROR_CHAT_HAVE_RESERVER);
		return;
	}
	
	GSProto::CMD_CHANNEL_CHAT_SC scMsg;
	scMsg.set_strsender(pEntity->getProperty(PROP_ENTITY_NAME, ""));
	GSProto::PBPkRole* pPkRole = scMsg.mutable_senderkey();
	pPkRole->set_straccount(pEntity->getProperty(PROP_ACTOR_ACCOUNT, "") );
	pPkRole->set_iworldid(pEntity->getProperty(PROP_ACTOR_WORLD, 0) );
	scMsg.set_strmsg(req.strmsg() );
	scMsg.set_ichannel(req.ichancel() );

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_CHANNEL_CHAT, scMsg);

	if(GSProto::en_ChatChannel_World == iChannel)
	{

		IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobal);
		int iTotalHave = pGlobal->getInt("世界聊天次数", 5);
		if(chatResetVal.iValue >= iTotalHave )
		{
			int iCost = pGlobal->getInt("世界聊天花费元宝", 5);
			int iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD,0);
			if(iActorHave < iCost)
			{
				pEntity->sendErrorCode(ERROR_NEED_GOLD);
				return;
			}
			//扣钱
			pEntity->changeProperty( PROP_ACTOR_GOLD, 0-iCost, GSProto::en_Reason_WORLD_CHATCOST);
		}
		chatResetVal.iValue ++;
		IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZoneTime);
		chatResetVal.dwLastChgTime = pZoneTime->GetCurSecond();
		GSProto::CSMessage msg;
		onQueryChannel(msg );
		
		IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
		assert(pJZMessageLayer);
		
		pJZMessageLayer->broadcastMsg(pkg);
	}
	else if(GSProto::en_ChatChannel_Legion == iChannel)
	{
		ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
		assert(pLegionFactory);

		string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
		ILegion* pLegoon =  pLegionFactory->getActorLegion(strActorName);
		if(pLegoon)
		{
			pLegoon->broadcastMessage(pkg);
		}
		else
		{
			pEntity->sendErrorCode(ERROR_NOTIN_LEGIONCHAT);
		}
	}
}

void ItemSystem::onWhisperMsg(const GSProto::CSMessage& msg)
{
	GSProto::CMD_WHISPER_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	string strRcv = req.strrecver();

	//敏感词过滤
	IReserveWordCheck* pReserveWordCheck = getComponent<IReserveWordCheck>("ReserveWordCheck", IID_IReserveWordCheck);
	assert(pReserveWordCheck);
	int iWordCount = pReserveWordCheck->calLength(req.strmsg().c_str());
	
	if( (iWordCount > GSProto::MAX_CHAT_LEN) || (iWordCount < 0) )
	{
		pEntity->sendErrorCode(ERROR_CHAT_MSG_TOOLONG);
		return ;
	}
	
	bool bReserverRes = pReserveWordCheck->hasReserveWord(req.strmsg());
	if(bReserverRes)
	{
		pEntity->sendErrorCode(ERROR_CHAT_HAVE_RESERVER);
		return;
	}
	

	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);

	HEntity hRcv = pUserStateMgr->getActorByName(strRcv);
	IEntity* pRcv = getEntityFromHandle(hRcv);
	if(!pRcv)
	{
		pEntity->sendErrorCode(ERROR_RCV_NOTONLINE);
		return;
	}

	GSProto::CMD_WHISPER_SC scMsg;
	scMsg.set_strsender(pEntity->getProperty(PROP_ENTITY_NAME, "") );
	GSProto::PBPkRole* pSenderKey = scMsg.mutable_senderkey();
	pSenderKey->set_straccount(pEntity->getProperty(PROP_ACTOR_ACCOUNT, "")  );
	pSenderKey->set_iworldid(pEntity->getProperty(PROP_ACTOR_WORLD, 0) );

	scMsg.set_strrecver(strRcv);
	GSProto::PBPkRole* pRcvKey = scMsg.mutable_recverkey();
	pRcvKey->set_straccount(pRcv->getProperty(PROP_ACTOR_ACCOUNT, "")  );
	pRcvKey->set_iworldid(pRcv->getProperty(PROP_ACTOR_WORLD, 0) );

	scMsg.set_strmsg(req.strmsg() );

	pEntity->sendMessage(GSProto::CMD_WHISPER,  scMsg);
	pRcv->sendMessage(GSProto::CMD_WHISPER,  scMsg);
}

void ItemSystem::onReqBuyPhyStength()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	int iVIPLevel = pActor->getProperty(PROP_ACTOR_VIPLEVEL, 0);
	int iBuyLimitCount = getBuyPhyCountLimit(iVIPLevel);

	if(getDayResetValue(m_buyPhyStrengthCount).iValue >= iBuyLimitCount)
	{
		
		IVIPFactory* pVIPFaactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
		assert(pVIPFaactory);
		if( pVIPFaactory->getVipTopLevel() >iVIPLevel )
		{
			//通知客户端提高vip等级
			pActor->sendMessage(GSProto::CMD_NOTICE_CLIENT_PUSHVIP_SCENE);
			return;
		}
		pActor->sendErrorCode(ERROR_BUYPHYSTRENGTHLIMIT);
		return;
	}

	// 通知客户端确认
	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);

	int iCostGold = 0;
	int iCount = getDayResetValue(m_buyPhyStrengthCount).iValue;
	if(!getBuyCostGold( iCount + 1, iCostGold) )
	{
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	int iBuyPhyStrength = pGlobalCfg->getInt("购买体力点数", 120);

	EventArgs args;
	args.context.setInt( "entity", m_hEntity);
	args.context.setInt( "cost", iCostGold);
	args.context.setInt( "getphy", iBuyPhyStrength);
	args.context.setInt( "haveByTimes", iCount);
	pScriptEngine->runFunction("NotiyBuyPhyStrength", &args, "EventArgs");
}

void ItemSystem::onReqBuyPhyStrength(const GSProto::CSMessage& msg)
{
	onReqBuyPhyStength();
}

int ItemSystem::getBuyPhyCountLimit(int iVIPLevel)
{
	IVIPFactory* pVIPFaactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVIPFaactory);

	int iValue = pVIPFaactory->getVipPropByHEntity(m_hEntity, VIP_PROP_BUY_PHYSTRENGTH_TIMES);

	return iValue;
}

bool ItemSystem::getBuyCostGold(int iCount, int& iCostGold)
{
	ITable* pTable = getCompomentObjectManager()->findTable("BuyPhyCost");
	assert(pTable);

	int iRecord = pTable->findRecord(iCount);
	if(iRecord < 0) return false;

	iCostGold = pTable->getInt(iRecord, "消耗元宝");

	return true;
}


bool ItemSystem::buyPhyStength()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	int iVIPLevel = pActor->getProperty(PROP_ACTOR_VIPLEVEL, 0);
	int iBuyLimitCount = getBuyPhyCountLimit(iVIPLevel);

	if(getDayResetValue(m_buyPhyStrengthCount).iValue >= iBuyLimitCount)
	{
		pActor->sendErrorCode(ERROR_BUYPHYSTRENGTHLIMIT);
		return false;
	}

	
	int iCostGold = 0;
	if(!getBuyCostGold(getDayResetValue(m_buyPhyStrengthCount).iValue + 1, iCostGold) )
	{
		return false;
	}

	int iTkGold = pActor->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iCostGold)
	{
		pActor->sendErrorCode(ERROR_NEED_GOLD);
		return false;
	}

	pActor->changeProperty(PROP_ACTOR_GOLD, 0-iCostGold, GSProto::en_Reason_BuyPhyStrengthConsume);

	// 增加购买体力次数
	getDayResetValue(m_buyPhyStrengthCount).iValue++;
	pActor->changeProperty( PROP_ENTITY_STRENGTHBUYTIMES, 1,  GSProto::en_Reason_BuyPhyStrengthConsume);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iBuyPhyStrength = pGlobalCfg->getInt("购买体力点数", 120);
	pActor->changeProperty(PROP_ENTITY_PHYSTRENGTH, iBuyPhyStrength, GSProto::en_Reason_BuyPhyStrengthCreate);

	int iResultPhyStrength = pActor->getProperty(PROP_ENTITY_PHYSTRENGTH, 0);
	PLAYERLOG(pActor)<<"BuyPhyStrength|"<<iCostGold<<"|"<<iBuyPhyStrength<<"|"<<iResultPhyStrength<<endl;

	return true;
}


void ItemSystem::onItemUseMsg(const GSProto::CSMessage& msg)
{
	GSProto::Cmd_Cs_ItemUse reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	HItem hItem = reqMsg.dwitemobjectid();

	// 判断物品是否合法
	vector<HItem>::iterator it = find(m_itemList.begin(), m_itemList.end(), hItem);
	if(it == m_itemList.end() )
	{
		return;
	}

	IItem* pItem = getItemFromHandle(hItem);
	if(!pItem) return;

	// 判断是否可使用
	if(0 == pItem->getProperty(PROP_ITEM_USEEFFECT, 0) )
	{
		pEntity->sendErrorCode(ERROR_ITEM_CANNOTUSE);
		return;
	}

	//xh 
	int baseID = pItem->getProperty(PROP_ITEM_BASEID, 0);
	if(baseID <=19000 || baseID >=19997)
	{
		pEntity->sendErrorCode(ERROR_ITEM_CANNOTUSE);
		return;
	}

	//PropertySet ctx;
	//if(!pItem->useItem(m_hEntity, ctx) )
	//{
	//	return;
//	}

	//使用物品

	int iDropId = pItem->getProperty(PROP_EQUIP_DROPID, 0);
	assert(iDropId!=0);

	GSProto::CMD_ITEM_USE_SC scMsg;

	IDropFactory *pDropFac = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFac);

	bool resCalcDrop = pDropFac->calcDrop(iDropId, *(scMsg.mutable_awardresoult()));
	assert(resCalcDrop);
	pDropFac->excuteDrop(m_hEntity, *(scMsg.mutable_awardresoult()), GSProto::en_Reason_Item_Use);
		
	pEntity->sendMessage(GSProto::CMD_ITEM_USE, scMsg);
		
	// 如果物品在使用中扣除了，不处理
	if(NULL == getItemFromHandle(hItem) )
	{
		return;
	}

	// 如果堆叠数目小于1
	int iStackCount = pItem->getProperty(PROP_ITEM_STACKCOUNT, 0);
	if(iStackCount <= 1)
	{
		removeItemByHandle(hItem, GSProto::en_Reason_ItemUse);
		return;
	}

	pItem->setProperty(PROP_ITEM_STACKCOUNT, iStackCount-1);

	// 通知客户端
	GSProto::Cmd_Sc_BagChg scBagChg;
	GSProto::BagChgInfo* pNewChg = scBagChg.mutable_szbagchglist()->Add();
	assert(pNewChg);

	int iItemType = pItem->getProperty(PROP_ITEM_TYPE, 0);
	int iBagType = getBagType(iItemType);

	scBagChg.set_ibagtype(iBagType);
	
	pNewChg->set_ichgtype(GSProto::en_BagChg_StackCountChg);
	pItem->packScItemInfo(*pNewChg->mutable_itemdetail() );

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_BAG_CHG, scBagChg);

	pEntity->sendMessage(pkg);
}

int ItemSystem::getBagType(int iItemType)
{
	if( (iItemType == GSProto::en_ItemType_Item) || (iItemType == GSProto::en_ItemType_Favorite) )
	{
		return GSProto::en_BagType_Item;
	}
	else if(iItemType == GSProto::en_ItemType_Material)
	{
		return GSProto::en_BagType_Materail;
	}

	return 999;
}


void ItemSystem::packSaveData(string& data)
{
	// 为Ghost 打包，无需物品数据
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	bool bForGhost = pEntity->getProperty(PROP_ENTITY_PACKFORGHOST, 0) == 1;
	if(bForGhost) return;
	

	ServerEngine::ItemSystemData itemSysData;
	itemSysData.itemList.resize(m_itemList.size() );
	
	for(size_t i = 0; i < m_itemList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_itemList[i]);
		assert(pItem);

		pItem->packJce(itemSysData.itemList[i]);
	}

	itemSysData.buyPhyStengthCount = m_buyPhyStrengthCount;
 	itemSysData.iChatResetVal = chatResetVal ;
	//add by hyf

	itemSysData.convertMap = m_ConvertMap;
	data = ServerEngine::JceToStr(itemSysData);
}

bool ItemSystem::addItem(int iItemID, int iCount, int iReason)
{
	IItemFactory* pItemFactory = getComponent<IItemFactory>("ItemFactory", IID_IItemFactory);
	assert(pItemFactory);

	const PropertySet* pPropSet = pItemFactory->getItemPropset(iItemID);
	if(!pPropSet) return false;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 装备走装备路线
	int iItemType = pPropSet->getInt(PROP_ITEM_TYPE, 0);
	if(GSProto::en_ItemType_Equip == iItemType)
	{
		IEquipBagSystem* pEquipBagSys = static_cast<IEquipBagSystem*>(pEntity->querySubsystem(IID_IEquipBagSystem) );
		assert(pEquipBagSys);

		if(1 != iCount) return false;

		return pEquipBagSys->addEquip(iItemID, iReason, true);
	}

	int iBackupCount = iCount;
	GSProto::Cmd_Sc_BagChg scBagChg;
	int iBagType = getBagType(iItemType);
	scBagChg.set_ibagtype(iBagType);

	for(size_t i = 0; i < m_itemList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_itemList[i]);
		assert(pItem);

		int iTmpBaseID = pItem->getProperty(PROP_ITEM_BASEID, 0);
		if(iTmpBaseID != iItemID) continue;

		int iMaxStackCount = pItem->getProperty(PROP_ITEM_MAXSTACKCOUNT, 0);
		int iStackCount = pItem->getProperty(PROP_ITEM_STACKCOUNT, 0);

		if(iMaxStackCount == iStackCount) continue;
		
		int iUseCount = std::min(iMaxStackCount - iStackCount, iCount);
		pItem->setProperty(PROP_ITEM_STACKCOUNT, iStackCount + iUseCount);
		iCount -= iUseCount;

		// 打包发送给客户端的信息
		GSProto::BagChgInfo* pNewScChg = scBagChg.mutable_szbagchglist()->Add();
		pNewScChg->set_ichgtype(GSProto::en_BagChg_StackCountChg);
		pItem->packScItemInfo(*pNewScChg->mutable_itemdetail() );
		
		if(iCount <= 0)break;
	}

	// 剩下的，直接创建新的
	vector<HItem> newItemList;
	bool bResult = pItemFactory->createItemFromID(iItemID, iCount, newItemList);
	assert(bResult);

	for(size_t i = 0; i < newItemList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(newItemList[i]);
		assert(pItem);
	
		GSProto::BagChgInfo* pNewScChg = scBagChg.mutable_szbagchglist()->Add();		
		pNewScChg->set_ichgtype(GSProto::en_BagChg_Add);
		pItem->packScItemInfo(*pNewScChg->mutable_itemdetail() );
	}

	// 添加到后面
	std::copy(newItemList.begin(), newItemList.end(), std::inserter(m_itemList, m_itemList.end() ) );

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_BAG_CHG, scBagChg);

	pEntity->sendMessage(pkg);

	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("item", iItemID);
	args.context.setInt("count", iBackupCount);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_ITEMCHG, args);

	PLAYERLOG(pEntity)<<"AddItem|"<<iItemID<<"|"<<iBackupCount<<"|"<<iReason<<endl;

	return true;
}


bool ItemSystem::removeItem(int iItemID, int iItemCount, int iReason)
{
	int iBackupItemCount = iItemCount;
	int iOwnCount = getItemCount(iItemID);
	if(iOwnCount < iItemCount)
	{
		return false;
	}

	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	const PropertySet* pItemStaticProp = pItemFactory->getItemPropset(iItemID);
	assert(pItemStaticProp);

	int iItemType = pItemStaticProp->getInt(PROP_ITEM_TYPE, 0);
	int iBagType = getBagType(iItemType);

	GSProto::Cmd_Sc_BagChg scBagChg;
	scBagChg.set_ibagtype(iBagType);

	for(vector<HItem>::iterator it = m_itemList.begin(); it != m_itemList.end(); )
	{
		IItem* pTmpItem= getItemFromHandle(*it);
		assert(pTmpItem);

		int iTmpItemID = pTmpItem->getProperty(PROP_ITEM_BASEID, 0);
		if(iTmpItemID != iItemID)
		{
			it++;
			continue;
		}

		int iStackCount = pTmpItem->getProperty(PROP_ITEM_STACKCOUNT, 0);
		if(iItemCount < iStackCount)
		{
			pTmpItem->setProperty(PROP_ITEM_STACKCOUNT, iStackCount - iItemCount);
			iItemCount = 0;

			GSProto::BagChgInfo* pBagChg = scBagChg.mutable_szbagchglist()->Add();
			pBagChg->set_ichgtype(GSProto::en_BagChg_StackCountChg);
			pTmpItem->packScItemInfo(*pBagChg->mutable_itemdetail() );
		}
		else
		{
			GSProto::BagChgInfo* pBagChg = scBagChg.mutable_szbagchglist()->Add();
			pBagChg->set_ichgtype(GSProto::en_BagChg_Del);
			pBagChg->set_delobjectid(*it);
			
			it = m_itemList.erase(it);
			delete pTmpItem;
			iItemCount -= iStackCount;
		}

		if(iItemCount <= 0) break;
	}

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_BAG_CHG, scBagChg);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	pEntity->sendMessage(pkg);

	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("item", iItemID);
	args.context.setInt("count", 0-iBackupItemCount);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_ITEMCHG, args);

	PLAYERLOG(pEntity)<<"RemoveItem|"<<iItemID<<"|"<<iBackupItemCount<<"|"<<iReason<<endl;

	return true;
}

bool ItemSystem::removeItemByHandle(HItem hItem, int iReason)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	vector<HItem>::iterator it = find(m_itemList.begin(), m_itemList.end(), hItem);
	if(it == m_itemList.end() )
	{
		return false;
	}

	m_itemList.erase(it);
	IItem* pItem = getItemFromHandle(hItem);
	assert(pItem);

	int iItemID = pItem->getProperty(PROP_ITEM_BASEID, 0);
	int iItemCount = pItem->getProperty(PROP_ITEM_STACKCOUNT, 0);
	int iItemType = pItem->getProperty(PROP_ITEM_TYPE, 0);

	PLAYERLOG(pEntity)<<"RemoveItem|"<<iItemID<<"|"<<iItemCount<<"|"<<iReason<<endl;
	delete pItem;

	// 通知客户端
	int iBagType = getBagType(iItemType);
	
	GSProto::Cmd_Sc_BagChg scBagChg;
	scBagChg.set_ibagtype(iBagType);

	GSProto::BagChgInfo* pBagChg = scBagChg.mutable_szbagchglist()->Add();
	pBagChg->set_ichgtype(GSProto::en_BagChg_Del);
	pBagChg->set_delobjectid(hItem);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_BAG_CHG, scBagChg);

	
	pEntity->sendMessage(pkg);

	return true;
}


int ItemSystem::getItemCount(int iItemID)
{
	int iResultCount = 0;
	for(size_t i = 0; i < m_itemList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_itemList[i]);
		assert(pItem);

		int iTmpBaseID = pItem->getProperty(PROP_ITEM_BASEID, 0);
		if(iTmpBaseID != iItemID) continue;

		int iTmpCount = pItem->getProperty(PROP_ITEM_STACKCOUNT, 0);
		iResultCount += iTmpCount;
	}

	return iResultCount;
}

void ItemSystem::GMOnlineGetBagData(string& strJson)
{
	
	strJson = "{\"cmd\":\"rolebag\",\"data\":[";

	for(size_t i = 0; i < m_itemList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_itemList[i]);
		assert(pItem);
		
		int iBaseItemID = pItem->getProperty(PROP_ITEM_BASEID, 0);
		int iStackCount = pItem->getProperty(PROP_ITEM_STACKCOUNT, 0);
		
		strJson+="{\"itemid\":\""+TC_I2S(iBaseItemID)+"\","+"\"count\":\""+TC_I2S(iStackCount)+"\"}";
		
		if( i != m_itemList.size()-1) strJson+=",";
	}
	strJson += "]}";
	
	//cout<<strJson.c_str()<<endl;
}



TimeRangeExecute PhyStrengthResume::getExecuteDelegate()
{
	return TimeRangeExecute(this, &PhyStrengthResume::doResume);
}

TimeRangeCheck PhyStrengthResume::getCheckDelegate()
{
	return TimeRangeCheck(this, &PhyStrengthResume::checkResume);
}

bool PhyStrengthResume::checkResume(Uint32 dwTime)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return false;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	Uint32 dwCutTime = time(0);
	Uint32 dwBeginDaySecond = pZoneTime->GetDayBeginSecond(dwCutTime);
	int iNeedSecond = pGlobalCfg->getInt("体力恢复间隔", 300);

	
	Uint32 dwLastResumtTime = pEntity->getProperty(PROP_ENTITY_LASTPHYSTRENGTHTIME, 0);
	if(0 == dwLastResumtTime)
	{
		// 序列化到整点
		Uint32 dwTodayPassSecond = dwCutTime - dwBeginDaySecond;
		pEntity->setProperty(PROP_ENTITY_LASTPHYSTRENGTHTIME, (int)(dwBeginDaySecond + dwTodayPassSecond/iNeedSecond * iNeedSecond) );
		return false;
	}

	// 序列化到整段
	Uint32 dwLastResumeDaySecond = pZoneTime->GetDayBeginSecond(dwLastResumtTime);
	dwLastResumtTime = dwLastResumeDaySecond + (dwLastResumtTime - dwLastResumeDaySecond)/iNeedSecond * iNeedSecond;
	pEntity->setProperty(PROP_ENTITY_LASTPHYSTRENGTHTIME, (int)dwLastResumtTime);
	

	// 如果超过上限,不恢复
	int iPhyStrengthLimit = pEntity->getProperty(PROP_ENTITY_PHYSTRENGTHLIMIT, 0);
	int iTkPhyStrngth = pEntity->getProperty(PROP_ENTITY_PHYSTRENGTH, 0);
	if(dwCutTime < (dwLastResumtTime + (Uint32)iNeedSecond) )
	{
		return false;
	}

	CloseAttCommUP  close(m_hEntity);

	// 体力达到上限，不+了，但是时间要计算
	Uint32 dwPassSecond = dwCutTime - dwLastResumtTime;
	Uint32 dwCount = dwPassSecond/(Uint32)iNeedSecond;
	pEntity->setProperty(PROP_ENTITY_LASTPHYSTRENGTHTIME, (int)(dwLastResumtTime + dwCount * iNeedSecond) );
	if(iTkPhyStrngth >= iPhyStrengthLimit) return false;

	// 未达上限, 恢复体力
	int iAddPhyStrength = (int)dwCount;
	int iResultPhyStrength = std::min(iTkPhyStrngth + iAddPhyStrength, iPhyStrengthLimit);
	pEntity->changeProperty(PROP_ENTITY_PHYSTRENGTH, iResultPhyStrength - iTkPhyStrngth, GSProto::en_Reason_ResumePhyStrengthCreate);

	PLAYERLOG(pEntity)<<"ResumePhyStrength|"<<(iResultPhyStrength - iTkPhyStrngth)<<"|"<<iResultPhyStrength<<endl;

	return true;
}

void PhyStrengthResume::doResume(Uint32 dwTime)
{
	// nothing
}






