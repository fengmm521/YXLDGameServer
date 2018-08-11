#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stSceneData
{
    public:
    stSceneData();
    virtual ~stSceneData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    string m_path;
    int m_icon;
    string m_desc;
    string m_drop;
    string m_monster;
};

