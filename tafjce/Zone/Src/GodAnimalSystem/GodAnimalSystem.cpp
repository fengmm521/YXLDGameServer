#include "GodAnimalSystemPch.h"
#include "GodAnimalSystem.h"
#include "ISkillSystem.h"
#include "IItemFactory.h"
#include "IItemSystem.h"
#include "IJZEntityFactory.h"

extern "C" IObject* createGodAnimalSystem()
{
	return new GodAnimalSystem;
}

extern int Prop2LifeAtt(int iPropID);
extern int LifeAtt2Prop(int iLifeAtt);

GodAnimalSystem::GodAnimalSystem()
{
}

GodAnimalSystem::~GodAnimalSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &GodAnimalSystem::onEventSend2Client);
	}

	for(size_t i = 0; i < m_godAnimalList.size(); i++)
	{
		IEntity* pTmpEntity = getEntityFromHandle(m_godAnimalList[i]);
		if(!pTmpEntity) continue;

		delete pTmpEntity;
	}
}

Uint32 GodAnimalSystem::getSubsystemID() const
{
	return IID_IGodAnimalSystem;
}

Uint32 GodAnimalSystem::getMasterHandle()
{
	return m_hEntity;
}

bool GodAnimalSystem::create(IEntity* pEntity, const std::string& strData)
{
	assert(pEntity);
	m_hEntity = pEntity->getHandle();

	initGodAnimalData(strData);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &GodAnimalSystem::onEventSend2Client);

	return true;
}

void GodAnimalSystem::initGodAnimalData(const string& strData)
{
	if(strData.size() == 0)
	{
		return;
	}

	ServerEngine::GodAnimalSystemData saveData;
	ServerEngine::JceToObj(strData, saveData);

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);

	for(size_t i = 0; i < saveData.godAnimalList.size(); i++)
	{
		ServerEngine::RoleSaveData& tmpGodAnimalData = saveData.godAnimalList[i];
		tmpGodAnimalData.basePropData.roleIntPropset[PROP_ENTITY_MASTER] = m_hEntity;
		string strTmpGodAnimalData = ServerEngine::JceToStr(tmpGodAnimalData);
		IEntity* pTmpGold = pEntityFactory->createEntity("GodAnimal", strTmpGodAnimalData);
		assert(pTmpGold);

		m_godAnimalList.push_back(pTmpGold->getHandle() );
		string strTmpUUID = pTmpGold->getProperty(PROP_ENTITY_UUID, "");
		if(strTmpUUID == saveData.strActiveUUID)
		{
			m_hActiveGodAnimal = pTmpGold->getHandle();
		}
	}
}

void GodAnimalSystem::onEventSend2Client(EventArgs& args)
{
	// 通知客户端信息
	GSProto::Cmd_Sc_GoldAnimalList scMsg;
	scMsg.set_dwactiveobjectid(m_hActiveGodAnimal);

	for(size_t i = 0; i < m_godAnimalList.size(); i++)
	{
		GSProto::GoldAnimalRoleBase* pGodAnimaRoleBase = scMsg.mutable_szgodanimallist()->Add();
		fillGoldAnimalRoleBase(m_godAnimalList[i], *pGodAnimaRoleBase);
	}

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_LIST, scMsg);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(pkg);
}

void GodAnimalSystem::fillGoldAnimalRoleBase(HEntity hGodAnimal, GSProto::GoldAnimalRoleBase& baseInfo)
{
	IEntity* pGodAnimal = getEntityFromHandle(hGodAnimal);
	assert(pGodAnimal);

	int iBaseID = pGodAnimal->getProperty(PROP_ENTITY_BASEID, 0);
	int iLevel = pGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	int iLevelStep = pGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	baseInfo.set_dwobjectid(hGodAnimal);
	baseInfo.set_ibaseid(iBaseID);
	baseInfo.set_ilevel(iLevel);
	baseInfo.set_ilevelstep(iLevelStep);
}


bool GodAnimalSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& GodAnimalSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_GODANIMAL_TRAIN);
		resultList.push_back(GSProto::CMD_GODANIMAL_ACTIVE);
		resultList.push_back(GSProto::CMD_GODANIMAL_QUERYLEVELSTEP);
		resultList.push_back(GSProto::CMD_GODANIMAL_EXELEVELSTEPUP);
		resultList.push_back(GSProto::CMD_GODANIMAL_QUERYINHERIT);
		resultList.push_back(GSProto::CMD_GODANIMAL_EXEINHERIT);
		resultList.push_back(GSProto::CMD_GODANIMAL_DETAIL);
		resultList.push_back(GSProto::CMD_GODANIMAL_DETAIL_BYID);
		resultList.push_back(GSProto::CMD_GODANIMAL_ANIMALSOUL_QUERY);
		resultList.push_back(GSProto::CMD_GODANIMAL_ANIMALSOUL_ACTIVE);
		resultList.push_back(GSProto::CMD_GODANIMAL_ANIMALSOUL_SELL);
		resultList.push_back(GSProto::CMD_GODANIMAL_EMPTY);
	}
	
	return resultList;
}

void GodAnimalSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd() )
	{
		case GSProto::CMD_GODANIMAL_TRAIN:
			onReqTrainGodAnimal(msg);
			break;

		case GSProto::CMD_GODANIMAL_ACTIVE:
			onReqSetActive(msg);
			break;

		case GSProto::CMD_GODANIMAL_QUERYLEVELSTEP:
			onQueryGodAnimalLevelStep(msg);
		break;

		case GSProto::CMD_GODANIMAL_EXELEVELSTEPUP:
			onExeGodAnimalLevelStepUp(msg);
			break;

		case GSProto::CMD_GODANIMAL_QUERYINHERIT:
			onQueryGodAnimalInherit(msg);
			break;

		case GSProto::CMD_GODANIMAL_EXEINHERIT:
			onExeGodAnimalInherit(msg);
			break;

		case GSProto::CMD_GODANIMAL_DETAIL:
			onQueryGodAnimalDetail(msg);
			break;

		case GSProto::CMD_GODANIMAL_DETAIL_BYID:
			onReqQueryDetailByID(msg);
			break;

		case GSProto::CMD_GODANIMAL_ANIMALSOUL_QUERY:
			onReqQueryAnimalSoul(msg);
			break;

		case GSProto::CMD_GODANIMAL_ANIMALSOUL_ACTIVE:
			onReqActiveAnimal(msg);
			break;
		case GSProto::CMD_GODANIMAL_ANIMALSOUL_SELL:
			onReqSellAnimal(msg);
			break;
		case GSProto::CMD_GODANIMAL_EMPTY:
			{
				IEntity *pEntity = getEntityFromHandle(m_hEntity);
				assert(pEntity);
				pEntity->sendMessage(GSProto::CMD_GODANIMAL_EMPTY);
			}break;
	}
}

