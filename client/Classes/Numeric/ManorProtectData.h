#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stManorProtectData
{
    public:
    stManorProtectData();
    virtual ~stManorProtectData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    int m_cost;
    int m_CD;
    int m_time;
    string m_info;
};

