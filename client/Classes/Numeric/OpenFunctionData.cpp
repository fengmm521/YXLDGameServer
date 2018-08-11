#include "OpenFunctionData.h"

void stOpenFunctionData::Decode(Freeman::MemStream& stream)
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
    m_icon = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_info = tempStr->toUTF8();
    stream >> m_yindao;
  delete tempStr;
}

stOpenFunctionData::stOpenFunctionData()
{
}

stOpenFunctionData::~stOpenFunctionData()
{
}

