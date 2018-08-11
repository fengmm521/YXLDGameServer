#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stDreamLandSectionData
{
    public:
    stDreamLandSectionData();
    virtual ~stDreamLandSectionData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
};

