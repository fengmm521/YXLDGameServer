#ifndef IFRIENDSYSTEM_H_
#define IFRIENDSYSTEM_H_


typedef TC_Functor<void, TL::TLMaker<taf::Int32, ServerEngine::NameDesc&>::Result> DelegateName;

class IFriendSystem:public IEntitySubsystem
{
	public:
		virtual bool deleteFriend(string name) = 0;
		virtual bool addRequstToRequestMap(string name,ServerEngine::PKRole role) = 0;
		virtual bool addFriendToMap(string name, ServerEngine::FriendNode node) = 0;
		virtual bool GetOrGivePhyStrength(string name) = 0;
		virtual bool deleteRequest(string strName) = 0;
		virtual void checkNotice() = 0;
		virtual void GMOnlineGetActorBaseInfo(string& strJson)=0;
};


#endif
