#include "ShopSystemPch.h"
#include "ShopSystem.h"
#include "IJZEntityFactory.h"
#include "util/tc_md5.h"
#include "Legion.h"
#include "IJZMessageLayer.h"
#include "ILegionFactory.h"
#include "IOperateSystem.h"
#include "IEquipBagSystem.h"



extern "C" IObject* createShopSystem()
{
	return new ShopSystem;
}
extern int LifeAtt2Prop(int iLifeAtt);

ShopSystem::ShopSystem()
{
}

ShopSystem:: ~ShopSystem()
{	
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	timeComponent->killTimer( m_CheckNoticeTimerHandle);

}

Uint32 ShopSystem::getSubsystemID() const
{
	return IID_IShopSystem;
}

Uint32 ShopSystem::getMasterHandle()
{
	return m_hEntity;
}

bool ShopSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();
	if(strData.size() > 0)
	{
		ServerEngine::JceToObj(strData, m_shopSystemData);
	}
	else
	{
		m_shopSystemData.iTokenRandOnceCount = 0;
		m_shopSystemData.iGoldRandOnceCount = 0;

		IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZoneTime);
		refreshConvertGoods();
		m_shopSystemData.dwRefreshSecond = pZoneTime->GetCurSecond();
		m_shopSystemData.iRefreshTimes  = 0;
		m_shopSystemData.bHaveFirstRand = false;
		m_shopSystemData.bHaveFirstRandTen = false;
		//m_shopSystemData.dwLastGoldFreeSecond = pZoneTime->GetCurSecond();
	}
	
	IGlobalCfg *pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	string iShakeBase = pGlobalCfg->getString("摇钱树参数基础产量及等级除数",  "2#10000#20");
	m_ShakeDataVec = TC_Common::sepstr<int>(iShakeBase, "#");
	assert(m_ShakeDataVec.size() == 3);


	
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	m_CheckNoticeTimerHandle = timeComponent->setTimer(this,1,5*1000,"ShopSystem");
	
	return true;
}

void ShopSystem::onTimer(int nEventId)
{
	if(nEventId == 1)
	{
		checkNotice();
	}
}

void ShopSystem::checkNotice()
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	int iTokenTotalFreeTimes = pGlobal->getInt("商城将星令抽卡免费次数", 5);
	int iFreeInterval =  pGlobal->getInt("免费将星令抽卡时间间隔秒", 300);
	int iGoldFreeInterval =  pGlobal->getInt("免费元宝抽卡时间间隔秒", 172800);
	
	int iRemainFreeTimes = iTokenTotalFreeTimes - m_shopSystemData.tokenFreeValue.iValue ;
	//将星令抽取
	bool bTokenNeedNotice = true;
	if(iRemainFreeTimes <= 0)
	{
		 bTokenNeedNotice = false;
	}
	else
	{
		int iSecond = pZoneTime->GetCurSecond() - m_shopSystemData.dwLastTokenFreeSecond;
		if(iSecond < iFreeInterval)
		{
			 bTokenNeedNotice = false;
		}
	}

	//元宝抽取信息
	bool bGoldNeedNotice = true;
	int iGoldLeaveSecond = pZoneTime->GetCurSecond() - m_shopSystemData.dwLastGoldFreeSecond;
	if(iGoldLeaveSecond < iGoldFreeInterval)
	{
		bGoldNeedNotice = false;
	}

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	if(bGoldNeedNotice || bTokenNeedNotice)
	{
		pEntity->chgNotice(GSProto::en_NoticeGuid_SHOP, true);
	}
	else
	{
		pEntity->chgNotice(GSProto::en_NoticeGuid_SHOP, false);
	}

	int iShowFirstPay = pEntity->getProperty( PROP_ACTOR_SHOWFIRSTPAY,0);
	if(iShowFirstPay != 2)
	{
		if( iShowFirstPay == 1)
		{
			pEntity->chgNotice(GSProto::en_NoticeGuid_FirstPayMent, true);
		}
		else
		{
			pEntity->chgNotice(GSProto::en_NoticeGuid_FirstPayMent, false);
		}
	}
	
	
}

void ShopSystem::refreshConvertGoods()
{
	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	vector < ShopGoodUnit > shopVec;
	pShopFactory->getActorHonorShop(m_hEntity, shopVec);
	for(size_t i = 0; i < shopVec.size(); ++i)
	{
		ShopGoodUnit& unit = shopVec[i];
		ServerEngine::HonorGoodDetail detail;
		detail.iGoodId = unit.iGoodId;
		detail.bIsSale = false;
		m_shopSystemData.szGoodmap.insert(make_pair(i , detail));
	}
}

bool ShopSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& ShopSystem::getSupportMessage()
{
	static std::vector<Uint32> messageList;
	if(0 == messageList.size())
	{
		messageList.push_back(GSProto::CMD_SHOPSYSTEM_RANDITEMORHERO);
		messageList.push_back(GSProto::CMD_SHOPSYSTEM_HONORCONVERT_QUERY);
		messageList.push_back(GSProto::CMD_SHOPSYSTEM_HONORCONVERT_REFRESH);
		messageList.push_back(GSProto::CMD_SHOPSYSTEM_HONORCONVERT_CONVERT);
		messageList.push_back(GSProto::CMD_COINTREE_SHAKE_QUERY);
		messageList.push_back(GSProto::CMD_COINTREE_SHAKE);
		messageList.push_back(GSProto::CMD_SHOPSYSTEM_QUERY);
		messageList.push_back(GSProto::CMD_PAYMENT_FIRST_PAY);
		messageList.push_back(GSProto::CMD_PAYMENT_FIRST_QUERY);
		messageList.push_back(GSProto::CMD_PAYMENT_QUERY);
		messageList.push_back(GSProto::CMD_VIVO_BUY_GOOD);
		messageList.push_back(GSProto::CMD_IOS_BUY_GOODS);

		messageList.push_back(GSProto::CMD_NORMALSHOP_QUERY);
		messageList.push_back(GSProto::CMD_NORMALSHOP_REFRESH);
		messageList.push_back(GSProto::CMD_NORMALSHOP_BUY);
	}
	return messageList;
}

void ShopSystem::onMessage(QxMessage* pMessage)
{
	assert( pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(GSProto::CSMessage*)(pMessage->pMsgDataBuff);
	dayResetFreeRandValue();
	switch(msg.icmd())
	{
		case GSProto::CMD_SHOPSYSTEM_RANDITEMORHERO :
			{
				onRequestRandItemOrHero(msg);
			}break;	
		
		case GSProto::CMD_SHOPSYSTEM_HONORCONVERT_QUERY :
			{
				onQueryHonorConvertInfo(msg);
			}break;
		
		case GSProto::CMD_SHOPSYSTEM_HONORCONVERT_REFRESH :
			{
				onReqRefreshHonorConvert(msg);
			}break;	
		
		case GSProto::CMD_SHOPSYSTEM_HONORCONVERT_CONVERT :
			{
				onReqConvert(msg);
			}break;

		case GSProto::CMD_COINTREE_SHAKE_QUERY :
			{
				checkCoinTreeReset();
				onQueryCoinTreeShake(msg);
			}break;

		case GSProto::CMD_COINTREE_SHAKE : 
			{
				checkCoinTreeReset();
				onCoinTreeShake(msg);
			}break;

		case GSProto::CMD_SHOPSYSTEM_QUERY:
			{
				onQueryShopInfo(msg);
			}
			break;
		
		case GSProto::CMD_PAYMENT_FIRST_PAY:
				onPaymentFirstPayGift(msg);
			break;

		case GSProto::CMD_PAYMENT_FIRST_QUERY:
				onQueryFirstPayGift(msg);
			break;

		case GSProto::CMD_PAYMENT_QUERY:
				onPaymentQuery(msg);
			break;
			
		case GSProto::CMD_VIVO_BUY_GOOD:
				onVIVOPrePay(msg);
			break;
		case GSProto::CMD_IOS_BUY_GOODS:
				onIOSPrePay(msg);
			break;

		case GSProto::CMD_NORMALSHOP_QUERY:
		{
			this->onReqNormalShopQuery(msg);
			break;
		}

		case GSProto::CMD_NORMALSHOP_REFRESH:
		{
			this->onReqNormalShopRefresh(msg);
			break;
		}

		case GSProto::CMD_NORMALSHOP_BUY:
		{
			this->onReqNormalShopBuy(msg);
			break;
		}
	}
}

class GetPrePayInfoCallback : public ServerEngine::LJSDKPrxCallback
{
	public:
		GetPrePayInfoCallback(HEntity hEntity):m_hEntity(hEntity){}
		
		virtual void callback_VIVOPostPerPay( const std::string& strAccessKey,  const std::string& strOrderNumber, taf::Int32 price)
        { 
        	IEntity *pEntity = getEntityFromHandle(m_hEntity);
			if(!pEntity) return;

			GSProto::CMD_VIVO_BUY_GOOD_SC scMsg;
			
			scMsg.set_iprice(price);
			scMsg.set_ordernumber(strOrderNumber);
			scMsg.set_accesskey(strAccessKey);
			scMsg.set_appid("dbc12025f223b66b9e950ed333094daf");

			pEntity->sendMessage(GSProto::CMD_VIVO_BUY_GOOD, scMsg);
        }
		
        virtual void callback_VIVOPostPerPay_exception(taf::Int32 ret)
        { 
        	IEntity *pEntity = getEntityFromHandle(m_hEntity);
			if(!pEntity) return;
			
        	string strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT,"");
			FDLOG("GetPrePayInfoCallback")<<strAccount<<"|" << ret<<endl;
		}

	private:
		HEntity m_hEntity;
};

