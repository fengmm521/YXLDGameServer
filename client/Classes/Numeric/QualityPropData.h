#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stQualityPropData
{
    public:
    stQualityPropData();
    virtual ~stQualityPropData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    int m_color;
    int m_Pingjie;
};

