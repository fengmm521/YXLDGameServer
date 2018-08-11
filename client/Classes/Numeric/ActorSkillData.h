#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stActorSkillData
{
    public:
    stActorSkillData();
    virtual ~stActorSkillData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    short m_level;
    short m_icon;
    char m_cost;
    string m_name;
    string m_desc;
};

