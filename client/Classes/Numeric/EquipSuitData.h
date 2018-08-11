#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stEquipSuitData
{
    public:
    stEquipSuitData();
    virtual ~stEquipSuitData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    string m_equipList;
    string m_att1;
    string m_att2;
    string m_att3;
};

