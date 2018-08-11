#include "PerfectPassAwardData.h"

void stPerfectPassAwardData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
   stream >> m_type;
    stream >> m_goodID;
    stream >> m_count;
    stream >> m_stepLvl;
  delete tempStr;
}

stPerfectPassAwardData::stPerfectPassAwardData()
{
}

stPerfectPassAwardData::~stPerfectPassAwardData()
{
}

