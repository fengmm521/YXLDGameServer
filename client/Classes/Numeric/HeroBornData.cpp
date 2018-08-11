#include "HeroBornData.h"

void stHeroBornData::Decode(Freeman::MemStream& stream)
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
    m_desc = tempStr->toUTF8();
    stream >> m_icon;
    stream >> m_img1;
    stream >> m_img2;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_animName = tempStr->toUTF8();
    stream >> m_animScale;
    stream >> m_heroType;
    stream >> m_skillId;
    stream >> m_attckability;
    stream >> m_defenseability;
    stream >> m_lifeability;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_herotrait = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_heroSoulInfo = tempStr->toUTF8();
    stream >> m_heroSoulItemId;
    stream >> m_heroNeedSoulNumber;
    stream >> m_heroInitStar;
    stream >> m_heroInitQuality;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_heroPositioning = tempStr->toUTF8();
    stream >> m_heroSummondCost;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_heroTallent = tempStr->toUTF8();
  delete tempStr;
}

stHeroBornData::stHeroBornData()
{
}

stHeroBornData::~stHeroBornData()
{
}

