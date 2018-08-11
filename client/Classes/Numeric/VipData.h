#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stVipData
{
    public:
    stVipData();
    virtual ~stVipData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_LvlPrivilege;
    int m_BuyTiLiNum;
};

