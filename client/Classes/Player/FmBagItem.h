#ifndef _FMBAGITEM_H_
#define _FMBAGITEM_H_
#include "FmConfig.h"
#include "../Numeric/ItemData.h"
#include "FmEntity.h"
NS_FM_BEGIN

class BagItem;
class Bag : public Component
{
private:
	vector<BagItem*> m_bagItems;
	int	m_containerType;
public:
	vector<BagItem*>& GetBagList(){return m_bagItems;}
	Bag(Entity* entity, int type);
	int GetItemCountByItemId(int itemId);
	BagItem* GetBagItemByPos(int pos)
	{
		if (pos < 0 || pos >= m_bagItems.size())
		{
			return NULL;
		}

		return m_bagItems[pos];
	}
};

class BagItem : public Entity{

friend class Bag;

protected:
	BagItem( uint8 entityType, uint entityId, const string& name );
public:
	static Entity* Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt );
	static void InitInterface();
public:
	virtual ~BagItem();
	//ªÒ»°ItemData
	stItemData* getItemData(){return m_itemData;}
	void SetItemData(stItemData* itemData){m_itemData = itemData;}

private: 
	stItemData* m_itemData;
};

NS_FM_END
#endif
