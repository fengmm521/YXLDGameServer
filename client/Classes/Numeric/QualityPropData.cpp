#include "QualityPropData.h"

void stQualityPropData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_color;
    stream >> m_Pingjie;
  delete tempStr;
}

stQualityPropData::stQualityPropData()
{
}

stQualityPropData::~stQualityPropData()
{
}