void GodAnimalSystem::onReqQueryAnimalSoul(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GODANIMAL_ANIMALSOUL_QUERY_SC s2cMsg;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	for(size_t i = 0; i < m_godAnimalList.size(); ++i)
	{
		GSProto::GodAnimalSoulUnit &unit = *s2cMsg.add_szunit();

		HEntity hEntity = m_godAnimalList[i];
		IEntity* pGodEntity = getEntityFromHandle(hEntity);
		assert(pGodEntity);

		int iGodAnimalID = pGodEntity->getProperty(PROP_ENTITY_BASEID,0);
		
		unit.set_ianimalsoulid(iGodAnimalID);
		
		int iChangeItemID =  pGodEntity->getProperty(PROP_GODANIMALSOUL_ITEMID, 0);

		if(0 == iChangeItemID)
		{
			continue;
		}
		
		IItemFactory* pIItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory,IID_IItemFactory);
		assert(pIItemFactory);
		
		const PropertySet* pItemPropset = pIItemFactory->getItemPropset(iChangeItemID);
		if(!pItemPropset) ;
	
		int saleType = pItemPropset->getInt(PROP_ITEM_SALETYPE);
		int itemPrice = pItemPropset->getInt(PROP_ITEM_PRICE);
		IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
		assert(pItemSystem);

		int actorHaveCount = pItemSystem->getItemCount(iChangeItemID);
		int price =  itemPrice* actorHaveCount;

		GSProto::PropItem& item = *unit.mutable_price();

		item.set_ilifeattid(saleType);

		item.set_ivalue(price);
	}

	pEntity->sendMessage(GSProto::CMD_GODANIMAL_ANIMALSOUL_QUERY, s2cMsg);
}

void GodAnimalSystem::onReqActiveAnimal(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GODANIMAL_ANIMALSOUL_ACTIVE_CS c2sMsg;

	if(! c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iGodAnimalId = c2sMsg.ianimalsoulid();

	ITable* pGodAnimalTb = getCompomentObjectManager()->findTable("GodAnimal");
	assert(pGodAnimalTb);
	int iRecord = pGodAnimalTb->findRecord(iGodAnimalId);
	if(iRecord < 0)
	{
		return;
	}

	int iChangeItemID = pGodAnimalTb->getInt(iRecord,"兑换ItemID");
	int iChangeItemCount = pGodAnimalTb->getInt(iRecord,"兑换数量");
	int iNeedSilverCount = pGodAnimalTb->getInt( iRecord,"激活花费");

	//没有魂魄的神兽 item == 0
	if(iChangeItemID == 0)
	{
		return;
	}
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	int actorHaveCount = pItemSystem->getItemCount(iChangeItemID);
	if(iChangeItemCount > actorHaveCount )
	{
		pEntity->sendErrorCode(ERROR_GODANIML_ACTIVE_NOITEM);
		return;
	}

	//激活 ,只能有一个可以激活
	for(size_t i = 0; i < m_godAnimalList.size(); ++i)
	{
		HEntity hEntity = m_godAnimalList[i];
		IEntity* pGodEntity = getEntityFromHandle(hEntity);
		assert(pGodEntity);
		int  id =  pGodEntity->getProperty(PROP_ENTITY_BASEID,0);
		if(id == iGodAnimalId )
		{
			return;
		}
	}
	//判断 钱是否足够 ，扣除钱币

	int iActorHaveSilver = pEntity->getProperty(PROP_ACTOR_SILVER , 0);
	if( iNeedSilverCount > iActorHaveSilver )
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}
	
	//扣钱
	pEntity->changeProperty( PROP_ACTOR_SILVER,0-iNeedSilverCount, GSProto::en_Reason_GodAnimalActive);
	PLAYERLOG(pEntity)<<"Active GodAnimalSoul|"<<iNeedSilverCount<<"|Use Silver"<<endl;

	//扣除item
	bool resItem = pItemSystem->removeItem(iChangeItemID,iChangeItemCount, GSProto::en_Reason_GodAnimalActive);
	if(!resItem)
	{
		return;
	}
	PLAYERLOG(pEntity)<<"Active GodAnimalSoul|"<<iChangeItemID<<"|USe Item "<< iChangeItemCount<<endl;

	//是否玩家已经有了这个兽
	bool res = addGodAnimal(iGodAnimalId,true,0);
	if(!res)
	{
		pItemSystem->addItem(iChangeItemID,iChangeItemCount, GSProto::en_Reason_GodAnimalActive);
		PLAYERLOG(pEntity)<<"Active GodAnimalSoul addGodAnimal False|"<<iChangeItemID<<"|Retuen Item "<< iChangeItemCount<<endl;
	}
	pEntity->sendMessage(GSProto::CMD_GODANIMAL_ANIMALSOUL_ACTIVE);
}

