#include "PaymentData.h"

void stPaymentData::Decode(Freeman::MemStream& stream)
{
    short tempLen = 0;
    ustring* tempStr = new ustring;
    stream >> m_ID;
    stream >> m_PayID;
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_des1 = tempStr->toUTF8();
    tempLen = 0;
    stream >> tempLen;
    tempStr->clear();
    for(int i = 0; i < tempLen; i++)
    {
        tempStr->append(stream.Read_int16());
    }
    m_des2 = tempStr->toUTF8();
    stream >> m_Recommend;
    stream >> m_yuanbao;
  delete tempStr;
}

stPaymentData::stPaymentData()
{
}

stPaymentData::~stPaymentData()
{
}

