#include "CheckInData.h"

void stCheckInData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_allTimes;
    stream >> m_isDouble;
    stream >> m_VipConditions;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_Award = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_des = tempStr->toUTF8();
  delete tempStr;
}

stCheckInData::stCheckInData()
{
}

stCheckInData::~stCheckInData()
{
}

