#pragma once
#include "Util/ustring.h"
#include "FmUtil.h"

USING_NS_FM;

struct stSkillEffectData
{
    public:
    stSkillEffectData();
    virtual ~stSkillEffectData();
    void Decode(Freeman::MemStream& stream);

    int m_ID;
    char m_ationType;
    char m_attEffectType;
    string m_attEffectName;
    string m_gasSound;
    string m_attSound;
    short m_attTime;
    char m_bulletType;
    string m_bulletName;
    short m_bulletSpeed;
    char m_targetEffectType;
    string m_targetEffectName;
    string m_targetSound;
    char m_sceneEffect;
};

