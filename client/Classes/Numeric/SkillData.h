#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stSkillData
{
    public:
    stSkillData();
    virtual ~stSkillData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    char m_quality;
    int m_openlevelstep;
    string m_name;
    string m_name1;
    string m_name2;
    string m_desc;
    string m_fullDesc;
    char m_type;
    char m_rangeType;
    char m_bMultiDamage;
    short m_icon;
    short m_skillEffect;
};

