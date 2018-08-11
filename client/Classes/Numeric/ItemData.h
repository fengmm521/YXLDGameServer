#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stItemData
{
    public:
    stItemData();
    virtual ~stItemData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    char m_quality;
    int m_icon;
    string m_desc;
    string m_descFull;
    int m_useEffect;
    int m_lvLimits;
    int m_sellMoney;
    string m_attinfo;
    int m_type;
};

