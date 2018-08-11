#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stHonorGoodsData
{
    public:
    stHonorGoodsData();
    virtual ~stHonorGoodsData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_type;
    int m_goodID;
    int m_count;
    int m_price;
    int m_costType;
    string m_desc;
};

