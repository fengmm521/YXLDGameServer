#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stEquipData
{
    public:
    stEquipData();
    virtual ~stEquipData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    char m_quality;
    char m_step;
    short m_lvlLimit;
    char m_equipPos;
    int m_icon;
    char m_sellType;
    int m_sellCount;
    string m_drop;
    int m_zhiyeLimit;
    string m_attinfo;
    string m_desc;
};

