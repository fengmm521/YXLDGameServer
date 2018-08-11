#include "LevelStepGrowData.h"

void stLevelStepGrowData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_CostNumber;
    stream >> m_needSliver;
   stream >> m_quality;
    stream >> m_step;
  delete tempStr;
}

stLevelStepGrowData::stLevelStepGrowData()
{
}

stLevelStepGrowData::~stLevelStepGrowData()
{
}

