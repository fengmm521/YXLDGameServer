#include "HeroLevelExpData.h"

void stHeroLevelExpData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_needxiuwei;
  delete tempStr;
}

stHeroLevelExpData::stHeroLevelExpData()
{
}

stHeroLevelExpData::~stHeroLevelExpData()
{
}

