#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stAnnounceMentData
{
    public:
    stAnnounceMentData();
    virtual ~stAnnounceMentData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_title;
    string m_name;
    string m_time;
    string m_info;
};

