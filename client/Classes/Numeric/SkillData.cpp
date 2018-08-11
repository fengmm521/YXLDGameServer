#include "SkillData.h"

void stSkillData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
   stream >> m_quality;
    stream >> m_openlevelstep;
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
    m_name1 = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_name2 = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_desc = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_fullDesc = tempStr->toUTF8();
   stream >> m_type;
   stream >> m_rangeType;
   stream >> m_bMultiDamage;
    stream >> m_icon;
    stream >> m_skillEffect;
  delete tempStr;
}

stSkillData::stSkillData()
{
}

stSkillData::~stSkillData()
{
}