unsigned char ShopSystem::ToHex(unsigned char x) 
{ 
    return  x > 9 ? x + 55 : x + 48; 
}

std::string ShopSystem::UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) || 
            (str[i] == '-') ||
            (str[i] == '_') || 
            (str[i] == '.') || 
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}



void ShopSystem::onVIVOPrePay(const GSProto::CSMessage& msg)
{

	GSProto::CMD_VIVO_BUY_GOOD_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iPrice = csMsg.iprice()/10;

	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_Payment);
	assert(pTable);

	int iRecord = pTable->findRecord(iPrice);
	if(iRecord < 0) 
	{
		return ;
	}	
	int iamount = iPrice * 100;
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	//组成string
	
	/*signature=
	to_lower_case(
		md5_hex(
		appId=XX&
		cpId=XX&
		cpOrderNumber=XX&
		extInfo=XX&
		notifyUrl=XXX&
		orderAmount=XXX&
		orderDesc=XXX&
		orderTime=XXX&
		orderTitle=XXX&
		version=XXX&
		to_lower_case(
		md5_hex(Cp-key))))
	*/
	string strappId = "dbc12025f223b66b9e950ed333094daf";
	string strCpId = "20140520165101913807";
	string strCpOrderNumber = szUUIDString;
		
	int iWorldId = pEntity->getProperty(PROP_ACTOR_WORLD,0);
	string iAccount = pEntity->getProperty( PROP_ACTOR_ACCOUNT,"");
	string extInfo = TC_I2S(iWorldId);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
	assert(pGlobalCfg);
	string strNotifyUrl =  pGlobalCfg->getGlobalCfg().get("/Zone<notifyUrl>", "");
	assert(strNotifyUrl.size() > 0);

	string strOrderAmount = TC_I2S(iamount);
	
	string strOrderDesc = "buy gold";

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	string strOrderTime = pZoneTime->GetCurVIVOTimeStr();

	string strOrderTitle = "gold";

	string strVersion = "3.0.1";
	string strCpKey = "A08DFB4D5831795785A6505AF256F556";
	string strSignMethod = "MD5";


	string strMd5HexCpKey = "0dc94df00a61c304b7b6ebefee93dc1c";
	

	string strPostContextTemp ="appId="+ strappId +
					 "&cpId=" + strCpId +
					 "&cpOrderNumber=" + strCpOrderNumber+
					 "&extInfo=" + extInfo+
					 "&notifyUrl=" + strNotifyUrl+
					 "&orderAmount=" + strOrderAmount+
					 "&orderDesc=" + strOrderDesc+
					 "&orderTime=" + strOrderTime + 
					 "&orderTitle=" + strOrderTitle + 
					 "&version=" + strVersion;
		
	string strTemp = strPostContextTemp +"&"+strMd5HexCpKey;

	string strTempSignature = TC_MD5::md5str(strTemp);
	//string strSignature = strTempSignature.substr(8,16);
	string strPostContext =   "appId="+ UrlEncode(strappId) +
					 "&cpId=" + UrlEncode(strCpId )+
					 "&cpOrderNumber=" + UrlEncode(strCpOrderNumber)+
					 "&extInfo=" + UrlEncode(extInfo)+
					 "&notifyUrl=" + UrlEncode(strNotifyUrl)+
					 "&orderAmount=" + UrlEncode(strOrderAmount)+
					 "&orderDesc=" + UrlEncode(strOrderDesc)+
					 "&orderTime=" + UrlEncode(strOrderTime) + 
					 "&orderTitle=" + UrlEncode(strOrderTitle) + 
					 "&version=" + UrlEncode(strVersion)+
					 "&signMethod=" +UrlEncode(strSignMethod) + 
					 "&signature=" + UrlEncode(strTempSignature);
	
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	pJZMessageLayer->AsyncGetPrePayInfo(new GetPrePayInfoCallback(m_hEntity), strPostContext);
}



/**
 * IOS Pay callback
 */
class IOSPayCallback : public ServerEngine::LJSDKPrxCallback
{
	public:
		IOSPayCallback(HEntity hEntity):m_hEntity(hEntity){}

	 	virtual void callback_iosPay(	const std::string& stransID,  
											const std::string& productID, 
											taf::Int32 sPrice, 
											taf::Int32 quantity, 
											ServerEngine::IOSVerifyRetcode retCode)
	 	{
	 		// get some addition params
	 		IEntity *pEntity = getEntityFromHandle(m_hEntity);
			if(!pEntity) return;
			int worldID = pEntity->getProperty(PROP_ACTOR_WORLD,0);
			string account = pEntity->getProperty(PROP_ACTOR_ACCOUNT,"");
			
	 		// pay yanbao to player
	 		bool isSuccess = false;
			int ybAmount = sPrice * 10 * quantity;
	 		if(retCode == ServerEngine::en_IOS_VERIFY_OK)
	 		{
				IShopSystem *pShopSystem = static_cast<IShopSystem*>(pEntity->querySubsystem(IID_IShopSystem));
				assert(pShopSystem);
				isSuccess = pShopSystem->addGoldForPayment(ybAmount, "ios_appstore", stransID);
	 		}

			// log
			FDLOG("IOS_PAY_RESULT")	<< worldID 	<< 	"|"
									<< account 	<< 	"|"
									<< productID<<	"|"
									<< stransID <<	"|"
									<< retCode	<<	"|"
									<< sPrice 	<< 	"|"
									<< quantity	<<	"|"
									<< ybAmount	<<	"|"
									<< isSuccess<<	


endl;

			// send message back to client
			GSProto::CMD_IOS_BUY_GOODS_SC scMsg;
			scMsg.set_iprice(sPrice);
			if(isSuccess){
				scMsg.set_errorcode(0);
			}else{
				scMsg.set_errorcode(1);
			}
			pEntity->sendMessage(GSProto::CMD_IOS_BUY_GOODS, scMsg);
			
	 	}
   		virtual void callback_iosPay_exception(taf::Int32 ret)
   		{
   			

			IEntity *pEntity = getEntityFromHandle(m_hEntity);
			if(!pEntity) return;
			
        	string account = pEntity->getProperty(PROP_ACTOR_ACCOUNT,"");
			FDLOG("IOS_PAY_EXCEPTION")	<< account <<"|"
										<< ret << endl;
   		}

	private:
		HEntity m_hEntity;
};



