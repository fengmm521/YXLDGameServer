#include "HeroCreateData.h"

void stHeroCreateData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_name = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_feature = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_stuntName = tempStr->toUTF8();
    stream >> m_img1;
    stream >> m_img2;
  delete tempStr;
}

stHeroCreateData::stHeroCreateData()
{
}

stHeroCreateData::~stHeroCreateData()
{
}

