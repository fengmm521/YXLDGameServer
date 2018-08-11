#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stHeroQualityData
{
    public:
    stHeroQualityData();
    virtual ~stHeroQualityData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_quality1;
    string m_quality2;
    string m_quality3;
    string m_quality4;
    string m_quality5;
    string m_quality6;
    string m_quality7;
    string m_quality8;
    string m_quality9;
    string m_quality10;
    string m_quality11;
    string m_quality12;
    string m_quality13;
};

