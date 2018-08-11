#include "ManorResLevelData.h"

void stManorResLevelData::Decode(Freeman::MemStream& stream)
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
    stream >> m_output;
    stream >> m_timeLevelUp;
    stream >> m_store;
    stream >> m_level;
    stream >> m_cost;
    stream >> m_resID;
  delete tempStr;
}

stManorResLevelData::stManorResLevelData()
{
}

stManorResLevelData::~stManorResLevelData()
{
}

