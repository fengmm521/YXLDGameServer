#include "SyncSettingThread.h"


SyncSettingThread::SyncSettingThread():m_bTerminate(false)
{
}

SyncSettingThread::~SyncSettingThread()
{
}

void SyncSettingThread::setDbAccess(ServerEngine::DbAccessPrx prx)
{
	m_dbaccessPrx = prx;
}


void SyncSettingThread::terminate()
{
	TC_ThreadLock::Lock lock(*this);
	m_bTerminate = true;
	notify();
}


void SyncSettingThread::run()
{
	while(!m_bTerminate)
	{
		std::pair<string, OperateData> tmpData;
		size_t queueSize = 0;
		{
			TC_ThreadLock::Lock lock(*this);
			if(m_settingMap.size() == 0)
			{
				timedWait(10000);
				continue;
			}

			assert(m_settingMap.size() > 0);
			std::map<string, OperateData>::iterator it = m_settingMap.begin();
			tmpData = *it;
			m_settingMap.erase(it);
			queueSize = m_settingMap.size();
		}

		const OperateData& tmpOperateData = tmpData.second;
		try
		{
			if(tmpOperateData.iOperate == en_Operate_Set)
			{
				m_dbaccessPrx->setString(tmpData.first, tmpOperateData.strValue);
				FDLOG("SettingSuccess")<<tmpData.first<<"|"<<queueSize<<endl;
			}
		}
		catch(...)
		{
			FDLOG("SetingFail")<<queueSize<<"|"<<tmpData.first<<"|"<<tmpOperateData.strValue<<endl;
		}


		try
		{
			if(tmpOperateData.iOperate == en_Operate_Del)
			{
				m_dbaccessPrx->delString(tmpData.first);
				FDLOG("DelSuccess")<<tmpData.first<<"|"<<queueSize<<endl;
			}
		}
		catch(...)
		{
			FDLOG("DelFail")<<queueSize<<"|"<<tmpData.first<<"|"<<tmpOperateData.strValue<<endl;
		}
	}
}

void SyncSettingThread::eraseData(const string& strKey)
{
	TC_ThreadLock::Lock lock(*this);
	OperateData tmpData;
	tmpData.iOperate = en_Operate_Del;

	m_settingMap[strKey] = tmpData;

	notify();
}


bool SyncSettingThread::isDeleting(const string& strKey)
{
	TC_ThreadLock::Lock lock(*this);

	std::map<string, OperateData>::iterator it = m_settingMap.find(strKey);
	if(it == m_settingMap.end() )
	{
		return false;
	}

	const OperateData& tmpOperate = it->second;
	if(tmpOperate.iOperate == en_Operate_Del)
	{
		return true;
	}

	return false;
}


bool SyncSettingThread::getSettingData(const string& strKey, string& strValue)
{
	TC_ThreadLock::Lock lock(*this);

	std::map<string, OperateData>::iterator it = m_settingMap.find(strKey);
	if(it != m_settingMap.end() )
	{
		const OperateData& tmpOperate = it->second;
		if(tmpOperate.iOperate == en_Operate_Set)
		{
			strValue = tmpOperate.strValue;
			return true;
		}
	}

	return false;
}



void SyncSettingThread::setSettingData(const string& strKey, const string& strValue)
{
	TC_ThreadLock::Lock lock(*this);

	OperateData tmpData;
	tmpData.iOperate = en_Operate_Set;
	tmpData.strValue = strValue;
	
	m_settingMap[strKey] = tmpData;

	notify();
}