void GodAnimalSystem::onReqSellAnimal(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GODANIMAL_ANIMALSOUL_SELL_CS c2sMsg;

	if(! c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iGodAnimalId = c2sMsg.ianimalsoulid();
	
	ITable* pGodAnimalTb = getCompomentObjectManager()->findTable("GodAnimal");
	assert(pGodAnimalTb);
	int iRecord = pGodAnimalTb->findRecord(iGodAnimalId);
	if(iRecord < 0)
	{
		return;
	}

	int iChangeItemID = pGodAnimalTb->getInt(iRecord,"兑换ItemID");
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	if(iChangeItemID == 0)
	{
		return ;
	}
	
	//没有激活的不能买

	bool bIsActive = false;
	for(size_t i = 0; i < m_godAnimalList.size(); ++i)
	{
		HEntity hEntity = m_godAnimalList[i];
		IEntity* pGodEntity = getEntityFromHandle(hEntity);
		assert(pGodEntity);
		int  id =  pGodEntity->getProperty(PROP_ENTITY_BASEID,0);
		if(id == iGodAnimalId )
		{
			bIsActive = true;
			break;
		}
	}
	if(!bIsActive) return;

	IItemFactory* pIItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory,IID_IItemFactory);
	assert(pIItemFactory);
	
	const PropertySet* pItemPropset = pIItemFactory->getItemPropset(iChangeItemID);
	if(!pItemPropset) return ;
	
	int saleType = pItemPropset->getInt(PROP_ITEM_SALETYPE);
	int itemPrice = pItemPropset->getInt(PROP_ITEM_PRICE);
	
	IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);
	int actorHaveCount = pItemSystem->getItemCount(iChangeItemID);
	int price =  itemPrice* actorHaveCount;

	int lifeatt = LifeAtt2Prop(saleType);
	assert(lifeatt>0);
	
	//扣除物品
	bool bResRemove = pItemSystem->removeItem(iChangeItemID,actorHaveCount, GSProto::en_Reason_SELLGODANIMALSOUL);
	if(!bResRemove) return;
	
	if(lifeatt == PROP_ENTITY_EXP)
	{
		pEntity->addExp(price);
	}
	else if(lifeatt == PROP_ACTOR_SILVER)
	{
		pEntity->changeProperty( lifeatt,price,GSProto::en_Reason_SELLGODANIMALSOUL);
	}
	else
	{
		assert(0);
	}
	
	pEntity->sendMessage(GSProto::CMD_GODANIMAL_ANIMALSOUL_SELL);
}


void GodAnimalSystem::onReqQueryDetailByID(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GODANIMAL_DETAIL_BYID_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	int iGoldAnimalID = req.igodanimalid();
	
	ServerEngine::RoleSaveData data;
	if(!fillGodAnimalData(iGoldAnimalID, data) )
	{
		return;
	}

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);

	string strData = ServerEngine::JceToStr(data);
	IEntity* pNewGodAnimal = pEntityFactory->createEntity("GodAnimal", strData);

	GSProto::CMD_GODANIMAL_DETAIL_BYID_SC scMsg;
	scMsg.set_igodanimalid(iGoldAnimalID);

	fillGoldAnimalDetail(pNewGodAnimal->getHandle(), *scMsg.mutable_detail(), true);
	delete pNewGodAnimal;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_GODANIMAL_DETAIL_BYID, scMsg);
}

void GodAnimalSystem::onQueryGodAnimalDetail(const GSProto::CSMessage& msg)
{
	GSProto::Cmd_Cs_QueryGodAnimalDetai reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	HEntity hGodAnimal = reqMsg.dwgodanimalobjectid();

	IEntity* pGodAnimal = getEntityFromHandle(hGodAnimal);
	if(!pGodAnimal) return;

	if((Uint32)pGodAnimal->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return;
	}

	GSProto::Cmd_Sc_QueryGodAnimalDetai scMsg;
	fillGoldAnimalDetail(hGodAnimal, *scMsg.mutable_detail(), true);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_DETAIL, scMsg);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(pkg);
}


bool GodAnimalSystem::checkInheritValid(HEntity hPassGodAnimal, HEntity hAcceptGodAnimal)
{
	if(hPassGodAnimal == hAcceptGodAnimal) return false;

	IEntity* pPassGodAnimal = getEntityFromHandle(hPassGodAnimal);
	IEntity* pAcceptGodAnimal = getEntityFromHandle(hAcceptGodAnimal);

	if(!pPassGodAnimal || !pAcceptGodAnimal)
	{
		return false;
	}

	if( (Uint32)pPassGodAnimal->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return false;
	}

	if( (Uint32)pAcceptGodAnimal->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return false;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iPassLevel = pPassGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	//int iPassLevelStep = pPassGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	int iAcceptLevel = pAcceptGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	//int iAcceptLevelStep = pAcceptGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	// 传承者必须有一项比被传承的大
	if(iPassLevel <= iAcceptLevel)
	{
		pEntity->sendErrorCode(ERROR_GODANIMAL_INVALID_INHERIT);
		return false;
	}

	return true;
}

void GodAnimalSystem::onQueryGodAnimalInherit(const GSProto::CSMessage& msg)
{
	GSProto::Cmd_Cs_QueryGodAnimalInherit reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	HEntity hAcceptGodAnimal = reqMsg.dwaccepobjectid();
	// 如果passer不存在
	if(!reqMsg.has_dwpasserobjectid() )
	{
		IEntity* pAcceptGodAnimal = getEntityFromHandle(hAcceptGodAnimal);
		if(!pAcceptGodAnimal) return;

		GSProto::Cmd_Sc_QueryGodAnimalInherit scMsg;
		fillGoldAnimalDetail(hAcceptGodAnimal, *scMsg.mutable_acceptpredetail(), true);
		pEntity->sendMessage(GSProto::CMD_GODANIMAL_QUERYINHERIT, scMsg);
		return;
	}
	
	HEntity hPassGodAnimal = reqMsg.dwpasserobjectid();
	if(!checkInheritValid(hPassGodAnimal, hAcceptGodAnimal) )
	{
		return;
	}

	IEntity* pPassGodAnimal = getEntityFromHandle(hPassGodAnimal);
	IEntity* pAcceptGodAnimal = getEntityFromHandle(hAcceptGodAnimal);
	assert(pPassGodAnimal && pAcceptGodAnimal);

	int iPassLevel = pPassGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	//int iPassLevelStep = pPassGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	int iAcceptLevel = pAcceptGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	//int iAcceptLevelStep = pAcceptGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	GSProto::Cmd_Sc_QueryGodAnimalInherit scMsg;
	fillGoldAnimalDetail(hPassGodAnimal, *scMsg.mutable_passerdetail(), true);
	fillGoldAnimalDetail(hAcceptGodAnimal, *scMsg.mutable_acceptpredetail(), true);

	int iResultLevel = std::max(iPassLevel, iAcceptLevel);
	//int iResultLevelStep = std::max(iPassLevelStep, iAcceptLevelStep);

	int tempLevel = iPassLevel - iAcceptLevel;
	IGlobalCfg* pConfig = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pConfig);
	int baseCost = pConfig->getInt( "神兽传承消耗基数", 3000);
	int silverCost = baseCost * tempLevel;
	scMsg.set_iinheritsilvercost( silverCost);
	
	ServerEngine::RoleSaveData tmpData;
	pAcceptGodAnimal->packSaveData(tmpData);

	tmpData.basePropData.roleIntPropset[PROP_ENTITY_LEVEL] = iResultLevel;
	//tmpData.basePropData.roleIntPropset[PROP_ENTITY_LEVELSTEP] = iResultLevelStep;

	string strTmpData = ServerEngine::JceToStr(tmpData);

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);

	IEntity* pTmpGodAnimal = pEntityFactory->createEntity("GodAnimal", strTmpData);
	assert(pTmpGodAnimal);

	fillGoldAnimalDetail(pTmpGodAnimal->getHandle(), *scMsg.mutable_acceptpostdetail(), true);
	scMsg.mutable_acceptpostdetail()->set_dwobjectid(hAcceptGodAnimal);
	delete pTmpGodAnimal;
	

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_QUERYINHERIT, scMsg);

	pEntity->sendMessage(pkg);
}

