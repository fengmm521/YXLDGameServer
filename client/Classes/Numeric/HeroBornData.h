#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stHeroBornData
{
    public:
    stHeroBornData();
    virtual ~stHeroBornData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    string m_desc;
    int m_icon;
    short m_img1;
    short m_img2;
    string m_animName;
    int m_animScale;
    int m_heroType;
    int m_skillId;
    int m_attckability;
    int m_defenseability;
    int m_lifeability;
    string m_herotrait;
    string m_heroSoulInfo;
    int m_heroSoulItemId;
    int m_heroNeedSoulNumber;
    int m_heroInitStar;
    int m_heroInitQuality;
    string m_heroPositioning;
    int m_heroSummondCost;
    string m_heroTallent;
};

