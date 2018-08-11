#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stHeroLevelExpData
{
    public:
    stHeroLevelExpData();
    virtual ~stHeroLevelExpData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    int m_needxiuwei;
};