void GodAnimalSystem::onExeGodAnimalInherit(const GSProto::CSMessage& msg)
{
	GSProto::Cmd_Cs_ExeGodAnimalInherit reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	HEntity hPassGodAnimal = reqMsg.dwpasserobjectid();
	HEntity hAcceptGodAnimal = reqMsg.dwaccepobjectid();

	if(!checkInheritValid(hPassGodAnimal, hAcceptGodAnimal) )
	{
		return;
	}

	IEntity* pPassGodAnimal = getEntityFromHandle(hPassGodAnimal);
	IEntity* pAcceptGodAnimal = getEntityFromHandle(hAcceptGodAnimal);
	assert(pPassGodAnimal && pAcceptGodAnimal);

	int iPassLevel = pPassGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	//int iPassLevelStep = pPassGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	int iAcceptLevel = pAcceptGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	//int iAcceptLevelStep = pAcceptGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	int iResultLevel = std::max(iPassLevel, iAcceptLevel);
	//int iResultLevelStep = std::max(iPassLevelStep, iAcceptLevelStep);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int tempLevel = iPassLevel - iAcceptLevel;
	IGlobalCfg* pConfig = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pConfig);
	int baseCost = pConfig->getInt( "神兽传承消耗基数", 3000);
	int silverCost = baseCost * tempLevel;
	int iActorSilver = pEntity->getProperty( PROP_ACTOR_SILVER, 0);
	if(silverCost > iActorSilver)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	//先扣钱
	pEntity->changeProperty( PROP_ACTOR_SILVER, 0-silverCost, GSProto::en_Reason_Inherit_Cost);
	PLAYERLOG(pEntity)<<"GodAnimal Inherit cost|"<<silverCost<<endl;
		
	pAcceptGodAnimal->setProperty(PROP_ENTITY_LEVEL, iResultLevel);
	//pAcceptGodAnimal->setProperty(PROP_ENTITY_LEVELSTEP, iResultLevelStep);

	// 触发事件
	/*if(iAcceptLevelStep != iResultLevelStep)
	{
		EventArgs args;
		args.context.setInt("entity", hAcceptGodAnimal);
		args.context.setInt("oldlevelstep", iAcceptLevelStep);
		args.context.setInt("newlevelstep", iResultLevelStep);
		pAcceptGodAnimal->getEventServer()->setEvent(EVENT_ENTITY_CALCGROW, args);
		pAcceptGodAnimal->getEventServer()->setEvent(EVENT_ENTITY_LEVELSTEPCHG, args);
	}*/

	pPassGodAnimal->setProperty(PROP_ENTITY_LEVEL, 1);
	//pPassGodAnimal->setProperty(PROP_ENTITY_LEVELSTEP, 0);

	// 触发事件
	/*{
		EventArgs args;
		args.context.setInt("entity", hPassGodAnimal);
		args.context.setInt("oldlevelstep", iPassLevelStep);
		args.context.setInt("newlevelstep", 1);
		pPassGodAnimal->getEventServer()->setEvent(EVENT_ENTITY_CALCGROW, args);
		pPassGodAnimal->getEventServer()->setEvent(EVENT_ENTITY_LEVELSTEPCHG, args);
	}*/

	sendGoldAnimalUpdate(hAcceptGodAnimal);
	sendGoldAnimalUpdate(hPassGodAnimal);

	GSProto::Cmd_Sc_ExeGodAnimalInherit scMsg;
	fillGoldAnimalDetail(hPassGodAnimal, *scMsg.mutable_passerdetail(), true);
	fillGoldAnimalDetail(hAcceptGodAnimal, *scMsg.mutable_acceptdetail(), true);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_EXEINHERIT, scMsg);

	pEntity->sendMessage(pkg);

	GODANIMALLOG(pAcceptGodAnimal)<<"InheritAccept"<<endl;
	GODANIMALLOG(pPassGodAnimal)<<"InheritPass"<<endl;
}

