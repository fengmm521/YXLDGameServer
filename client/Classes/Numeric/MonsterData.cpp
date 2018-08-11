#include "MonsterData.h"

void stMonsterData::Decode(Freeman::MemStream& stream)
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
    m_animName = tempStr->toUTF8();
    stream >> m_animScale;
    stream >> m_icon;
   stream >> m_monterType;
  delete tempStr;
}

stMonsterData::stMonsterData()
{
}

stMonsterData::~stMonsterData()
{
}

