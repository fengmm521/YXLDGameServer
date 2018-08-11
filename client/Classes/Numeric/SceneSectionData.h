#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stSceneSectionData
{
    public:
    stSceneSectionData();
    virtual ~stSceneSectionData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_nameFirst;
    string m_name;
    short m_lvlLimit;
    string m_desc;
};

