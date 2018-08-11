#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stAccumlateLoginData
{
    public:
    stAccumlateLoginData();
    virtual ~stAccumlateLoginData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_RewardOne;
    string m_RewardTwo;
    string m_RewardTree;
    string m_RewardFour;
};

