#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stLevelStepGrowData
{
    public:
    stLevelStepGrowData();
    virtual ~stLevelStepGrowData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    int m_CostNumber;
    int m_needSliver;
    char m_quality;
    int m_step;
};

