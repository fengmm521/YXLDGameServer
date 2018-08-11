#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stGiftData
{
    public:
    stGiftData();
    virtual ~stGiftData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_title;
    string m_reward;
};

