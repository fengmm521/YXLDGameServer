#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stPaymentData
{
    public:
    stPaymentData();
    virtual ~stPaymentData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    int m_PayID;
    string m_des1;
    string m_des2;
    int m_Recommend;
    int m_yuanbao;
};