void ShopSystem::onIOSPrePay(const GSProto::CSMessage& _msg)
{
	// collect params
	GSProto::CMD_IOS_BUY_GOODS_CS csMsg;
	if(!csMsg.ParseFromString(_msg.strmsgbody()))
	{
		return;
	}
	int price = csMsg.iprice() / 10;
	const string& receipt = csMsg.receipt();

	// collect other params
	IEntity *entity = getEntityFromHandle(m_hEntity);
	assert(entity);
	int worldID = entity->getProperty(PROP_ACTOR_WORLD,0);
	string account = entity->getProperty( PROP_ACTOR_ACCOUNT,"");

	// log
	FDLOG("IOS_PAY")<<worldID<<"|"<<account<<"|"<<price<<"|"<<receipt<<endl;
	

	// send message to LJSDKServer
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);
	pJZMessageLayer->AsyncIosPay(	new IOSPayCallback(m_hEntity),
									worldID,
									account,
									price,
									receipt);
	
}

void ShopSystem::onPaymentQuery(const GSProto::CSMessage& msg)
{
	map<int, int>::iterator iter = m_shopSystemData.paymentMap.begin();
	GSProto::CMD_PAYMENT_QUERY_SC scMsg;
	for(; iter != m_shopSystemData.paymentMap.end(); ++iter)
	{
		scMsg.add_iid(iter->first);
	}
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_PAYMENT_QUERY, scMsg);
}

void ShopSystem::onQueryFirstPayGift(const GSProto::CSMessage& msg)
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>( COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iFirstPayDropId = pGlobal->getInt("首冲礼包ID", 90001);

	IDropFactory *pDropFact = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFact);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iFirstPay = pEntity->getProperty( PROP_ACTOR_SHOWFIRSTPAY, 0);
	// iFirstPay == 0,//没有充值 ，== 1 ，充值了，==2 已经领取了首冲礼包
	bool bIsFirstPay = (iFirstPay == 1);
	
	GSProto::CMD_PAYMENT_FIRST_QUERY_SC scMsg;
	GSProto::FightAwardResult& result = *(scMsg.mutable_resoult());
	scMsg.set_bcanget(bIsFirstPay);
	bool res = pDropFact->calcDrop(iFirstPayDropId,  result);
	assert(res);

	pEntity->sendMessage( GSProto::CMD_PAYMENT_FIRST_QUERY , scMsg);
	
}

void ShopSystem::onPaymentFirstPayGift(const GSProto::CSMessage& msg)
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>( COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iFirstPayDropId = pGlobal->getInt("首冲礼包ID", 90001);

	IDropFactory *pDropFact = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFact);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iFirstPay = pEntity->getProperty( PROP_ACTOR_SHOWFIRSTPAY, 0);
	// iFirstPay == 0,//没有充值 ，== 1 ，充值了，==2 已经领取了首冲礼包
	bool bIsFirstPay = (iFirstPay == 1);

	if(bIsFirstPay)
	{
		GSProto::CMD_PAYMENT_FIRST_PAY_SC scMsg;
		GSProto::FightAwardResult& result = *(scMsg.mutable_resoult());
		bool res = pDropFact->calcDrop(iFirstPayDropId,  result);
		assert(res);

		pDropFact->excuteDrop(m_hEntity, result, GSProto::en_Reason_First_Pay);
		
		pEntity->setProperty( PROP_ACTOR_SHOWFIRSTPAY, 2);

		pEntity->sendMessage( GSProto::CMD_PAYMENT_FIRST_PAY , scMsg);
	}
}


void ShopSystem::dayResetFreeRandValue()
{
	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);

	if(!pZomeTime->IsInSameDay(m_shopSystemData.tokenFreeValue.dwLastChgTime, pZomeTime->GetCurSecond() ) )
	{
		m_shopSystemData.tokenFreeValue.iValue = 0;
		m_shopSystemData.tokenFreeValue.dwLastChgTime = pZomeTime->GetCurSecond();
	}
}

void ShopSystem::sendShopBaseInfo()
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	int iTokenTotalFreeTimes = pGlobal->getInt("商城将星令抽卡免费次数", 5);
	int iFreeInterval =  pGlobal->getInt("免费将星令抽卡时间间隔秒", 300);
	int iGoldFreeInterval =  pGlobal->getInt("免费元宝抽卡时间间隔秒", 172800);
	
	int iRemainFreeTimes = iTokenTotalFreeTimes - m_shopSystemData.tokenFreeValue.iValue ;

	GSProto::CMD_SHOPSYSTEM_QUERY_SC scMsg;

	//将星令抽取
	
	if(iRemainFreeTimes <= 0)
	{
		scMsg.set_btokenhavefreetimes(false);
	}
	else
	{
		scMsg.set_btokenhavefreetimes(true);
		scMsg.set_itotalfreetimes(iTokenTotalFreeTimes);
		scMsg.set_iremaindfreetimes(  iRemainFreeTimes);
		
		int iSecond = pZoneTime->GetCurSecond() - m_shopSystemData.dwLastTokenFreeSecond;
		if(iSecond < iFreeInterval)
		{
			scMsg.set_bhavetime( true);
			scMsg.set_isecond( iFreeInterval - iSecond);
		}
		else
		{
			scMsg.set_bhavetime( false);
		}
	}

	//元宝抽取信息

	int iGoldLeaveSecond = pZoneTime->GetCurSecond() - m_shopSystemData.dwLastGoldFreeSecond;
	if(iGoldLeaveSecond < iGoldFreeInterval)
	{
		scMsg.set_bgoldcanfree(false);
		scMsg.set_iremainsecond(  iGoldFreeInterval - iGoldLeaveSecond);
	}
	else
	{
		scMsg.set_bgoldcanfree(true);
	}

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_SHOPSYSTEM_QUERY,  scMsg);
}

bool ShopSystem::bHaveTokenFreeTimes()
{
	
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	int iTokenTotalFreeTimes = pGlobal->getInt("商城将星令抽卡免费次数", 5);
	int iFreeInterval =  pGlobal->getInt("免费将星令抽卡时间间隔秒", 300);
	
	int iRemainFreeTimes = iTokenTotalFreeTimes - m_shopSystemData.tokenFreeValue.iValue ;

	if(iRemainFreeTimes <= 0)
	{
		return false;
	}
	else
	{
		int iSecond = pZoneTime->GetCurSecond() - m_shopSystemData.dwLastTokenFreeSecond;
		if(iSecond >= iFreeInterval)
		{
			return  true;
		}
	}

	return false;

}

bool ShopSystem::bHaveGoldFreeTimes()
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	int iGoldFreeInterval =  pGlobal->getInt("免费元宝抽卡时间间隔秒", 172800);

	int iGoldLeaveSecond = pZoneTime->GetCurSecond() - m_shopSystemData.dwLastGoldFreeSecond;
	
	if(iGoldLeaveSecond >= iGoldFreeInterval)
	{
		return true;
	}
	
	return false;
}

void ShopSystem::onQueryShopInfo(const GSProto::CSMessage& msg)
{
	sendShopBaseInfo();
}

void  ShopSystem::onQueryCoinTreeShake(const GSProto::CSMessage& msg)
{
	GSProto::CMD_COINTREE_SHAKE_QUERY_CS csMsg;
	if( ! csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	GSProto::CoinTreeShakeType type = csMsg.entype();

	IShopFactory* pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	IVIPFactory *pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVipFactory);
	int iCost = 0;
	int iGet  = 0;

	GSProto::CMD_COINTREE_SHAKE_QUERY_SC scMsg;
	
	int iActorCanShakeTimes = pVipFactory->getVipPropByHEntity(m_hEntity, VIP_PROP_SPEEDPRODUCT_TIMES);
	int iActorHaveUse = m_shopSystemData.shakeTimes.iValue;
	assert(iActorCanShakeTimes>= iActorHaveUse);
	GSProto::CoinTreeDetail& detail = *(scMsg.mutable_detail());
	
	if(GSProto::enCoinTreeShakeType_One == type)
	{
		if(iActorCanShakeTimes > iActorHaveUse)
		{
			iCost = pShopFactory->getShakeCost(iActorHaveUse +1);
			iGet = getCoinTreeShakeBaseResoult();
		}
		
		detail.set_entype(type);
		detail.set_iremaindshaketimes(iActorCanShakeTimes -iActorHaveUse );
		detail.set_itotalshaketimes(iActorCanShakeTimes);
	}
	else if(GSProto::enCoinTreeShakeType_Ten == type)
	{
		int iCanShakeTimes = iActorCanShakeTimes - iActorHaveUse;
		iCanShakeTimes = std::min(iCanShakeTimes,10);
		int iShakeTimes = iActorHaveUse;
		for(int i = 0; i < iCanShakeTimes; ++i)
		{
			iShakeTimes ++;
			int iNowCost = pShopFactory->getShakeCost( iShakeTimes);
			int  iNowGet = getCoinTreeShakeBaseResoult();
			iCost += iNowCost;
			iGet += iNowGet;
		}
		detail.set_entype(type);
		scMsg.set_icanshaketimes(iCanShakeTimes);
	}
	detail.set_iget( iGet);
	detail.set_icost(iCost);

	IEntity*pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(GSProto::CMD_COINTREE_SHAKE_QUERY,  scMsg);
}

