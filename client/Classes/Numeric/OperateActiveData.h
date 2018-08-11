#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stOperateActiveData
{
    public:
    stOperateActiveData();
    virtual ~stOperateActiveData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_activityIcon;
    string m_activityName;
    string m_activityDes;
    string m_activityInfo;
    string m_activityUpIcon;
    int m_activityType;
};

