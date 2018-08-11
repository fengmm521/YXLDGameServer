#include "GodAnimalData.h"

void stGodAnimalData::Decode(Freeman::MemStream& stream)
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
    stream >> m_changeItemId;
    stream >> m_changeNumber;
    stream >> m_changeType;
    stream >> m_cost;
  delete tempStr;
}

stGodAnimalData::stGodAnimalData()
{
}

stGodAnimalData::~stGodAnimalData()
{
}

