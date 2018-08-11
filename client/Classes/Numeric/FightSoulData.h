#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stFightSoulData
{
    public:
    stFightSoulData();
    virtual ~stFightSoulData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    char m_quality;
};