int ShopSystem::getCoinTreeShakeBaseResoult()
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	int iLevel = pEntity->getProperty( PROP_ENTITY_LEVEL, 1);
	double resoult = m_ShakeDataVec[0]*m_ShakeDataVec[1];
	resoult  *= 1 + double(iLevel)/m_ShakeDataVec[2];

	return resoult;
}

void  ShopSystem::onCoinTreeShake(const GSProto::CSMessage& msg)
{
	GSProto::CMD_COINTREE_SHAKE_CS csMsg;
	if( ! csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	GSProto::CoinTreeShakeType type = csMsg.entype();
	
	IEntity*pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);	

	IShopFactory* pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	IVIPFactory *pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVipFactory);
	
	//关闭上浮提示框

	CloseAttCommUP close(m_hEntity);
	GSProto::CMD_COINTREE_SHAKE_SC scMsg;
	
	int iActorCanShakeTimes = pVipFactory->getVipPropByHEntity(m_hEntity, VIP_PROP_SPEEDPRODUCT_TIMES);
	int iActorHaveUse = m_shopSystemData.shakeTimes.iValue;
	assert(iActorCanShakeTimes>= iActorHaveUse);
	if(iActorCanShakeTimes == iActorHaveUse)
	{
		return;
	}
	
	if(GSProto::enCoinTreeShakeType_One == type)
	{ 
		int iCost = pShopFactory->getShakeCost(iActorHaveUse +1);
		int iDouble =  pShopFactory->getKnock();
		assert(iDouble >0);
		assert(iCost >0);
		int iGet = getCoinTreeShakeBaseResoult()*iDouble;
		
		GSProto::CoinTreeShakeResoult& resoult = *(scMsg.add_szshakeresoult());
		resoult.set_icost(iCost);
		resoult.set_iget(iGet);
		resoult.set_iknock(iDouble);
		//扣钱 加物品
		int iActorHave = pEntity->getProperty( PROP_ACTOR_GOLD, 0);
		if(iCost > iActorHave)
		{
			pEntity->sendErrorCode(ERROR_NEED_GOLD);
			return;
		}
		pEntity->changeProperty( PROP_ACTOR_GOLD, 0-iCost, GSProto::en_Reason_CoinTree_Shake);
		pEntity->changeProperty( PROP_ACTOR_SILVER, iGet, GSProto::en_Reason_CoinTree_Shake);
		m_shopSystemData.shakeTimes.iValue ++;
		
		{
			EventArgs args;
			args.context.setInt("entity", m_hEntity);
			args.context.setInt("times",1 );
			pEntity->getEventServer()->setEvent(EVENT_ENTITY_YAOQIANSHU_USETIMES, args);
		}
	}
	else if(GSProto::enCoinTreeShakeType_Ten == type)
	{
		int iCanShakeTimes = iActorCanShakeTimes - m_shopSystemData.shakeTimes.iValue;
		iCanShakeTimes = std::min(iCanShakeTimes,10);
		int iShakeTimes = iActorHaveUse;
		int iCost = 0;
		int iGet  = 0;
		for(int i = 0; i < iCanShakeTimes; ++i)
		{
			iShakeTimes ++;
			int iNowCost = pShopFactory->getShakeCost( iShakeTimes);
			int iDouble =  pShopFactory->getKnock();
			assert(iDouble >0);
			assert(iNowCost >0);
			int  iNowGet = getCoinTreeShakeBaseResoult() *iDouble;
			iCost += iNowCost;
			iGet += iNowGet;
			
			GSProto::CoinTreeShakeResoult& resoult = *(scMsg.add_szshakeresoult());
			resoult.set_icost(iNowCost);
			resoult.set_iget(iNowGet);
			resoult.set_iknock(iDouble);
		}
		
		int iActorHave = pEntity->getProperty( PROP_ACTOR_GOLD, 0);
		if(iCost > iActorHave)
		{
			pEntity->sendErrorCode(ERROR_NEED_GOLD);
			return;
		}
	
		pEntity->changeProperty( PROP_ACTOR_GOLD, 0-iCost, GSProto::en_Reason_CoinTree_Shake);
		pEntity->changeProperty( PROP_ACTOR_SILVER, iGet, GSProto::en_Reason_CoinTree_Shake);
		
		m_shopSystemData.shakeTimes.iValue += iCanShakeTimes;
		{
			EventArgs args;
			args.context.setInt("entity", m_hEntity);
			args.context.setInt("times",iCanShakeTimes );
			pEntity->getEventServer()->setEvent(EVENT_ENTITY_YAOQIANSHU_USETIMES, args);
				
		}
		
		
	}

	int iNextCost = 0;
	int iNextGet = 0;
	if(iActorCanShakeTimes >  m_shopSystemData.shakeTimes.iValue )
	{
		iNextCost = pShopFactory->getShakeCost(m_shopSystemData.shakeTimes.iValue +1);
		iNextGet = getCoinTreeShakeBaseResoult();
		assert(iNextCost >0);
		assert(iNextGet >0);
	}
	
	GSProto::CoinTreeDetail& detail = *(scMsg.mutable_detail());
	detail.set_entype(type);
	detail.set_icost(iNextCost);
	detail.set_iget( iNextGet);
	int remaind = iActorCanShakeTimes -m_shopSystemData.shakeTimes.iValue;
	remaind = std::max(remaind, 0);
	detail.set_iremaindshaketimes( remaind);
	detail.set_itotalshaketimes(iActorCanShakeTimes);
	
	pEntity->sendMessage(GSProto::CMD_COINTREE_SHAKE,  scMsg);
}

void ShopSystem::checkCoinTreeReset()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	if(!pZoneTime->IsInSameDay(pZoneTime->GetCurSecond(), m_shopSystemData.shakeTimes.dwLastChgTime))
	{
		m_shopSystemData.shakeTimes.iValue = 0;
		m_shopSystemData.shakeTimes.dwLastChgTime = pZoneTime->GetCurSecond();
	}
}

