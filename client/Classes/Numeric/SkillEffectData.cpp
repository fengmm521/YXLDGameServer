#include "SkillEffectData.h"

void stSkillEffectData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
   stream >> m_ationType;
   stream >> m_attEffectType;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_attEffectName = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_gasSound = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_attSound = tempStr->toUTF8();
    stream >> m_attTime;
   stream >> m_bulletType;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_bulletName = tempStr->toUTF8();
    stream >> m_bulletSpeed;
   stream >> m_targetEffectType;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_targetEffectName = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_targetSound = tempStr->toUTF8();
   stream >> m_sceneEffect;
  delete tempStr;
}

stSkillEffectData::stSkillEffectData()
{
}

stSkillEffectData::~stSkillEffectData()
{
}

