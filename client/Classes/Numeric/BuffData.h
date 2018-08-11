#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stBuffData
{
    public:
    stBuffData();
    virtual ~stBuffData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
};

