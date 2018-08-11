#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stHeroCreateData
{
    public:
    stHeroCreateData();
    virtual ~stHeroCreateData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    string m_feature;
    string m_stuntName;
    short m_img1;
    short m_img2;
};

