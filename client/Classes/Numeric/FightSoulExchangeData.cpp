#include "FightSoulExchangeData.h"

void stFightSoulExchangeData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_fightSoulID;
    stream >> m_count;
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

stFightSoulExchangeData::stFightSoulExchangeData()
{
}

stFightSoulExchangeData::~stFightSoulExchangeData()
{
}