void ShopSystem::onRequestRandItemOrHero(const GSProto::CSMessage& msg)
{
	GSProto::CMD_SHOPSYSTEM_RANDITEMORHERO_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	GSProto::RandomType type = csMsg.entype();
	//钱够不够
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg *pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);
	
	int iNeedCost = -1;
	int iActorHave = -1;
	bool bIsGold = false;
	bool bIsGoldRandOne = false;
	bool bIsTokenRandOne = false;

	int iCanGetHeroExp = 0;
	int iTokenOnceGetHeroExp = pGlobalCfg->getInt("令牌抽奖一次获得修为", 1000);
	int iGoldOnceGetHeroExp =  pGlobalCfg->getInt("元宝抽奖一次获得修为", 10000);

	int iVipRandOnceGetHeroExp = pGlobalCfg->getInt("VIP元宝抽奖一次获得修为", 15000);
	switch(type)
	{
		case GSProto::en_RandType_Token_RandOne :
		{

			bool bCanFree = bHaveTokenFreeTimes();
			if(bCanFree)
			{
				iNeedCost = 0;
			}
			else
			{
				iNeedCost = pGlobalCfg->getInt("令牌抽取一次花费令牌",1);
			}
			
			iCanGetHeroExp = iTokenOnceGetHeroExp;
			int iTokenId =  pGlobalCfg->getInt("将星令ID",19998);
			iActorHave = pItemSystem->getItemCount(iTokenId);
			
			bIsTokenRandOne  = true;
		}break;
		
		case GSProto::en_RandType_Token_RandTen :
		{
			iNeedCost = pGlobalCfg->getInt("令牌抽取10 次花费令牌",10);
			int iTokenId =  pGlobalCfg->getInt("将星令ID",19998);
			iActorHave = pItemSystem->getItemCount(iTokenId);

			iCanGetHeroExp = iTokenOnceGetHeroExp * 10;
			
		}break;
		
		case GSProto::en_RandType_Gold_RandOne :
		{
			bool bCanFree = bHaveGoldFreeTimes();
			if(bCanFree  )
			{
				iNeedCost = 0;
			}
			else
			{
				 iNeedCost = pGlobalCfg->getInt("元宝抽取一次花费", 299);
			}
			
			 iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD,0);
			 bIsGold = true;
			 bIsGoldRandOne = true;

			 iCanGetHeroExp = iGoldOnceGetHeroExp;
			 
		}break;
		
		case GSProto::en_RandType_Gold_RandTen :
		{
			iNeedCost = pGlobalCfg->getInt("元宝抽取10 次花费", 2990);
			iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD,0);
			bIsGold = true;
			iCanGetHeroExp = iGoldOnceGetHeroExp * 10;
		}break;

		case GSProto::en_RandType_VIP_RandOne:
		{
			//判断VIP等级
			int iActorVIPLevel = pEntity->getProperty(PROP_ACTOR_VIPLEVEL, 0);
			int iVIPNeed = pGlobalCfg->getInt("VIP元宝抽奖VIP等级", 13);
			if(iActorVIPLevel < iVIPNeed)
			{
				//pEntity->sendMessage(GSProto::CMD_NOTICE_CLIENT_PUSHVIP_SCENE);
				return;
			}

			iNeedCost = pGlobalCfg->getInt("VIP元宝抽取1次花费", 499);
			iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD,0);
			bIsGold = true;
			iCanGetHeroExp = iVipRandOnceGetHeroExp ;
			
		}break;

		case GSProto::en_RandType_VIP_RandTen:
		{
			//判断VIP等级
			int iActorVIPLevel = pEntity->getProperty(PROP_ACTOR_VIPLEVEL, 0);
			int iVIPNeed = pGlobalCfg->getInt("VIP元宝抽奖VIP等级", 13);
			if(iActorVIPLevel < iVIPNeed)
			{
				//pEntity->sendMessage(GSProto::CMD_NOTICE_CLIENT_PUSHVIP_SCENE_COMMON);
				return;
			}
			
			iNeedCost = pGlobalCfg->getInt("VIP元宝抽取10次花费", 4988);
			iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD,0);
			bIsGold = true;
			iCanGetHeroExp = iVipRandOnceGetHeroExp *10 ;
		}break;
		
		default:
			return;
	}

	if(iActorHave < iNeedCost)
	{
		if(bIsGold)
		{
			pEntity->sendErrorCode(ERROR_NEED_GOLD);
		}
		else
		{
			pEntity->sendErrorCode(ERROR_NEED_TOKEN);
		}
		return;
	}
	
	if(bIsGold)
	{
		if(iNeedCost != 0)
		{
			pEntity->changeProperty( PROP_ACTOR_GOLD, 0-iNeedCost, GSProto::en_Reason_ShopSystem_RandomHero);
		}
		else
		{
			m_shopSystemData.dwLastGoldFreeSecond = time(0);
		}
		
	}
	else
	{
		if(iNeedCost != 0)
		{
			int iTokenId =  pGlobalCfg->getInt("将星令ID",19998);
			pItemSystem->removeItem( iTokenId, iNeedCost,GSProto::en_Reason_ShopSystem_RandomHero);
		}
		else
		{
			m_shopSystemData.dwLastTokenFreeSecond = time(0);
			m_shopSystemData.tokenFreeValue.iValue += 1;
		}
	
	}

	bool bRandHero = false;
	if(bIsTokenRandOne)
	{
		m_shopSystemData.iTokenRandOnceCount += 1;
		bRandHero = (m_shopSystemData.iTokenRandOnceCount % 10 == 0);
	}
	if(bIsGoldRandOne)
	{	
		if(iNeedCost != 0)
		{
			bRandHero = (m_shopSystemData.iGoldRandOnceCount % 10 == 0);
			m_shopSystemData.iGoldRandOnceCount += 1;
		}
		
	}
	
	vector<RandHero> heroVec;
	vector<RandItem> itemVec;
	heroVec.clear();
	itemVec.clear();
	if(!m_shopSystemData.bHaveFirstRand)
	{
		string strHero = pGlobalCfg->getString("初次抽取英雄","1021#1");
		vector<int> firstRandHeroVec = TC_Common::sepstr<int>(strHero , "#");
		assert(firstRandHeroVec.size() == 2);
		RandHero hero;
		hero.iHeroId = firstRandHeroVec[0];
		hero.iHeroLevelStep =  firstRandHeroVec[1];
		heroVec.push_back(hero);
		m_shopSystemData.bHaveFirstRand = true;
	}
	else
	{
		IShopFactory* pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
		assert(pShopFactory);

		if(GSProto::en_RandType_Gold_RandTen == type)
		{
			bool bFirstGoldTen = m_shopSystemData.bHaveFirstRandTen;
			bool resTen = pShopFactory->randHeroAndItem(heroVec, itemVec, type, bRandHero, ! bFirstGoldTen);
			assert(resTen);
			if(m_shopSystemData.bHaveFirstRandTen == false)
			{
				m_shopSystemData.bHaveFirstRandTen = true;
			}
		}
		else
		{
			bool res = pShopFactory->randHeroAndItem(heroVec, itemVec, type, bRandHero,!m_shopSystemData.bHaveFirstRandTen);
			assert(res);
		}
	}

	{
		CloseAttCommUP close(m_hEntity);
		pEntity->changeProperty( PROP_ENTITY_HEROEXP, iCanGetHeroExp, GSProto::en_Reason_RandHero_Get_HeroExp);
	}
	
	
	IHeroSystem* pHeroSystem = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSystem);
	GSProto::CMD_SHOPSYSTEM_RANDITEMORHERO_SC scMsg;
	for(size_t i = 0; i < heroVec.size() ; ++i)
	{
		GSProto::RandReturnUnit &unit = *(scMsg.add_szreturn());
		RandHero& hero = heroVec[i];
		
		AddHeroReturn addReturn(m_hEntity);
		FDLOG("ShopSystem"  ) <<"addHeroWithLevelStep====="<<hero.iHeroId<< " " << hero.iHeroLevelStep<<"  |"<<endl;
		HEntity hHeroEntity = pHeroSystem->addHeroWithLevelStep(hero.iHeroId, hero.iHeroLevelStep,false, GSProto::en_Reason_ShopSystem_RandomHero);
		if(0 != hHeroEntity)
		{
			unit.set_entype(GSProto::enRandReturnType_Hero);
			unit.set_id(hero.iHeroId);
			unit.set_ilevelup(hero.iHeroLevelStep);
			FDLOG("ShopSystem"  ) <<hero.iHeroId<< " " << hero.iHeroLevelStep<<endl;
		}
		else
		{
			unit.set_entype(GSProto::enRandReturnType_HeroSoul);
			unit.set_id(addReturn.iSoulId);
			unit.set_icount(addReturn.iSoulCount);
			unit.set_iheroid(hero.iHeroId);
			unit.set_ilevelup(hero.iHeroLevelStep);
			FDLOG("ShopSystem"  ) <<addReturn.iSoulId<< " " << addReturn.iSoulCount<<"  |"<< addReturn.iHeroId<<endl;
		}
		
	}

	for(size_t i = 0; i < itemVec.size(); ++i)
	{
		GSProto::RandReturnUnit &unit = *(scMsg.add_szreturn());
		RandItem& item = itemVec[i];
		unit.set_entype(GSProto::enRandReturnType_Item);
		unit.set_id( item.iItemId);
		unit.set_icount( item.iCount);
		CloseAttCommUP close(m_hEntity);
		pItemSystem->addItem( item.iItemId, item.iCount, GSProto::en_Reason_ShopSystem_RandomHero);
	}
	
	pEntity->sendMessage(GSProto::CMD_SHOPSYSTEM_RANDITEMORHERO,  scMsg);

	checkNotice();
 }

