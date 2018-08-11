#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stFightSoulExchangeData
{
    public:
    stFightSoulExchangeData();
    virtual ~stFightSoulExchangeData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    int m_fightSoulID;
    short m_count;
    string m_desc;
};

