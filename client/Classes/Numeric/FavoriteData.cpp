#include "FavoriteData.h"

void stFavoriteData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_favoriteOne;
    stream >> m_favoriteOneNeed;
    stream >> m_favoriteTwo;
    stream >> m_favoriteTwoNeed;
    stream >> m_favoriteThree;
    stream >> m_favoriteThreeNeed;
    stream >> m_favoriteFour;
    stream >> m_favoriteFourNeed;
    stream >> m_favoriteCount;
    stream >> m_cost;
  delete tempStr;
}

stFavoriteData::stFavoriteData()
{
}

stFavoriteData::~stFavoriteData()
{
}

