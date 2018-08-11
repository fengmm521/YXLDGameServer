#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stGodAnimalData
{
    public:
    stGodAnimalData();
    virtual ~stGodAnimalData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    string m_name;
    string m_desc;
    int m_icon;
    int m_img1;
    int m_img2;
    string m_animName;
    int m_animScale;
    int m_changeItemId;
    int m_changeNumber;
    int m_changeType;
    int m_cost;
};

