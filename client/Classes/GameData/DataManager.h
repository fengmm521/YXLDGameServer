#ifndef ____DATA_MANAGER_H____
#define ____DATA_MANAGER_H____

#pragma once

#include "GameData.h"
#include "FmConfig.h"
//#include "enumdata.h"
#include "Enum/FmNumricEnum.h"

NS_FM_BEGIN

class CDataManager
{
public:
	CDataManager(void);
	virtual ~CDataManager(void);

	void Init();
	void ShutDown();

	SINGLETON_MODE(CDataManager);

	void LoadDownloadListFile(char* pData, int length);

	const vector<int>* GetGameDataKeyList(int storageID);

	template<class T>
	T* GetGameData( int storageID, int dataID )
	{
		if (storageID < 0 || storageID >= DATA_FILE_COUNT)
		{
			return NULL;
		}

		return (T*)(m_DataStorage[storageID]->GetData(dataID));
	}

	void* GetGameDataUnKnow( int storageID, int dataID )
	{
		if (storageID < 0 || storageID >= DATA_FILE_COUNT)
		{
			return NULL;
		}

		return m_DataStorage[storageID]->GetData(dataID);
	}

private:
	IGameData** m_DataStorage;
};

NS_FM_END

#endif