void ShopSystem::onQueryHonorConvertInfo(const GSProto::CSMessage& msg)
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	IGlobalCfg *pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	unsigned int refreshSecond = getRefreshSecond();
	unsigned int iLastRefreshSecond = refreshSecond - 24*3600;
	bool bNeedRefresh = false;
	if(m_shopSystemData.dwRefreshSecond < iLastRefreshSecond)
	{
		bNeedRefresh = true;
	}
	if( pZoneTime->GetCurSecond() >= refreshSecond  && refreshSecond >= m_shopSystemData.dwRefreshSecond)
	{
		bNeedRefresh = true;
	}
	if(bNeedRefresh)
	{
		//刷新
		m_shopSystemData.szGoodmap.clear();
		refreshConvertGoods();
		m_shopSystemData.dwRefreshSecond = pZoneTime->GetCurSecond();
		m_shopSystemData.iRefreshTimes  = 0;
	}
	sendHonorInfo();
	
}

int ShopSystem::getRefreshSecond()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IGlobalCfg *pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	int iEveryDayRefreshSecond = pGlobalCfg->getInt("荣誉兑换每日刷新离一天开始秒", 75600);
	int iCurDayBeginSecond = pZoneTime->GetDayBeginSecond( pZoneTime->GetCurSecond());
	int refreshSecond = iCurDayBeginSecond + iEveryDayRefreshSecond;	
	return refreshSecond;
}

void ShopSystem::sendHonorInfo()
{
	GSProto::CMD_SHOPSYSTEM_HONORCONVERT_QUERY_SC scMsg;
	unsigned int refreshSecond = getRefreshSecond();
	if( m_shopSystemData.dwRefreshSecond < refreshSecond)
	{
		scMsg.set_bistoday(true);
	}
	else
	{
		scMsg.set_bistoday(false);
	}
	IGlobalCfg *pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	int iEveryDayRefreshSecond = pGlobalCfg->getInt("荣誉兑换每日刷新离一天开始秒", 75600);
	int moment = iEveryDayRefreshSecond /3600;
	scMsg.set_iupdatemoment(moment);

	map<taf::Int32, ServerEngine::HonorGoodDetail>::iterator  iter = m_shopSystemData.szGoodmap.begin();
	for(; iter != m_shopSystemData.szGoodmap.end(); ++iter)
	{
		ServerEngine::HonorGoodDetail &unit = iter->second;
		GSProto::HonerGoodDetail &detail = *(scMsg.add_szgoodlist());
		detail.set_igoodid(unit.iGoodId);
		detail.set_bissale(unit.bIsSale);
		detail.set_iindex( iter->first);
		FDLOG("ShopSystem")<<unit.iGoodId<<" |"<< unit.bIsSale<<endl;
	}

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(GSProto::CMD_SHOPSYSTEM_HONORCONVERT_QUERY,  scMsg);
}

void ShopSystem::refreshActorHonorConvertShop()
{
	
	//扣钱
	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iNowRefreshTimes = m_shopSystemData.iRefreshTimes;
	int iCost = pShopFactory->getRefreshCost(iNowRefreshTimes+1);
	assert(iCost != -1);
	//钱是否足够
	int iActorHave= pEntity->getProperty(PROP_ENTITY_HONOR, 0);
	if(iActorHave < iCost) 
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	//扣钱
	pEntity->changeProperty( PROP_ENTITY_HONOR, 0-iCost, GSProto::en_Reason_SHOPSYSTEM_RefreshHonorConvert);

	////刷新
	m_shopSystemData.szGoodmap.clear();
	refreshConvertGoods();
	
	m_shopSystemData.iRefreshTimes++;
	sendHonorInfo();
}

void ShopSystem::onReqRefreshHonorConvert(const GSProto::CSMessage& msg)
{
	IScriptEngine *pScript = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScript);

	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iNowRefreshTimes = m_shopSystemData.iRefreshTimes;
	int iCost = pShopFactory->getRefreshCost(iNowRefreshTimes+1);
	assert(iCost != -1);

	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("cost", iCost);
	
	pScript->runFunction("shopHonerConvertRefreshCost",&args, "EventArgs");
}
	
void ShopSystem::onReqConvert(const GSProto::CSMessage& msg)
{
	GSProto::CMD_SHOPSYSTEM_HONORCONVERT_CONVERT_CS csMsg;
	if( !csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	int iIndex = csMsg.iindex();
	map<taf::Int32, ServerEngine::HonorGoodDetail>::iterator  iter = m_shopSystemData.szGoodmap.find(iIndex);
	if(iter ==  m_shopSystemData.szGoodmap.end())
	{
		return; 
	}
	ServerEngine::HonorGoodDetail& detail = iter->second;
 	if( detail.bIsSale)
 	{
 		return;
 	}
	//可以买
	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	ShopGoodBaseInfo goodInfo;
	bool res = pShopFactory->getGoodById(detail.iGoodId, goodInfo);
	assert(res);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iLifeAtt = goodInfo.iCostType;
	int prop = LifeAtt2Prop(iLifeAtt);
	int iNeed = goodInfo.iGoodPrice;
	int iActorHave = pEntity->getProperty( prop, 0);
	if(iActorHave < iNeed )
	{
		int errorCode = getErrorCodeByCostType(iLifeAtt);
		pEntity->sendErrorCode(errorCode);
		return;
	}

	//扣钱
	pEntity->changeProperty(prop, 0-iNeed, GSProto::en_Reason_SHOPSYSTEM_Honor_Convert);

	//发东西
	if(goodInfo.iType == enGoodType_Equip)
	{
		//to do
	}
	else if(goodInfo.iType == enGoodType_HeroSoul || goodInfo.iType == enGoodType_FavorConsume)
	{
		IItemSystem* pITemSys = static_cast<IItemSystem*>( pEntity->querySubsystem(IID_IItemSystem));
		pITemSys->addItem(goodInfo.iGoodsId, goodInfo.iGoodCount, GSProto::en_Reason_SHOPSYSTEM_Honor_Convert);
	}
	
	detail.bIsSale = true;
	sendHonorInfo();
}

int ShopSystem::getErrorCodeByCostType(int iCostType)
{
	if(GSProto::en_LifeAtt_Silver == iCostType)
	{
		return ERROR_NEED_SILVER;
	}
	else if(GSProto::en_LifeAtt_Gold == iCostType)
	{
		return ERROR_NEED_GOLD;
	}
	else if(GSProto::en_LifeAtt_Honor == iCostType)
	{
		return ERROR_NO_HONOR;
	}
	else
	{
		assert(0);
	}
	return -1;
}


void ShopSystem::packSaveData(string& data)
{
	data = ServerEngine::JceToStr(m_shopSystemData);
}


bool  ShopSystem::addGoldForPayment(int iGameCoinCount, string strChanel,const string& strOrderId)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity );
	if(!pEntity);
	
	//是否已经有了这个订单
	if(strOrderId != "")
	{
		//去重
		map<string, bool>::iterator iter = m_shopSystemData.actorPayOrderMap.find(strOrderId);
		if(iter != m_shopSystemData.actorPayOrderMap.end())
		{	
			PLAYERLOG(pEntity)<<"Payment|"<<strChanel<<"|"<<iGameCoinCount<<"Order Repeat"<<strOrderId<<endl;
			return true;
		}
		m_shopSystemData.actorPayOrderMap.insert(make_pair(strOrderId,true));
	}
		
	int payId = iGameCoinCount / 10 ;

	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_Payment);
	assert(pTable);

	int iRecord = pTable->findRecord(payId);
	if(iRecord < 0) 
	{
		FDLOG("Pay_ERROR")<<"PayId == "<< payId<<endl;
		return false;
	}	

	int iGetGameCoin = pTable->getInt(iRecord, "获得游戏币");

	int iPayFlag = pEntity->getProperty( PROP_ACTOR_SHOWFIRSTPAY, 0);
	if(iPayFlag > 0)
	{
		iPayFlag = 1;
	}
	//是否首充
	int iAddition = 0;
	map<int, int>::iterator iter = m_shopSystemData.paymentMap.find(payId);
	if(iter == m_shopSystemData.paymentMap.end())
	{
	   iAddition = pTable->getInt(iRecord, "首次充值赠送");
	   assert(iAddition >= 0);
	   m_shopSystemData.paymentMap.insert(make_pair(payId, 1));
	
	   int iShowFirstPay = pEntity->getProperty( PROP_ACTOR_SHOWFIRSTPAY,0);
	   
	   bool bShowFirstBtn = (iShowFirstPay == 0);
	   if(bShowFirstBtn)
	   {
	   		assert(m_shopSystemData.paymentMap.size() == 1);
	   		pEntity->setProperty( PROP_ACTOR_SHOWFIRSTPAY, 1);
			//提醒
			checkNotice();
	   }
	}
	else
	{
		iter->second += 1;
	}
	
	iGetGameCoin += iAddition;
	pEntity->addVipExp(payId*10);
	pEntity->changeProperty(PROP_ACTOR_GOLD, iGetGameCoin, GSProto::en_Reason_Gold_Pay);
	pEntity->changeProperty(PROP_ACTOR_ACC_PAYMENT, payId, GSProto::en_Reason_Gold_Pay);

	// 增加日充值活动累积数
	IOperateSystem* operateSystem = static_cast<IOperateSystem*>(pEntity->querySubsystem(IID_IOperateSystem));
	assert(operateSystem);
	operateSystem->IncDayPaymentPaied(payId);

	//如果是月卡

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	int iMonthCardPayId = pGlobal->getInt("月卡PayId",25);
	if(payId == iMonthCardPayId)
	{
		ITaskSystem *pTaskSystem = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
		assert(pTaskSystem);
		pTaskSystem->PayMonthCard(1);
	}
	//Log
	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 1);
	PLAYERLOG(pEntity)<<"Payment|"<<strChanel<<"|"<<iGameCoinCount<<"|"<<iGetGameCoin<<"|"<<iPayFlag<<"|"<<iActorLevel<<endl;
	
	return true;
}