void GodAnimalSystem::onExeGodAnimalLevelStepUp(const GSProto::CSMessage& msg)
{
	GSProto::Cmd_Cs_ExeLevelStepUp reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	HEntity hGodAnimal = reqMsg.dwobjectid();
	IEntity* pGodAnimal = getEntityFromHandle(hGodAnimal);
	if(!pGodAnimal) return;

	if( (Uint32)pGodAnimal->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return;
	}

	int iCurLevelStep = pGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	// 验证材料是否足够(代码很猥琐~~)
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IItemSystem* pItemSys = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSys);
	
	ITable* pGodAnimalLevelStep = getCompomentObjectManager()->findTable(TABLENAME_GodAnimalLevelStep);
	assert(pGodAnimalLevelStep);

	Int32 iRecord = pGodAnimalLevelStep->findRecord(iCurLevelStep);
	assert(iRecord >= 0);
	
	int iItemCount = pGodAnimalLevelStep->getInt(iRecord, "消耗材料数");
	assert(iItemCount >= 0);

	int iItemId = pGodAnimal->getProperty(PROP_GODANIMALSOUL_ITEMID,0);

	if(pItemSys->getItemCount( iItemId) < iItemCount)
	{
		pEntity->sendErrorCode(ERROR_NEED_ITEM);
		return;
	}


	// 验证是否达到等阶上限了
	if(pGodAnimalLevelStep->findRecord(iCurLevelStep + 1) < 0)
	{
		pEntity->sendErrorCode(ERROR_GODANIMAL_MAXLEVELSTEP);
		return;
	}

	//扣除钱币 add by hyf
	int iNeedSilver = pGodAnimalLevelStep->getInt( iRecord,"升阶消耗");
	assert(iNeedSilver >= 0);
	int iActorSilver = pEntity->getProperty( PROP_ACTOR_SILVER, 0);
	if(iActorSilver < iNeedSilver)
	{
		pEntity->sendErrorCode( ERROR_NEED_SILVER);
		return ;
	}
	pEntity->changeProperty( PROP_ACTOR_SILVER, 0-iNeedSilver, GSProto::en_Reason_GodAnimalLevelStep);
	PLAYERLOG(pEntity)<<"God_LevelStepUp |" << iNeedSilver << "|Use Silver"<<endl;

	// 扣除物品
	if(!pItemSys->removeItem( iItemId, iItemCount, GSProto::en_Reason_GodAnimalLevelStep))
	{
		GODANIMALLOG(pGodAnimal)<<"LevelStepUp|RemoveItemFail|"<<iItemId<<"|"<<iItemCount<<endl;
	}
	
	GSProto::CMD_GODANIMAL_EXELEVELSTEPUP_SC  scMsg;
	GSProto::GodAnimalLevelStepState& oldState = *scMsg.mutable_oldstate();
	oldState.set_igodanimallevelstep(iCurLevelStep);
	oldState.set_ihpgrow( pGodAnimal->getProperty(PROP_ENTITY_HPGROW, 0) );
	oldState.set_iattgrow( pGodAnimal->getProperty(PROP_ENTITY_ATTGROW, 0));

	int iOldMaxHP = pGodAnimal->getProperty(PROP_ENTITY_MAXHP, 0);
	int iOldAtt = pGodAnimal->getProperty(PROP_ENTITY_ATT, 0);
	// 升阶，调整成长
	pGodAnimal->setProperty(PROP_ENTITY_LEVELSTEP, iCurLevelStep+1);
	
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("oldlevelstep", iCurLevelStep);
	args.context.setInt("newlevelstep", iCurLevelStep+1);
	pGodAnimal->getEventServer()->setEvent(EVENT_ENTITY_CALCGROW, args);
	pGodAnimal->getEventServer()->setEvent(EVENT_ENTITY_LEVELSTEPCHG, args);

	GSProto::GodAnimalLevelStepState& newState = *scMsg.mutable_newstate();
	newState.set_igodanimallevelstep(iCurLevelStep+1);
	newState.set_ihpgrow( pGodAnimal->getProperty(PROP_ENTITY_HPGROW, 0) );
	newState.set_iattgrow( pGodAnimal->getProperty(PROP_ENTITY_ATTGROW, 0));
	
	int iAddMaxHP = pGodAnimal->getProperty(PROP_ENTITY_MAXHP, 0) - iOldMaxHP;
	int iAddAtt = pGodAnimal->getProperty(PROP_ENTITY_ATT, 0) - iOldAtt;
	scMsg.set_ihpaddvalue(iAddMaxHP);
	scMsg.set_iattaddvalue(iAddAtt);
	scMsg.set_dwnewgodanimalobjectid(pGodAnimal->getHandle());
	
	// 通知客户端
	pEntity->sendMessage(GSProto::CMD_GODANIMAL_EXELEVELSTEPUP,scMsg);

	sendGodAnimalLevelStepInfo(hGodAnimal);
	sendGoldAnimalUpdate(hGodAnimal);
	// 广播
	int iGodAnimalID = pGodAnimal->getProperty(PROP_ENTITY_BASEID, 0);
	notifyLevelStepUp(iGodAnimalID, iCurLevelStep, iCurLevelStep+1);

	GODANIMALLOG(pGodAnimal)<<"LevelStepUp|"<<iCurLevelStep<<"|"<<(iCurLevelStep+1)<<endl;
}



void GodAnimalSystem::notifyLevelStepUp(int iGodAnimalID, int iPreLevelStep, int iCurLevelStep)
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strNotifyLevelStep = pGlobalCfg->getString("神兽升阶提示", "30#40");
	vector<int> hitList = TC_Common::sepstr<int>(strNotifyLevelStep, "#");

	if(find(hitList.begin(), hitList.end(), iCurLevelStep) == hitList.end() )
	{
		return;
	}

	GSProto::CMD_MARQUEE_SC scMsg;
	scMsg.set_marqueeid(GSProto::en_marQueue_GodAnimalLevelStep);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	scMsg.add_szstrmessage(strActorName);

	// 升阶前
	{
		stringstream ss;
		ss<<iGodAnimalID<<"#"<<iPreLevelStep;
		scMsg.add_szstrmessage(ss.str() );
	}

	// 升阶后
	{
		stringstream ss;
		ss<<iGodAnimalID<<"#"<<iCurLevelStep;
		scMsg.add_szstrmessage(ss.str() );
	}

	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_MARQUEE, scMsg);
	
	pMsgLayer->broadcastMsg(pkg);
}

