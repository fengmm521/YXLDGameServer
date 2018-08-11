#include "HonorGoodsData.h"

void stHonorGoodsData::Decode(Freeman::MemStream& stream)
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
    m_type = tempStr->toUTF8();
    stream >> m_goodID;
    stream >> m_count;
    stream >> m_price;
    stream >> m_costType;
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

stHonorGoodsData::stHonorGoodsData()
{
}

stHonorGoodsData::~stHonorGoodsData()
{
}

