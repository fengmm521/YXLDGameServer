#ifndef __CLIENT_SIM_COMPONENT_H__
#define __CLIENT_SIM_COMPONENT_H__

class SimpleParse:public IPacketParse
{
public:

	SimpleParse(){}
	~SimpleParse(){}

	int ParsePacket(std::string& in, std::string& out)
	{
		if(in.size() <= 2) return PACKET_LESS;

		short wLen = *(short*)in.c_str();
		wLen = ntohs(wLen);
		
		if( (short)in.length() < wLen) return PACKET_LESS;

		out = in.substr(0, wLen);
		in = in.substr(wLen);

		return PACKET_FULL;
	}
};


class ClientSimComponent:public ComponentBase<IComponent, 1>,public ITimerCallback
{
public:

	ClientSimComponent();
	~ClientSimComponent();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);
    
private:

    // ITimerCallback Interface
	virtual void onTimer(int nEventId);

private:
	SimpleParse m_packetParse;
	int m_iCurRobotIndex;
	int m_iRobotCount;
	ITimerComponent::TimerHandle m_hTimeHandle;
    int m_iMaxRobotCount;
    int m_iRobotLoginGap;
    int m_iRobotLoginCountInGap;
};


#endif
