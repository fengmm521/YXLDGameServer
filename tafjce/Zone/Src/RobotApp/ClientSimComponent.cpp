#include "RobotServerPch.h"
#include "ClientSimComponent.h"
#include "ClientPlayer.h"

extern "C" IComponent* CreateSimComponent(Int32)
{
	return new ClientSimComponent;
}

ClientSimComponent::ClientSimComponent()
{
}

ClientSimComponent::~ClientSimComponent()
{
}

bool ClientSimComponent::initlize(const PropertySet& propSet)
{
    m_iCurRobotIndex = 0;
    m_iMaxRobotCount = -1; //>=0表示只登录csv中前几个，否则表示不限制


    //单位ms
    m_iRobotLoginGap = 100;
    m_iRobotLoginCountInGap = 2;
  
	ISocketSystem* pSocketSystem = getComponent<ISocketSystem>(COMPNAME_SocketSystem, IID_ISocektSystem);
	assert(pSocketSystem);
	pSocketSystem->SetPacketParse(&m_packetParse);
    
	ITable* pRobotTb = getCompomentObjectManager()->findTable("Robot");
	assert(pRobotTb);
    
    m_iRobotCount = pRobotTb->getRecordCount();
	ITimerComponent* pTimerComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimerComponent);

	m_hTimeHandle = pTimerComponent->setTimer(this, 1, m_iRobotLoginGap, "NewRoleConnect");
    return true;
}

void ClientSimComponent::onTimer(int nEventId)
{
    if(m_iCurRobotIndex >= m_iRobotCount
        ||
        (m_iMaxRobotCount >= 0 && m_iCurRobotIndex >= m_iMaxRobotCount)
        )
    {
        return;
    }

	ISocketSystem* pSocketSystem = getComponent<ISocketSystem>(COMPNAME_SocketSystem, IID_ISocektSystem);
	assert(pSocketSystem);
    
	ITable* pRobotTb = getCompomentObjectManager()->findTable("Robot");
	assert(pRobotTb);
    
    int iCount = 0;
	for(int i = m_iCurRobotIndex; i < m_iRobotCount; ++i,++iCount)
	{
        if(iCount >= m_iRobotLoginCountInGap)
            break;
		
		
		
		IClientSocket* pClientSocket = pSocketSystem->CreateClientSocket();
		assert(pClientSocket);

		string strAccount = pRobotTb->getString(i, "帐号");
		string strRoleName = pRobotTb->getString(i, "角色名字");
		int iWorldID = pRobotTb->getInt(i, "世界ID");
		string strAICmd = pRobotTb->getString(i, "AI命令");

		ClientPlayer* pPlayer = new ClientPlayer(pClientSocket, strAccount, strRoleName, iWorldID, strAICmd);
		pClientSocket->SetClientSink(pPlayer);

		//pClientSocket->Connect("114.215.181.2", 22322);
		pClientSocket->Connect("192.168.1.100", 22322);
        m_iCurRobotIndex++;
	}
}

