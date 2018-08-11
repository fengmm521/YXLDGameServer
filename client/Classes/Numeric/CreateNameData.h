#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stCreateNameData
{
    public:
    stCreateNameData();
    virtual ~stCreateNameData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name1;
    string m_name2;
    string m_name3;
};