void GodAnimalSystem::sendGodAnimalLevelStepInfo(HEntity hGodAnimal)
{
	IEntity* pGodAnimal = getEntityFromHandle(hGodAnimal);
	if(!pGodAnimal) return;

	if( (Uint32)pGodAnimal->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::Cmd_Sc_QueryGodAnimalLevelStep scMsg;
	scMsg.set_dwobjectid(hGodAnimal);

	ITable* pGodAnimalLevelStep = getCompomentObjectManager()->findTable(TABLENAME_GodAnimalLevelStep);
	assert(pGodAnimalLevelStep);

	int iCurLevelStep = pGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	int iRecord = pGodAnimalLevelStep->findRecord(iCurLevelStep + 1);
	if(iRecord < 0)
	{
		scMsg.set_btouchmaxlv(true);
		fillGodAnimalLevelStep(hGodAnimal, scMsg.mutable_curinfo() );

		GSProto::SCMessage pkg;
		HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_QUERYLEVELSTEP, scMsg);
		pEntity->sendMessage(pkg);
		return;
	}

	scMsg.set_btouchmaxlv(false);
	fillGodAnimalLevelStep(hGodAnimal, scMsg.mutable_curinfo() );

	// 填充下一级别数据(临时处理,创建个新的GodAnimal 然后打包数据)
	//IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	//assert(pEntityFactory);
	
	//ServerEngine::RoleSaveData tmpGodAnimalSaveData;
	//pGodAnimal->packSaveData(tmpGodAnimalSaveData);
	//tmpGodAnimalSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVELSTEP] = iCurLevelStep + 1;
	//string strTmpSaveString = ServerEngine::JceToStr(tmpGodAnimalSaveData);
	
	//IEntity* pTmpGodAnimal = pEntityFactory->createEntity("GodAnimal", strTmpSaveString);
	//assert(pTmpGodAnimal);

	//fillGodAnimalLevelStep(pTmpGodAnimal->getHandle(), scMsg.mutable_nextinfo() );
	//delete pTmpGodAnimal;

	// 填充
	fillLevelStepUpConsume(hGodAnimal,iCurLevelStep, scMsg.mutable_szconsumeitem() );

	int iCurRecord = pGodAnimalLevelStep->findRecord( iCurLevelStep);
	int iNeedSilver = pGodAnimalLevelStep->getInt( iCurRecord,"升阶消耗");
	scMsg.set_ilevelstepsilvercost( iNeedSilver);
	
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_QUERYLEVELSTEP, scMsg);

	pEntity->sendMessage(pkg);
}

void GodAnimalSystem::fillGodAnimalLevelStep(HEntity hGodAnimal, GSProto::GoldAnimalLevelStepInfo* pLevelStepInfo)
{
	IEntity* pGodAnimal = getEntityFromHandle(hGodAnimal);
	assert(pGodAnimal);

	// 填充属性
	static int s_GoldAnimalProp[] = {PROP_ENTITY_MAXHP, PROP_ENTITY_ATT, PROP_ENTITY_HIT, PROP_ENTITY_KNOCK,};

	for(size_t i = 0; i < sizeof(s_GoldAnimalProp)/sizeof(s_GoldAnimalProp[0]); i++)
	{
		int iPropID = s_GoldAnimalProp[i];
		int iLifeAttID = Prop2LifeAtt(iPropID);
		assert(iLifeAttID >= 0);
		int iValue = pGodAnimal->getProperty(iPropID, 0);

		GSProto::PropItem* pNewPropItem = pLevelStepInfo->mutable_szproplist()->Add();
		pNewPropItem->set_ilifeattid(iLifeAttID);
		pNewPropItem->set_ivalue(iValue);
	}

	// 技能
	ISkillSystem* pSkillSys = static_cast<ISkillSystem*>(pGodAnimal->querySubsystem(IID_ISkillSystem));
	assert(pSkillSys);

	int iActiveSkillID = pSkillSys->getSelectedActiveSkill();
	int iStageSkillID = pSkillSys->getStageSkill();

	pLevelStepInfo->set_iactiveskillid(iActiveSkillID);
	pLevelStepInfo->set_istageskillid(iStageSkillID);

	// 当前等阶
	int iLevelStep = pGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	pLevelStepInfo->set_ilevelstep(iLevelStep);

	// 等级上限
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	int iLimitLevel = pJZEntityFactory->getGodAnimalLevelLimit(iLevelStep);
	pLevelStepInfo->set_ilevellimit(iLimitLevel);


	int iFightValue = pGodAnimal->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	pLevelStepInfo->set_ifightvalue(iFightValue);
}

void GodAnimalSystem::fillLevelStepUpConsume(HEntity hGodAnimal,int iLevelStep, google::protobuf::RepeatedPtrField< ::GSProto::ConsumeItem>* pszConsume)
{
	ITable* pGodAnimalLevelStep = getCompomentObjectManager()->findTable(TABLENAME_GodAnimalLevelStep);
	assert(pGodAnimalLevelStep);

	Int32 iRecord = pGodAnimalLevelStep->findRecord(iLevelStep);
	assert(iRecord >= 0);

	int iItemCount = pGodAnimalLevelStep->getInt(iRecord, "消耗材料数");
	assert(iItemCount >= 0);

	IEntity *pGodAnimal = getEntityFromHandle( hGodAnimal);
	assert(pGodAnimal);
	int iItemId = pGodAnimal->getProperty(PROP_GODANIMALSOUL_ITEMID,0);
	GSProto::ConsumeItem* pNewConsumeItem = pszConsume->Add();
	pNewConsumeItem->set_iitemid(iItemId);
	pNewConsumeItem->set_ineedcount( iItemCount);
}

void GodAnimalSystem::onQueryGodAnimalLevelStep(const GSProto::CSMessage& msg)
{
	GSProto::Cmd_Cs_QueryGodAnimalLevelStep reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	HEntity hGodAnimal = reqMsg.dwobjectid();
	sendGodAnimalLevelStepInfo(hGodAnimal);
}

