#include "ActorSkillData.h"

void stActorSkillData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_level;
    stream >> m_icon;
   stream >> m_cost;
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
  delete tempStr;
}

stActorSkillData::stActorSkillData()
{
}

stActorSkillData::~stActorSkillData()
{
}

