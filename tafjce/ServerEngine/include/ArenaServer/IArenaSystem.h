#ifndef __IARENA_SYSTEM_H__
#define __IARENA_SYSTEM_H__

class IArenaSystem:public IEntitySubsystem
{
public:

	// 功能: 添加竞技场次数
	virtual bool addArenaCount() = 0;

	virtual void addArenaLog(const GSProto::ArenaLogItem& logItem, bool bActorIsAtt) = 0;
	virtual void confirmArenaGlodResetCD() = 0;
};


#endif
