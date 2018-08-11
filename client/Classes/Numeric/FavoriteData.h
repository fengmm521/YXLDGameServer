#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stFavoriteData
{
    public:
    stFavoriteData();
    virtual ~stFavoriteData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    int m_favoriteOne;
    int m_favoriteOneNeed;
    int m_favoriteTwo;
    int m_favoriteTwoNeed;
    int m_favoriteThree;
    int m_favoriteThreeNeed;
    int m_favoriteFour;
    int m_favoriteFourNeed;
    int m_favoriteCount;
    int m_cost;
};

