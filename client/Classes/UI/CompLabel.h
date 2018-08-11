/********************************************************************
created:	2013-1-27
author:		SXH
summary:	复合型文本框，各种格式的文本及图标图块
*********************************************************************/
#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"

NS_FM_BEGIN

USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;

enum EFontType
{
	EFontType_Default,
	EFontType_Menu,
	EFontType_Max,
};

enum EFontSize
{
	EFontSize_Small	= 18,
	EFontSize_Mid	= 24,
	EFontSize_Big	= 30,
};

static std::string s_FontNameList[] =
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	// custom ttf files are defined in Test-info.plist
	//"Helvetica",
	//"FZKATJW--GB1-0",
	"Fonts/FZZhunYuan-M02S.ttf",
	"Fonts/FZZhunYuan-M02S.ttf",
#else
	"Fonts/FZZhunYuan-M02S.ttf",
	"Fonts/FZZhunYuan-M02S.ttf",
#endif
};

static const char* GetFontName(int id)
{
	if(id<0||id>=EFontType_Max)
		id = EFontType_Default;
	return s_FontNameList[id].c_str();
}

class CompLabel
{
public:
	static ccColor3B s_CommonColor;
	static uint s_FontSize;
	static void SetCurColor(const ccColor3B& color ){s_CommonColor.r = color.r;s_CommonColor.g = color.g;s_CommonColor.b = color.b;};
	static void SetFontSize(uint size){s_FontSize = size;}
	static void SetFontSizeDefault(){s_FontSize = EFontSize_Mid;}
	static uint GetFontSize(){return s_FontSize;}

	static void ShowFileString(CCNode* panl,const char* filename,const CCSize& size ,const CCPoint& pos,CCTextAlignment ali);

	static void ShowString(CCNode* panl,const char* filename,const CCSize& size ,const CCPoint& pos,CCTextAlignment ali);

	static Label* GetDefaultLab(const char* str);
	static Label* GetDefaultLabWithSize(const char* str,int size);
	static Label* GetDefaultLabScript(const char* str,int size);
	static Label* GetDefaultLabWithRect(const char* str,int size,const CCSize& recsize);
	static Label* GetDefaultLabWithAnchorRect(const char* str,int size,const CCSize& recsize,CCTextAlignment ali);
	static Label* GetLabWithSize(const char* str,int fontType,int size);
	static Label* GetLab(const char* str,int fontType);
	static Label* GetLabWithAnchorRect(const char* str,int fontType,int size,const CCSize& recsize,CCTextAlignment ali);
	static Label* GetMenuTTFLab(const char* str);


	static Widget* GetMenuLab(const char* str);

	static Widget* GetDefaultCompLabel(const char * str,float wrapwidth);
	static Widget* GetCompLabel(const char * str,float wrapwidth,CCTextAlignment ali = kCCTextAlignmentLeft );
	static Widget* GetCompLabelWithSpaces(const char * str,float wrapwidth,CCTextAlignment ali,float spaceW,float spaceH);
	static Widget* GetCompLabelWithFile(const char * file,float wrapwidth,CCTextAlignment ali );
	static CCNode* createStroke(CCLabelTTF *label, float size, ccColor3B incolor,ccColor3B outcolor);

	static void ConvertToCompStrStyle(string& str);

	static string SetColorInLab(string& str,string colorStr);
	//是否已完成打字
	static bool IsShowAll(Widget* widget);
};

#define  CML_MAX_LINES 100

class StringParser: public CCNodeRGBA, public CCSAXDelegator
{
friend class CompLabel;
public:
	static StringParser *create(void);

	StringParser();
	virtual ~StringParser();

	 //void menuDownAction(CCObject *sender, CCControlEvent controlEvent);

	// void MenuItemImgCallBack(CCObject* pSender);

	// implement pure virtual methods of CCSAXDelegator
	void startElement(void *ctx, const char *name, const char **atts);
	void endElement(void *ctx, const char *name);
	void textHandler(void *ctx, const char *ch, int len);

	void Set(float wrapWidth,CCTextAlignment ali);
	void SetSpaceGap(float w,float h);

	void UpdatePos();

	bool getIsShowAll(){return isShowAll;}
private:
	void ChangeLine();
	void UpdatePos(int line);
	void AppendChild(CCNode* nd);
	void ShowFinish();
private:
	//当前行列 tag
	int		m_curTagX;
	int		m_curTagY;
	float	m_curX;
	float	m_curY;
	float	m_curMaxHeight;

	CCTextAlignment m_Ali;
	float	m_wrapWidth;
	float	m_spaceW;//水平间隔
	float	m_spaceH; //垂直间隔
	float	m_ConstLineHeight; //固定行高
	float	m_delayTime;
	float	m_curChildDelayTime;
	bool isShowAll;//使用打字机时是否已完成打字
};

NS_FM_END