#ifndef   __GAMEDATA_INTERFACE_H__
#define  __GAMEDATA_INTERFACE_H__
NS_FM_BEGIN
class IGameData
{
public:
	virtual void* GetData(int key) = 0;

	virtual const vector<int>* GetKeyList() = 0;
};
NS_FM_END
#endif