#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stArenaAwardData
{
    public:
    stArenaAwardData();
    virtual ~stArenaAwardData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_Ranking;
    int m_honor;
    int m_Copper;
    int m_wing;
};

