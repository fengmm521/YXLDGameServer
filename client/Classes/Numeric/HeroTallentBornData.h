#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stHeroTallentBornData
{
    public:
    stHeroTallentBornData();
    virtual ~stHeroTallentBornData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    string m_Info;
    short m_level;
    char m_quality;
    string m_material;
    int m_sliver;
    short m_levelstep;
    string m_curskillinfo;
    string m_nextskillinfo;
    int m_icon;
    short m_playerlimit;
    string m_skillInfoShow;
};

