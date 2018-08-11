#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stLegionCityData
{
    public:
    stLegionCityData();
    virtual ~stLegionCityData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    int m_image;
};