void GodAnimalSystem::onReqSetActive(const GSProto::CSMessage& msg)
{
	GSProto::Cmd_Cs_GoldAnimalActive reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	HEntity hGodAnimal = reqMsg.dwobjectid();
	IEntity* pGoldAnimal = getEntityFromHandle(hGodAnimal);
	if(!pGoldAnimal) return;

	if( (Uint32)pGoldAnimal->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity) return;

	HEntity hOld = m_hActiveGodAnimal;
	
	m_hActiveGodAnimal = hGodAnimal;

	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("old", hOld);
	args.context.setInt("cur", m_hActiveGodAnimal);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_ACTIVEGODANIMAL_CHG, args);

	// 通知客户端
	GSProto::Cmd_Sc_GoldAnimalActive scMsg;
	scMsg.set_dwobjectid(hGodAnimal);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_ACTIVE, scMsg);

	pEntity->sendMessage(pkg);

	GODANIMALLOG(pGoldAnimal)<<"SetFight"<<endl;
}


void GodAnimalSystem::onReqTrainGodAnimal(const GSProto::CSMessage& msg)
{
	GSProto::Cmd_Cs_GodAnimalTrain req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	HEntity hGodAnimal = req.dwobjectid();
	IEntity* pGodAnimal = getEntityFromHandle(hGodAnimal);
	if(!pGodAnimal) return;

	if( (Uint32)pGodAnimal->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity) return;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iNeedSilver = pGlobalCfg->getInt("神兽培养消耗", 1000);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iTkSilver = pEntity->getProperty(PROP_ACTOR_SILVER, 0);
	if(iTkSilver < iNeedSilver)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	// 验证等级是否达到上限
	ITable* pGodAnimalExpTb = getCompomentObjectManager()->findTable("GodAnimalLevelExp");
	assert(pGodAnimalExpTb);

	int iCurLevel = pGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);

	// 神兽等级不能超过人物等级3倍
	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	if(iCurLevel >= iActorLevel*2)
	{
		pEntity->sendErrorCode(ERROR_GODANIMALLEVEL_3ACTORLEVEL);
		return;
	}
	
	int iCurLevelStep = pGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	int iNexLvRecord = pGodAnimalExpTb->findRecord(iCurLevel + 1);
	if(iNexLvRecord < 0)
	{
		pEntity->sendErrorCode(ERROR_GODANIMAL_MAXLEVEL);
		return;
	}

	int iNextLevelStepLimit = pGodAnimalExpTb->getInt(iNexLvRecord, "等阶限制");
	if(iNextLevelStepLimit > iCurLevelStep)
	{
		pEntity->sendErrorCode(ERROR_GODANIMAL_NEEDLEVELSTEP);
		return;
	}

	pEntity->changeProperty(PROP_ACTOR_SILVER, 0-iNeedSilver, GSProto::en_Reason_GoldAnimaTrain);

	int iKnockChance = pGlobalCfg->getInt("神兽培养暴击几率", 3000);
	string strExpRange = pGlobalCfg->getString("神兽培养经验区间", "1000#2000");
	vector<int> rangeList = TC_Common::sepstr<int>(strExpRange, "#");
	assert(rangeList.size() == 2);

	int iMinValue = rangeList[0];
	int iMaxValue = rangeList[1];
	assert(iMaxValue >= iMinValue);

	GSProto::Cmd_Sc_GoldAnimalTrain scMsg;
	scMsg.set_dwobjectid(hGodAnimal);
	scMsg.set_bknock(false);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	int iGetExp = pRandom->random()%(iMaxValue - iMinValue + 1) + iMinValue;
	if( (pRandom->random() % 10000) < iKnockChance)
	{
		scMsg.set_bknock(true);
		iGetExp = iGetExp * 10;
	}
	scMsg.set_igetexp(iGetExp);

	// 添加经验
	int iPreLevel = pGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	pGodAnimal->addExp(iGetExp);
	int iAfterLevel = pGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);

	if(iPreLevel != iAfterLevel)
	{
		EventArgs args;
		args.context.setInt("godAnimalLevel",iAfterLevel);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_GODANIMLLEVEL, args);
	}
	
	fillGoldAnimalDetail(hGodAnimal, *scMsg.mutable_detail(), true);

	// 通知客户端
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_TRAIN, scMsg);
	pEntity->sendMessage(pkg);

	sendGoldAnimalUpdate(hGodAnimal);

	//任务系统
	{
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_GODANIMALTRAIN, args);
	}

	GODANIMALLOG(pGodAnimal)<<"Train|"<<iNeedSilver<<"|"<<iGetExp<<endl;
}

void GodAnimalSystem::fillGoldAnimalDetail(HEntity hGodAnimal, GSProto::GoldAnimalDetail& detailInfo, bool bSetSkill)
{
	IEntity* pGodAnimal = getEntityFromHandle(hGodAnimal);
	assert(pGodAnimal);

	int iLevel = pGodAnimal->getProperty(PROP_ENTITY_LEVEL, 0);
	int iLevelStep = pGodAnimal->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	ITable* pGodAnimalExpTb = getCompomentObjectManager()->findTable(TABLENAME_GodAnimalLevelExp);
	assert(pGodAnimalExpTb);

	int iRecord = pGodAnimalExpTb->findRecord(iLevel);
	assert(iRecord >= 0);

	int iNeedExp = pGodAnimalExpTb->getInt(iRecord, "升级经验");
	int iBaseID = pGodAnimal->getProperty(PROP_ENTITY_BASEID, 0);

	detailInfo.set_dwobjectid(hGodAnimal);
	detailInfo.set_ibaseid(iBaseID);
	detailInfo.set_icurexp(pGodAnimal->getProperty(PROP_ENTITY_EXP, 0) );
	detailInfo.set_ineedexp(iNeedExp);
	detailInfo.set_ifightvalue(pGodAnimal->getProperty(PROP_ENTITY_FIGHTVALUE, 0) );
	detailInfo.set_ilevel(iLevel);
	detailInfo.set_ilevelstep(iLevelStep);

	if(bSetSkill)
	{
		ISkillSystem* pSkillSys = static_cast<ISkillSystem*>(pGodAnimal->querySubsystem(IID_ISkillSystem));
		assert(pSkillSys);

		int iActiveSkillID = pSkillSys->getSelectedActiveSkill();
		int iStageSkillID = pSkillSys->getStageSkill();

		detailInfo.set_iactiveskillid(iActiveSkillID);
		detailInfo.set_istageskillid(iStageSkillID);
	}

	// 填充属性
	static int s_GoldAnimalProp[] = {PROP_ENTITY_MAXHP, PROP_ENTITY_ATT, PROP_ENTITY_HIT, PROP_ENTITY_KNOCK,};

	for(size_t i = 0; i < sizeof(s_GoldAnimalProp)/sizeof(s_GoldAnimalProp[0]); i++)
	{
		int iPropID = s_GoldAnimalProp[i];
		int iLifeAttID = Prop2LifeAtt(iPropID);
		assert(iLifeAttID >= 0);
		int iValue = pGodAnimal->getProperty(iPropID, 0);

		GSProto::PropItem* pNewPropItem = detailInfo.mutable_szproplist()->Add();
		pNewPropItem->set_ilifeattid(iLifeAttID);
		pNewPropItem->set_ivalue(iValue);
	}
}


