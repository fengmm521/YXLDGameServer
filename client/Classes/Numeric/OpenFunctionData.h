#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stOpenFunctionData
{
    public:
    stOpenFunctionData();
    virtual ~stOpenFunctionData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_icon;
    string m_info;
    int m_yindao;
};