// 普通商店相关实现

static int __getIndexOfSecondsArray(unsigned int _time, const vector<unsigned int>& _array)
{
	for(size_t i = 0; i < _array.size(); i ++)
	{
		if(_time < _array[i])
		{
			return (int)i;
		}
	}
	return (int)_array.size();
}


void ShopSystem::__caculateNormalShopRefreshTime(
		bool* _outNeedRefresh ,
		unsigned int*	_outCurTime,
		unsigned int* _outToNextTime)
{
	// 获取相关接口对象
	IZoneTime *zoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	IShopFactory * shopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(zoneTime);
	assert(shopFactory);
	
	// 获取刷新时间数组
	const vector<int>& refreshTimeArray = shopFactory->getNormalShopRefreshTime();
	assert(!refreshTimeArray.empty());

	// 获取当前时间以及当天开始时间
	unsigned int curTime = zoneTime->GetCurSecond();
	unsigned int beginSecond = zoneTime->GetDayBeginSecond(curTime);

	// 展开时间
	vector<unsigned int> refreshSecondArray;
	unsigned int secondFor1Hour = 60 * 60;
	unsigned int secodFor1Day = 24 * secondFor1Hour;
	for(size_t i = 0; i < refreshTimeArray.size(); i ++)
	{
		unsigned int seconds = beginSecond + refreshTimeArray[i] * secondFor1Hour;
		refreshSecondArray.push_back(seconds);
	}

	// 获取上次刷新时间索引和本次刷新时间索引
	unsigned int lastRefreshTime = this->m_shopSystemData.lastNormalShopRefreshSecond;
	int lastRefreshIndex = __getIndexOfSecondsArray(lastRefreshTime, refreshSecondArray);
	int curRefreshIndex = __getIndexOfSecondsArray(curTime, refreshSecondArray);

	// 确定是否需要刷新
	bool needRefresh = (lastRefreshIndex != curRefreshIndex);

	// 计算下次刷新时间
	unsigned int nextRefreshTime = 0;
	if(curRefreshIndex < (int)refreshSecondArray.size())
	{
		nextRefreshTime = refreshSecondArray[curRefreshIndex];
	}
	else
	{
		nextRefreshTime = beginSecond + secodFor1Day + refreshSecondArray[0] * secondFor1Hour;
	}
	unsigned int toNextTime = nextRefreshTime - curTime;

	// 输出
	if(_outNeedRefresh != NULL)	* _outNeedRefresh = needRefresh;
	if(_outCurTime != NULL)		* _outCurTime = curTime;
	if(_outToNextTime != NULL)	* _outToNextTime = toNextTime;

	//debug
	printf(	"__caculateNormalShopRefreshTime() : needRefresh:%u curTime:%u toNextTime:%u\n",
			needRefresh,
			curTime,
			toNextTime);
	return;
}


void ShopSystem::__refreshNormalShop()
{
	//刷新
	this->m_shopSystemData.normalShopGoods.clear();
	IShopFactory * shopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(shopFactory);
	vector < ShopGoodUnit > shopVec;
	shopFactory->getNormalShop(m_hEntity, shopVec);
	for(size_t i = 0; i < shopVec.size(); ++i)
	{
		ShopGoodUnit& unit = shopVec[i];
		ServerEngine::HonorGoodDetail detail;
		detail.iGoodId = unit.iGoodId;
		detail.bIsSale = false;
		m_shopSystemData.normalShopGoods.insert(make_pair(i , detail));
	}
}

void ShopSystem::__sendNormalShop()
{
	// 获取相关对象
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	assert(playerE);

	// 计算下次刷新时间
	unsigned int toNextRefreshTime = 0;
	this->__caculateNormalShopRefreshTime(NULL, NULL, &toNextRefreshTime);

	//生成response
	GSProto::CMD_NORMALSHOP_QUERY_SC	 response;
	response.set_untilnextrefreshseconds(toNextRefreshTime);

	//printf("goods detail count:%u \n", (unsigned int)this->m_shopSystemData.normalShopGoods.size());
	map<taf::Int32, ServerEngine::HonorGoodDetail>::iterator  it = this->m_shopSystemData.normalShopGoods.begin();
	while( it != this->m_shopSystemData.normalShopGoods.end())
	{
		ServerEngine::HonorGoodDetail &unit = it->second;
		GSProto::HonerGoodDetail &detail = *(response.add_goodslist());
		// printf(	"goods id:%u issale:%u index:%u\n", 
		//		unit.iGoodId,
		//		unit.bIsSale,
		//		it->first);
		
		detail.set_igoodid(unit.iGoodId);
		detail.set_bissale(unit.bIsSale);
		detail.set_iindex(it->first);
		it ++;
	}

	// 进行发送
	playerE->sendMessage(GSProto::CMD_NORMALSHOP_QUERY, response);

	/*
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	IZoneTime *zoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	IShopFactory *shopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(playerE);
	assert(zoneTime);
	assert(shopFactory);
	
	GSProto::CMD_NORMALSHOP_QUERY_SC	 response;

	// 计算下次刷新的时间
	unsigned int curTime = zoneTime->GetCurSecond();
	unsigned int lastRefreshTime = this->m_shopSystemData.lastNormalShopRefreshSecond;
	unsigned int refreshInterval = shopFactory->getNormalShopRefreshInterval();
	unsigned int nextRefreshTime = lastRefreshTime + refreshInterval;
	unsigned int untilNextRefreshSeconds = nextRefreshTime - curTime;
	response.set_untilnextrefreshseconds(untilNextRefreshSeconds);

	// 写入商店列表
	printf("goods detail count:%u \n", this->m_shopSystemData.normalShopGoods.size());
	map<taf::Int32, ServerEngine::HonorGoodDetail>::iterator  it = this->m_shopSystemData.normalShopGoods.begin();
	while( it != this->m_shopSystemData.normalShopGoods.end())
	{
		ServerEngine::HonorGoodDetail &unit = it->second;
		GSProto::HonerGoodDetail &detail = *(response.add_goodslist());
		printf(	"goods id:%u issale:%u index:%u\n", 
				unit.iGoodId,
				unit.bIsSale,
				it->first);
		detail.set_igoodid(unit.iGoodId);
		detail.set_bissale(unit.bIsSale);
		detail.set_iindex(it->first);
		it ++;
	}

	// 进行发送
	playerE->sendMessage(GSProto::CMD_NORMALSHOP_QUERY, response);
	*/
}