void GodAnimalSystem::packSaveData(string& data)
{
	ServerEngine::GodAnimalSystemData saveData;
	IEntity* pActiveGodAnimal = getEntityFromHandle(m_hActiveGodAnimal);
	if(pActiveGodAnimal)
	{
		saveData.strActiveUUID = pActiveGodAnimal->getProperty(PROP_ENTITY_UUID, "");
	}
	
	saveData.godAnimalList.resize(m_godAnimalList.size() );
	for(size_t i = 0; i < m_godAnimalList.size(); i++)
	{
		IEntity* pTmpGodAnimal = getEntityFromHandle(m_godAnimalList[i]);
		if(!pTmpGodAnimal) continue;

		pTmpGodAnimal->packSaveData(saveData.godAnimalList[i]);
	}

	data = ServerEngine::JceToStr(saveData);
}

vector<HEntity> GodAnimalSystem::getGodAnimalList()
{
	return m_godAnimalList;
}

HEntity GodAnimalSystem::getActiveGodAnimal()
{
	return m_hActiveGodAnimal;
}

void GodAnimalSystem::autoActiveGodAnimal()
{
	if(m_godAnimalList.size() > 0)
	{
		m_hActiveGodAnimal = m_godAnimalList[0];
	}
}

bool GodAnimalSystem::fillGodAnimalData(int iBaseID, ServerEngine::RoleSaveData& data)
{
	ITable* pGodAnimalTb = getCompomentObjectManager()->findTable("GodAnimal");
	assert(pGodAnimalTb);

	int iRecord = pGodAnimalTb->findRecord(iBaseID);
	if(iRecord < 0) return false;
	

	data.basePropData.roleIntPropset[PROP_ENTITY_BASEID] = iBaseID;
	data.basePropData.roleIntPropset[PROP_ENTITY_LEVEL] = 1;
	data.basePropData.roleIntPropset[PROP_ENTITY_MASTER] = m_hEntity;

	// 设置UUID吧
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	data.basePropData.roleStringPropset[PROP_ENTITY_UUID] = szUUIDString;

	// 填充技能信息
	/*string strSkillList = pGodAnimalTb->getString(iRecord, "技能列表");

	ServerEngine::SkillSystemSaveData tmpSkillSysData;
	tmpSkillSysData.skillList = TC_Common::sepstr<int>(strSkillList, "#");
	
	data.subsystemData[IID_ISkillSystem] = ServerEngine::JceToStr(tmpSkillSysData);*/

	return true;
}


HEntity GodAnimalSystem::addGodAnimal(int iBaseID, bool bNotifyError, int iLevelStep)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 如果有相同ID的神兽在，不允许
	for(size_t i = 0; i < m_godAnimalList.size(); i++)
	{
		HEntity hTmpGodAnimal = m_godAnimalList[i];
		IEntity* pTmpGodAnimal = getEntityFromHandle(hTmpGodAnimal);
		if(!pTmpGodAnimal) continue;

		int iTmpBaseID = pTmpGodAnimal->getProperty(PROP_ENTITY_BASEID, 0);
		if(iTmpBaseID == iBaseID)
		{
			pEntity->sendErrorCode(ERROR_GODANIMAL_SAMEID);
			return false;
		}
	}

	ServerEngine::RoleSaveData data;
	if(!fillGodAnimalData(iBaseID, data) )
	{
		return 0;
	}

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);

	string strData = ServerEngine::JceToStr(data);
	IEntity* pNewGodAnimal = pEntityFactory->createEntity("GodAnimal", strData);
	pNewGodAnimal->setProperty(PROP_ENTITY_LEVELSTEP,iLevelStep);
	m_godAnimalList.push_back(pNewGodAnimal->getHandle() );

	// 通知客户端
	sendGoldAnimalUpdate(pNewGodAnimal->getHandle() );

	// 如果是第一个神兽，默认设置为出战
	if(m_godAnimalList.size() == 1)
	{
		m_hActiveGodAnimal = pNewGodAnimal->getHandle();

		EventArgs args;
		args.context.setInt("entity", m_hEntity);
		args.context.setInt("old", 0);
		args.context.setInt("cur", m_hActiveGodAnimal);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_ACTIVEGODANIMAL_CHG, args);
	}

	GODANIMALLOG(pNewGodAnimal)<<"Add"<<endl;

	return pNewGodAnimal->getHandle();
}

HEntity GodAnimalSystem::addGodAnimalWithLevelStep(int iBaseID, bool bNotifyError, int iLevelStep)
{
	return addGodAnimal(iBaseID,bNotifyError,iLevelStep);
}


void GodAnimalSystem::sendGoldAnimalUpdate(HEntity hGodAnimal)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::Cmd_Sc_GodAnimalUpdate scMsg;
	fillGoldAnimalRoleBase(hGodAnimal, *scMsg.mutable_updateinfo() );

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GODANIMAL_UPDATE, scMsg);

	pEntity->sendMessage(pkg);
}



