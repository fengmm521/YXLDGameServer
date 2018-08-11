#ifndef __ITEM_H__
#define __ITEM_H__

struct EquipProp;
class Item:public ObjectBase<IItem>, public Detail::EventHandle
{
public:

	friend class ItemFactory;

	Item();
	~Item();

	// IItem Interface
	virtual bool useItem(HEntity hTarget, const PropertySet& ctx);
	virtual void packJce(ServerEngine::ItemData& itemSaveData);
	virtual HItem getHandle();
	virtual void packScItemInfo(GSProto::ItemInfo& scItemInfo);
	virtual void packScEquipInfo(GSProto::EquipInfo& scEquipInfo, int iPos);
	virtual void changeProperty(HEntity hEntity, bool bOn);

	void randomEquipProp();

	int getEquipPropAddPercent();

	/**
	 * 将属性id转换为显示属性id
	 * <p>	服务器记录的属性id	</p>
	 * <p>	在发送消息给客户端时，需要将属性id转换为显示属性id</p>
	 * <p>	转换失败,返回-1	</p>
	 */
	static int convertAddPropIdToLifeAttri(int _addPropId);


	/**
	 * @brief 获取洗练属性
	 */
	XilianPropVec& getXilianProp();

private:

	HItem m_hHandle;
	EquipProp* m_pEquipProp;
	XilianPropVec xilianProp;	// 洗练属性
};


#endif
