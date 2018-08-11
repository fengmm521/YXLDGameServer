#ifndef __EQUIP_BAGSYSTEM_H__
#define __EQUIP_BAGSYSTEM_H__


// 洗练配置相关结构
typedef struct stXilianCfg_ValueChance
{
	int min;
	int max;
	int chance;
	stXilianCfg_ValueChance():min(0),max(0),chance(0){};
	
}XilianCfg_ValueChance;

typedef struct stXilianCfg_Prop
{
	int propId;
	int chance;
	int maxValue;
	vector<stXilianCfg_ValueChance*>	valueChanceList;

	stXilianCfg_Prop():propId(0),chance(0),maxValue(0){};
}XilianCfg_Prop;

typedef struct stXilianCfg_EquipPos
{
	int equipPos;
	map<int, XilianCfg_Prop*> propMap;
	stXilianCfg_EquipPos():equipPos(0){};
}XilianCfg_EquipPos;

typedef map<int, XilianCfg_EquipPos*>	XilianCfg;
// 洗练配置相关结构end





class EquipBagSystem:public ObjectBase<IEquipBagSystem>, public IEquipContainer, public Detail::EventHandle
{
public:

	EquipBagSystem();
	~EquipBagSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	virtual void* queryInterface(int iInterfaceID);

	// IEquipBagSystem Interface
	virtual Uint32 addEquip(	int 	iItemID, 
								int 	iReason, 
								bool 	bNotifyError,
								int*	_outPos = NULL);
	virtual int getFreeSize();
	virtual int addEquipByHandle(HItem hEquip, int iReason, bool bNotifyError);

	// IEquipContainer Interface
	virtual int getSize();
	virtual Uint32 getEquip(int iPos);
	virtual bool setEquip(int iPos, Uint32 hEquipHandle, GSProto::EquipChgItem* pOutItem);
	virtual HEntity getOwner();
	virtual void sendContainerChg(const vector<int>& posList);
	virtual bool canSetEquip(int iPos, Uint32 hEquipHandle, bool bNotifyError);
	virtual int getContainerType(){return GSProto::en_EquipContainer_ActorBag ;}

	void initFromDB(const ServerEngine::EquipBagSystemData& equipData);
	void onEventSend2Client(EventArgs& args);

	void onReqQueryHeroEquip(const GSProto::CSMessage& msg);
	void onReqMoveEquip(const GSProto::CSMessage& msg);
	void onReqSellByPos(const GSProto::CSMessage& msg);
	void onReqSellByQuality(const GSProto::CSMessage& msg);
	void onReqEquipCombine(const GSProto::CSMessage& msg);
	void onReqQueryEquipByID(const GSProto::CSMessage& msg);
	void onReqEquipSource(const GSProto::CSMessage& msg);

	void onReqQueryEnhance(const GSProto::CSMessage& msg);
	void onReqEquipEnhance(const GSProto::CSMessage& msg);
	void onReqEquipChipSell(const GSProto::CSMessage& msg);
	void onReqChipCombine(const GSProto::CSMessage& msg);

	// rong lian equips.
	void onReqEquipRonglian(const GSProto::CSMessage& _msg);

	
	

	bool checkMoveValid(HEntity hSrcEntity, HEntity hDstEntity, int iSrcPos, int& iDstPos);
	bool checkEntityValid(HEntity hEntity);
	IEquipContainer* getEquipContainer(HEntity hEntity);
	int getFirstFreePos();
	bool checkEquuipLevelLimit(int iLevel, bool bNotify);
	int randEquipLevelUp();

	// get equip ronglian lv related config data
	bool getEquipRonglianLvRule(	int 	_equipLv,
											int*	_outValue = NULL,
											int* 	_outCostRatio = NULL);

	// get equip ronglian quality related config data
	bool getEquipRonglianQualityRule(	int 	_quality,
													int*	_outValue = NULL,
													int*	_outCostRatio = NULL);

	// get equip quanlity chance
	// 万分位
	bool getEquipRonglianQualityChance(	int _b,
													vector<int>& _outChanceVec);

	// get possible equip id
	vector<int>* getEquipRonglianIds(	int _lv,
												int _equipType,
												int _equipQuality);

	/**
	 * @brief 检查洗练装备功能是否开启
	 * @param	_notifyErrorCode	[in] : 是否向客户端下行错误码
	 * @return	是否开启
	 */
	bool __checkXilianFunctionOpen(bool _notifyErrorCode = true);


	/**
	 * @brief 获取当前洗练数据
	 * @param	_msg	[in] : 消息
	 */
	void onReqEquipXilianData(const GSProto::CSMessage& _msg);

	/**
	 * @brief 请求进行洗练
	 * @param 	_msg	[in] : 消息
	 */
	void onReqEquipXilianGen(const GSProto::CSMessage& _msg);


	/**
	 * @brief 响应请求 : 将洗练属性替换到装备上
	 */
	void onReqEquipXilianReplace(const GSProto::CSMessage& _msg);

	/**
	 * @brief 响应请求 : 将洗练属性从一个装备转移到另一件装备上
	 */
	void onReqEquipXilianTransfer(const GSProto::CSMessage& _msg);

	/**
	 * @brief 购买洗炼锁
	 */
	void onReqEquipXilianBuyLock(const GSProto::CSMessage& _msg);

private:
	/**
	 * @brief 初始化洗练配置数据
	 */
	void __initXilianConfigData();


	/**
	 * @brief 根据装备位置获取洗练配置
	 */
	XilianCfg_EquipPos* getXilianCfgByEquipPos(int _equipPos);


	int getPosFromHandle(HItem _itemH);


	/**
	 * @brief 通过容器id和位置查询装备
	 * @param _cobjId			[in] : 容器id
	 * @param _pos				[in] : 位置
	 * @param _outEquip			[out] : 输出装备
	 * @param _outContainer		[out] : 容器
	 */
	bool queryEquip(HEntity 			_cobjId, 
						int 				_pos,
						Uint32&				_outEquip,
						IEquipContainer**	_outContainer = NULL);

private:

	HEntity m_hEntity;
	vector<HItem> m_equipList;

	Uint32 				m_xilianOBJId;		// 洗练obj对象
	XilianPropVec		m_xilianPropList;	// 洗练属性数组
};


#endif
