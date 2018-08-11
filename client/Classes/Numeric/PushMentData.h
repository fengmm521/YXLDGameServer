#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stPushMentData
{
    public:
    stPushMentData();
    virtual ~stPushMentData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_Title;
    string m_info;
    string m_time;
};

