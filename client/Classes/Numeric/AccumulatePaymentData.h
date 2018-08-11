#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stAccumulatePaymentData
{
    public:
    stAccumulatePaymentData();
    virtual ~stAccumulatePaymentData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_RewardOne;
    string m_RewardTwo;
    string m_RewardTree;
};

