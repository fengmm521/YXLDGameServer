#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stCheckInData
{
    public:
    stCheckInData();
    virtual ~stCheckInData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    int m_allTimes;
    int m_isDouble;
    int m_VipConditions;
    string m_Award;
    string m_des;
};

