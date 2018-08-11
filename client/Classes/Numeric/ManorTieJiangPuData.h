#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stManorTieJiangPuData
{
    public:
    stManorTieJiangPuData();
    virtual ~stManorTieJiangPuData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    int m_cost;
    int m_timeLevelUp;
    int m_timeProduct;
    int m_boxNum;
};

