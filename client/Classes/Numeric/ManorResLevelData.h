#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stManorResLevelData
{
    public:
    stManorResLevelData();
    virtual ~stManorResLevelData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    int m_output;
    int m_timeLevelUp;
    int m_store;
    short m_level;
    int m_cost;
    short m_resID;
};

