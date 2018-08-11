#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stTaskData
{
    public:
    stTaskData();
    virtual ~stTaskData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    string m_info;
    string m_rewardTypeone;
    string m_rewardTypetwo;
    string m_rewardTypetree;
    string m_Relevance;
    int m_Relevance2;
    string m_Icon;
};