void ShopSystem::onReqNormalShopQuery(const GSProto::CSMessage& _msg)
{
	// 获取先关对象
	//IEntity* playerE = getEntityFromHandle(m_hEntity);

	// 判定是否需要刷新
	unsigned int curTime = 0;
	bool needRefresh = false;
	this->__caculateNormalShopRefreshTime(&needRefresh, &curTime, NULL);

	// 刷新
	if(needRefresh)
	{
		this->__refreshNormalShop();
		this->m_shopSystemData.lastNormalShopRefreshSecond = curTime;
		this->m_shopSystemData.normalShopRefreshTimes = 0;
	}

	// 返回普通商店数据给客户端
	this->__sendNormalShop();

	// debug
///	printf("ShopSystem::onReqNormalShopQuery() : curTime:%u needRefresh:%u", curTime, needRefresh);
											

	/*
	// 获取实体以及相关对象
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	IZoneTime *zoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	IShopFactory *shopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(playerE);
	assert(zoneTime);
	assert(shopFactory);

	// 判断是否需要刷新, 并计算&设置刷新时间
	bool needRefresh = false;
	unsigned int lastRefreshTime = this->m_shopSystemData.lastNormalShopRefreshSecond;
	unsigned int curTime = zoneTime->GetCurSecond();
	unsigned int newLastRefreshTime = lastRefreshTime;
	if(lastRefreshTime == 0)
	{
		needRefresh = true;
		newLastRefreshTime = curTime;
	}
	else
	{
		unsigned int refreshInterval = shopFactory->getNormalShopRefreshInterval();
		unsigned int timePassby = curTime - lastRefreshTime;
		unsigned int div = timePassby / refreshInterval;
		unsigned int mod = timePassby % refreshInterval;
		if(div == 0)
		{
			needRefresh = false;
		}
		else
		{
			needRefresh = true;
			newLastRefreshTime = curTime - mod;
		}
	}

	// 保存刷新时间
	if(newLastRefreshTime != lastRefreshTime)
	{
		this->m_shopSystemData.lastNormalShopRefreshSecond = newLastRefreshTime;
	}
	
	// 刷新(会更新记录的刷新次数)
	if(needRefresh)
	{
		this->__refreshNormalShop();
		this->m_shopSystemData.normalShopRefreshTimes = 0;
	}

	// 返回普通商店数据给客户端
	this->__sendNormalShop();

	// debug
	printf(	"ShopSystem::onReqNormalShopQuery() : "
			"needRefresh:%u "
			"lastRefreshTime:%u "
			"curTime:%u "
			"newLastRefreshTime:%u \n",
			needRefresh,
			lastRefreshTime,
			curTime,
			newLastRefreshTime);
	*/
}

void ShopSystem::onReqNormalShopRefresh(const GSProto::CSMessage& _msg)
{
	// 获取对象
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	IScriptEngine *scriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	IZoneTime *zoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	IShopFactory *shopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(playerE);
	assert(scriptEngine);
	assert(zoneTime);
	assert(shopFactory);

	// 获取刷新价格(单位:元宝)
	unsigned int refreshTimes = this->m_shopSystemData.normalShopRefreshTimes;
	int ybCost = shopFactory->getRefreshCost((int)(refreshTimes + 1));
	assert(ybCost != -1);

	// 调用脚本
	EventArgs args;
	args.context.setInt("entity", this->m_hEntity);
	args.context.setInt("cost", ybCost);
	scriptEngine->runFunction("normalShopRefreshCost", &args, "EventArgs");

	// debug
	printf(	"ShopSystem::onReqNormalShopRefresh() : "
			"refreshTimes:%u "
			"ybCost:%u \n",
			refreshTimes,
			ybCost);
}

void ShopSystem::normalShopRefreshCallback()
{
	// 获取对象
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	IScriptEngine *scriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	IZoneTime *zoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	IShopFactory *shopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(playerE);
	assert(scriptEngine);
	assert(zoneTime);
	assert(shopFactory);

	// 获取刷新价格(单位:元宝)
	unsigned int refreshTimes = this->m_shopSystemData.normalShopRefreshTimes;
	int ybCost = shopFactory->getRefreshCost((int)(refreshTimes + 1));
	assert(ybCost != -1);

	// 检查愿元宝是否足够
	int ybHas = playerE->getProperty(PROP_ACTOR_GOLD, 0);
	if(ybHas < ybCost)
	{
		DEBUG_LOG(playerE)<<"normalShopRefreshCallback:[error]:no enough yb."<<endl;
		playerE->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	// 扣除元宝
	playerE->changeProperty(PROP_ACTOR_GOLD, - ybCost, GSProto::en_Reason_NormalShop_Refresh);

	// 进行刷新& 更新刷新次数
	this->__refreshNormalShop();
	this->m_shopSystemData.normalShopRefreshTimes ++;

	// 发送数据给客户端
	this->__sendNormalShop();

	// debug
	printf(	"ShopSystem::normalShopRefreshCallback() : "
			"refreshTimes:%u "
			"ybHas:%u "
			"ybCost:%u \n",
			refreshTimes,
			ybHas,
			ybCost);
}

void ShopSystem::onReqNormalShopBuy(const GSProto::CSMessage& _msg)
{
	// 获取角色及其他对象
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	IShopFactory *shopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(playerE);
	assert(shopFactory);
	
	// 获取消息
	GSProto::CMD_NORMALSHOP_BUY_CS request;
	if( !request.ParseFromString(_msg.strmsgbody()))
	{
		DEBUG_LOG(playerE) <<"ShopSystem::onReqNormalShopBuy() : [error] parse request failed." <<endl;
		return;
	}
	int index = request.index();

	// 获取对应的商品信息
	map<taf::Int32, ServerEngine::HonorGoodDetail>::iterator it = this->m_shopSystemData.normalShopGoods.find(index);
	if(it == this->m_shopSystemData.normalShopGoods.end())
	{
		DEBUG_LOG(playerE) <<"ShopSystem::onReqNormalShopBuy() : [error] can't find good detail data with index"<<endl;
		return;
	}
	ServerEngine::HonorGoodDetail& detail = it->second;
	if(detail.bIsSale)
	{
		DEBUG_LOG(playerE)<<"ShopSystem::onReqNormalShopBuy() : [error] goods is not on sale."<<endl;
		return;
	}
	
	ShopGoodBaseInfo goodsInfo;
	bool res = shopFactory->getGoodById(detail.iGoodId,  goodsInfo);
	assert(res);

	// 获取各项属性
	int lifeAtt = goodsInfo.iCostType;
	int goodsPrice = goodsInfo.iGoodPrice;
	int goodsPriceType = LifeAtt2Prop(lifeAtt);

	// 检查余额
	int has = playerE->getProperty( goodsPriceType, 0);
	if(has < goodsPrice)
	{
		int errorCode = getErrorCodeByCostType(lifeAtt);
		playerE->sendErrorCode(errorCode);
		return;
	}

	// 扣钱
	playerE->changeProperty(goodsPriceType, -goodsPrice, GSProto::en_Reason_NormalShop_Buy);

	//发东西
	if(goodsInfo.iType == enGoodType_Equip)
	{
		IEquipBagSystem* equipSystem = static_cast<IEquipBagSystem*>(playerE->querySubsystem(IID_IEquipBagSystem) );
		assert(equipSystem);
		equipSystem->addEquip(goodsInfo.iGoodsId, GSProto::en_Reason_NormalShop_Buy, false);
	}
	else if(goodsInfo.iType == enGoodType_HeroSoul || goodsInfo.iType == enGoodType_FavorConsume)
	{
		IItemSystem* itemSystem = static_cast<IItemSystem*>( playerE->querySubsystem(IID_IItemSystem));
		itemSystem->addItem(goodsInfo.iGoodsId, 
							goodsInfo.iGoodCount, 
							GSProto::en_Reason_NormalShop_Buy);
	}

	// 更新卖出状态
	detail.bIsSale = true;

	// 返回客户端数据
	this->__sendNormalShop();
}

// 普通商店相关实现 end

