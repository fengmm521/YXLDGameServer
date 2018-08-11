#include "ManorProtectData.h"

void stManorProtectData::Decode(Freeman::MemStream& stream)
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
    stream >> m_cost;
    stream >> m_CD;
    stream >> m_time;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_info = tempStr->toUTF8();
  delete tempStr;
}

stManorProtectData::stManorProtectData()
{
}

stManorProtectData::~stManorProtectData()
{
}

