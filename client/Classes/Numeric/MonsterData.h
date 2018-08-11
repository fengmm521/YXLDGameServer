#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stMonsterData
{
    public:
    stMonsterData();
    virtual ~stMonsterData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    string m_animName;
    int m_animScale;
    int m_icon;
    char m_monterType;
};

