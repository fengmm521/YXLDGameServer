#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stPerfectPassAwardData
{
    public:
    stPerfectPassAwardData();
    virtual ~stPerfectPassAwardData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    char m_type;
    int m_goodID;
    int m_count;
    short m_stepLvl;
};

