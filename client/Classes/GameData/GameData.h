#ifndef ____GAME_DATA_H____
#define ____GAME_DATA_H____

#include "FmConfig.h"
#include "Util/ustring.h"
#include "IGameData.h"
#include "FmStream.h"
NS_FM_BEGIN

struct DefaultMkKey
{
	template<class T> void* MakeKey(const T* poData)
	{
		return (void*)poData->m_ID;
	}
};

template<typename GameDataType, typename KeyMake>
class CGameData : public IGameData
{
public:
	CGameData(void) {}

	virtual ~CGameData(void)
    {
    }

    template<typename GameDataType1>
    void Dispose()
    {
        typename map<int, GameDataType1*>::iterator it = m_GameData.begin();
        for (; it != m_GameData.end(); ++it )
        {
            GameDataType1* data = it->second;
            FM_SAFE_DELETE(data);
        }
        m_GameData.clear();
    }

	virtual void* GetData(int key)
	{
		if (m_GameData.find(key) != m_GameData.end())
			return m_GameData[key];
		else
			return NULL;
	}

	//template<typename GameDataType1>
	void LoadData(Freeman::MemStream& stream)
	{
		while (stream.GetOffset() < stream.GetSize())
		{
			GameDataType* data = new GameDataType;
			data->Decode(stream);
			uintptr_t key = (uintptr_t)KeyMake().MakeKey(data);
			m_GameData.insert(typename map<int, GameDataType*>::value_type(key, data));
			m_keyList.push_back(key);
		}
	}

	virtual const vector<int>*  GetKeyList(){return &m_keyList;}

private:
	map<int, GameDataType*> m_GameData;
	vector<int> m_keyList;
};

NS_FM_END
#endif
