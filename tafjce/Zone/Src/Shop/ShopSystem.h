#ifndef SHOPSYSTEM_H_
#define SHOPSYSTEM_H_

class ShopSystem
:public ObjectBase<IShopSystem>
,public Detail::EventHandle
,public ITimerCallback 
{
public:
	ShopSystem();
	virtual ~ShopSystem();
	
public:
	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	// ITimerCallback Interface
	virtual void onTimer(int nEventId);
	virtual  void refreshActorHonorConvertShop();
	virtual bool  addGoldForPayment(int iGameCoinCount, string strChanel,const string& strOrderId="");

	virtual void normalShopRefreshCallback();
	
protected:
	void onRequestRandItemOrHero(const GSProto::CSMessage& msg);
	void onQueryHonorConvertInfo(const GSProto::CSMessage& msg);
	void onReqRefreshHonorConvert(const GSProto::CSMessage& msg);
	void onReqConvert(const GSProto::CSMessage& msg);
	void sendHonorInfo();
	int getRefreshSecond();
	int getErrorCodeByCostType(int iCostType);

	void refreshConvertGoods();

	void onQueryCoinTreeShake(const GSProto::CSMessage& msg);
	void onCoinTreeShake(const GSProto::CSMessage& msg);
	int getCoinTreeShakeBaseResoult();
	void checkCoinTreeReset();

	void dayResetFreeRandValue();
	void onQueryShopInfo(const GSProto::CSMessage& msg);
	void sendShopBaseInfo();
	bool bHaveTokenFreeTimes();
	bool bHaveGoldFreeTimes();

	void onPaymentFirstPayGift(const GSProto::CSMessage& msg);

	void checkNotice();
	void onQueryFirstPayGift(const GSProto::CSMessage& msg);
	void onPaymentQuery(const GSProto::CSMessage& msg);

	void onVIVOPrePay(const GSProto::CSMessage& msg);

	void onIOSPrePay(const GSProto::CSMessage& _msg);

	unsigned char ToHex(unsigned char x);

	std::string UrlEncode(const std::string& str);

	// 普通商店相关接口

	/**
	 * @brief 计算普通商店时间相关参数
	 * @param	_outNeedRefresh	[out] : 是否需要刷新
	 * @param	_outCurTime		[out] : 当前时间
	 * @param	_outToNext		[out] : 距离下一次刷新的秒数
	 */
	void __caculateNormalShopRefreshTime(
		bool* _outNeedRefresh 		= NULL,
		unsigned int*	_outCurTime 	= NULL,
		unsigned int* _outToNextTime 	= NULL
	);

	/**
	 * 刷新普通商店
	 */
	void __refreshNormalShop();

	/**
	 * 返回普通商店数据给客户端
	 */
	void __sendNormalShop();


	/**
	 * 查询普通商店数据
	 */
	void onReqNormalShopQuery(const GSProto::CSMessage& _msg);
	
	/**
	 * 刷新普通商店
	 */
	void onReqNormalShopRefresh(const GSProto::CSMessage& _msg);

	/**
	 * 普通商店购买
	 */
	void onReqNormalShopBuy(const GSProto::CSMessage& _msg);

	// 普通商店相关接口end
private:
	HEntity m_hEntity;
	ServerEngine::ShopSystemData m_shopSystemData;
	vector<int> m_ShakeDataVec;

	ITimerComponent::TimerHandle m_CheckNoticeTimerHandle;
};



#endif





