/********************************************************************
created:	2013-3-8
author:		pelog
summary:	∂Øª≠±Ì«È
*********************************************************************/
#pragma once
#include "FmConfig.h"
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;

#define FACES_MAX 20
class FaceMgr
{
public:

	SINGLETON_MODE( FaceMgr );

	FaceMgr();
	virtual ~FaceMgr();
	bool LoadFile(const char* name);
	Widget* GetById(int id);

	void Shutdown();
private:
	CCAnimation* m_Anims[FACES_MAX];
	CCSize m_CtSize;
};