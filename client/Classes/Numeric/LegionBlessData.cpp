#include "LegionBlessData.h"

void stLegionBlessData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_silver;
    stream >> m_gold;
    stream >> m_contribution;
    stream >> m_physicalpower;
  delete tempStr;
}

stLegionBlessData::stLegionBlessData()
{
}

stLegionBlessData::~stLegionBlessData()
{
}

