#include "LegionContributeData.h"

void stLegionContributeData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_silver;
    stream >> m_gold;
    stream >> m_contribution;
  delete tempStr;
}

stLegionContributeData::stLegionContributeData()
{
}

stLegionContributeData::~stLegionContributeData()
{
}

