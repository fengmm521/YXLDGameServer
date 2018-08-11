#ifndef __BUFFFACTORY_H__
#define __BUFFFACTORY_H__

class BuffCondition_Base;
class BuffFactory:public ComponentBase<IBuffFactory, IID_IBuffFactory>
{
public:

	BuffFactory();
	~BuffFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IBuffFactory Interface
	virtual const IEntityBuff* getBuffPrototype(Int32 iBuffID);

private:
	bool loadFightBuff();
	BuffCondition_Base* parseCondition(const std::string& strCmd, bool bSpecialCondition);
private:

	typedef std::map<int, IEntityBuff*> BuffPrototypeMap;
	BuffPrototypeMap m_buffPrototypeMap;
};


#endif
