#include "FmStringUtil.h"
#include "CompLabel.h"
//#include "Animation/FaceMgr.h"
#include <map>
#include "FmStringUtil.h"
#include "FmVar.h"
#include "Util/FmStringMgr.h"
#include "Util/comUTF.h"
#include "UI/FmUIMgr.h"
//#include "Asset/Launcher.h"
NS_FM_BEGIN


StringParser * StringParser::create( void )
{
	StringParser* parser = new StringParser();
	if (parser && parser->init())
	{
		parser->setCascadeColorEnabled(true);
		parser->setCascadeOpacityEnabled(true);
		parser->autorelease();
		return parser;
	}
	else
	{
		CC_SAFE_DELETE(parser);
		return NULL;
	}
}

StringParser::StringParser()
{
	m_curTagX = 0;
	m_curTagY = 0;
	m_curX = 0;
	m_curY = 0;
	m_curMaxHeight = 0;
	m_delayTime = 50.0f;
	m_curChildDelayTime = 0.0f;

	m_wrapWidth = CCDirector::sharedDirector()->getWinSize().width;
	m_spaceW = 1;//水平间隔
	m_spaceH = 1; //垂直间隔
	m_Ali = kCCTextAlignmentLeft;//kCCTextAlignmentCenter;
	m_ConstLineHeight = 0;
	setAnchorPoint(ccp(0,0));
}

StringParser::~StringParser()
{

}



static const char* ValueForKey(const char *key, std::map<std::string, std::string>* dict)
{
	if (dict)
	{
		std::map<std::string, std::string>::iterator it = dict->find(key);
		return it!=dict->end() ? it->second.c_str() : "";
	}
	return "";
}

//<s><lb nm="A" sz="25" cl="250 100 22" c="BBB大师法BBBBBBBBBBBBBBBBBBB"></lb><lb nm="B" sz="50" cl="250 100 250" c="A放大AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"></lb></s>
void StringParser::startElement( void *ctx, const char *name, const char **atts )
{
	CC_UNUSED_PARAM(ctx);

	m_delayTime = 0;

	if(strcmp(name,"clh") == 0)
	{
		if(atts && atts[0])
		{
			for(int i = 0; atts[i]; i += 2) 
			{
				std::string key = (char*)atts[i];
				std::string value = (char*)atts[i+1];
				if(key.compare("h")==0)
				{
					m_ConstLineHeight = (float)atof(value.c_str());
				}
			}
		}
	}

	//强制换行
	if(strcmp(name,"n") == 0)
	{
		if(m_curMaxHeight <= 1)
		{
			m_curMaxHeight = EFontSize_Mid;
		}
		ChangeLine();
	}
	else if(strcmp(name,"hi") == 0)
	{
		float scale = 1;
		int headId = -1;
		if(atts && atts[0])
		{
			for(int i = 0; atts[i]; i += 2) 
			{
				std::string key = (char*)atts[i];
				std::string value = (char*)atts[i+1];
				if(key.compare("c")==0)
				{
					headId = (int)atoi(value.c_str());
				}
				else if( key.compare("s")==0)
				{
					scale = (float)atof(value.c_str());
				}
			}
		}
		/*CCNode* headspr = UIMgr::GetInstance().GetHeadIcon(headId);
		if(headspr)
		{
			headspr->setScale(scale);
			headspr->setAnchorPoint(ccp(0,1));
			AppendChild(headspr);
		}*/
	}
	//物品图标
	else if(strcmp(name,"ii") == 0)
	{
		int itemId = -1;
		float scale = 1;
		if(atts && atts[0])
		{
			for(int i = 0; atts[i]; i += 2) 
			{
				std::string key = (char*)atts[i];
				std::string value = (char*)atts[i+1];
				if(key.compare("c")==0)
				{
					itemId = (int)atoi(value.c_str());
				}
				else if(key.compare("s")==0)
				{
					scale = (float)atof(value.c_str());
				}
			}
		}
		/*CCNode* itemspr = UIMgr::GetInstance().GetItemIcon(itemId);
		if(itemspr)
		{
			itemspr->setAnchorPoint(ccp(0,1));
			itemspr->setScale(scale);
			AppendChild(itemspr);
		}*/
	}
	//表情
	else if(strcmp(name,"f") == 0)
	{
		int fid = -1;
		float scale = 1;
		if(atts && atts[0])
		{
			for(int i = 0; atts[i]; i += 2) 
			{
				std::string key = (char*)atts[i];
				std::string value = (char*)atts[i+1];
				if(key.compare("c")==0)
				{
					fid = (int)atoi(value.c_str());
				}				
				else if(key.compare("s")==0)
				{
					scale = (float)atof(value.c_str());
				}
			}
		}
		/*CCSprite* face = FaceMgr::GetInstance().GetById(fid);
		if(face)
		{
			face->setScale(scale);
			face->setAnchorPoint(ccp(0,1));
			AppendChild(face);
		}*/
	}
	//图片
	else if(strcmp(name,"p") == 0)
	{
		string plist="";
		string path = "";
		int cmd = 0;
		float scale = 1;
		if(atts && atts[0])
		{
			for(int i = 0; atts[i]; i += 2) 
			{
				std::string key = (char*)atts[i];
				std::string value = (char*)atts[i+1];
				if(key.compare("c")==0)
				{
					path = value.c_str();
				}
				else if(key.compare("s")==0)
				{
					scale = (float)atof(value.c_str());
				}				
				else if(key.compare("l")==0)
				{
					plist = value.c_str();
				}
			}
		}
		
		CCSprite* spr = NULL;
		if (plist.compare("") == 0)
		{
			spr = CCSprite::create(path.c_str());
		}
		else
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plist.c_str());
			spr =  CCSprite::createWithSpriteFrameName(path.c_str());
		}

		if(spr)
		{
			spr->setCascadeColorEnabled(true);
			spr->setCascadeOpacityEnabled(true);
			spr->setScale(scale);
			AppendChild(spr);
		}
	}
	//文本label
	else if(strcmp(name,"lb") == 0)
	{
		std::string elementName = (char*)name;
		std::map<std::string, std::string> pDict;

		if(atts && atts[0])
		{
			for(int i = 0; atts[i]; i += 2) 
			{
				std::string key = (char*)atts[i];
				std::string value = (char*)atts[i+1];
				pDict.insert(pair<std::string, std::string>(key, value));
			}
		}
		//字符内容
		std::string pStr = ValueForKey("c",&pDict);
		//字体大小
		int size =  (int)atoi(ValueForKey("sz",&pDict));
		if(size == 0)
			size = CompLabel::GetFontSize();
		//字体颜色
		std::string clors = ValueForKey("cl",&pDict);
		ccColor3B color = ccc3(CompLabel::s_CommonColor.r,CompLabel::s_CommonColor.g,CompLabel::s_CommonColor.b);

		if(clors.length() > 3)
		{
			std::vector<std::string> cs = Freeman::StringUtil::Split( clors, " " );
			if(cs.size() == 3)
			{
				color.r =(int)atoi(cs[0].c_str());
				color.g =(int)atoi(cs[1].c_str());
				color.b =(int)atoi(cs[2].c_str());
			}
		}
		int delayTime = (int)atoi(ValueForKey("de",&pDict));
		if (delayTime >= 0)
		{
			m_delayTime = delayTime;
		}

		//字体名，雅黑or卡通
		int fontType = (int)atoi(ValueForKey("t",&pDict));
		if(fontType < 0 || fontType > EFontType_Max)
			fontType = EFontType_Default;

		//是否描边
		bool isStroke = false;
		int skSize = 1;
		int skType = 0;
		ccColor3B skColor = ccc3(0,0,0);
		std::string sks = ValueForKey("sk",&pDict);
		if(sks.length() > 3)
		{
			isStroke = true;
			std::vector<std::string> cs = Freeman::StringUtil::Split( sks, " " );
			if(cs.size() >= 4)
			{
				skSize = (int)atoi(cs[0].c_str());
				skColor.r =(int)atoi(cs[1].c_str());
				skColor.g =(int)atoi(cs[2].c_str());
				skColor.b =(int)atoi(cs[3].c_str());
			}
			if(cs.size() == 5)
			{
				skType  =(int)atoi(cs[4].c_str());
			}
		}

		
		int menuId =  (int)atoi(ValueForKey("m",&pDict)); 
		if(menuId > 0)//菜单项，一次型生成 CCControlButton
		{
			/*const char* bg = ValueForKey("mbg",&pDict);
			CCScale9Sprite *backgroundButton = NULL;
			if(bg && strlen(bg) > 1)
				backgroundButton = CCScale9Sprite::create(bg);
			if(backgroundButton == NULL)
				backgroundButton = CCScale9Sprite::create("ImgUi/button/btnk1_1.png");
			CCLabelTTF* titleButton = CompLabel::GetLabWithSize( pStr.c_str(),fontType,size );
			CCControlButton *button = CCControlButton::create(titleButton, backgroundButton);
			button->addTargetWithActionForControlEvents(this, cccontrol_selector(StringParser::menuDownAction), CCControlEventTouchDown);
			button->setAnchorPoint(ccp(0,1));
			button->setMargins(28,16);
			titleButton->setTag(menuId);
			AppendChild(button);*/
		}
		else//一个一个字符排列
		{
			const char *p = pStr.c_str();
			int len = 0;
			const char *cp = p;
			while (*p)
			{
				cp = COM_utf8_next_char (p);
				int nLen = cp - p ;
				if(nLen > 6)
				{
					CCLog("String Encode Utf Error %s " , pStr.c_str());
					break;
				}

				if( nLen == 1 && p[0]=='\\')
				{
					const char* nc = COM_utf8_next_char (cp);
					if(!*nc)
					{
						if(cp[0]=='n')
						{
							ChangeLine();
						}
						break;
					}

					int nLen1 = nc - cp ;
					if(nLen1 > 6)
					{
						CCLog("String Encode Utf Error %s " , pStr.c_str());
						break;
					}

					if(nLen1 == 1 && cp[0]=='n')
					{
						ChangeLine();
						p = nc;
						continue;
					}
				}

				char str[8] = {};
				memcpy(str, p, nLen);
				str[nLen] ='\0';
				p = cp;

				CCLabelTTF* tl = CCLabelTTF::create(str, GetFontName(fontType), size);//CCLabelTTF*)CompLabel::GetLabWithSize( str,fontType,size )->getVirtualRenderer();//CompLabel::GetDefaultLabWithSize(str, size);
				

				if(isStroke)
				{
					//tl->enableShadow(CCSizeMake(2, 2), 200, true);
					tl->enableStroke(ccc3(0, 0, 0), 1);
				}

				tl->setAnchorPoint(ccp(0,1));
				tl->setColor(color);
				AppendChild(tl);
			}
			isShowAll = false;
			if (m_curChildDelayTime != 0){ 
				CCSequence* pAction = CCSequence::createWithTwoActions(CCDelayTime::create(m_curChildDelayTime/1000), 
					CCCallFunc::create(this,callfunc_selector(StringParser::ShowFinish)));
				this->runAction(pAction);
			}
		}
	}
}
static CCSize GetNodeBoundSize(CCNode* node)
{
	if(node)
	{
		CCSize size = node->getContentSize();
		size.width *= node->getScaleX();
		size.height *= node->getScaleY();

		//真机bug，获取大小不匹配
		CCLabelTTF* tf = dynamic_cast<CCLabelTTF*>(node);
		if(tf)
		{
			size.height = tf->getFontSize();
		//	CCSize dmz = tf->getDimensions();
		//	float fz = tf->getFontSize();
		//	
		//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		//	CCLOG("Win32 TTF Dms(%f,%f) contentSize( %f %f)  FontSize(%f)",dmz.width,dmz.height,size.width,size.height,fz);
		//#else
		//	CCLOG("Not Win32 TTF Dms(%f,%f) contentSize( %f %f)  FontSize(%f)",dmz.width,dmz.height,size.width,size.height,fz);
		//#endif
		}

		return size;
	}
	return CCSizeMake(0,0);
}

void StringParser::AppendChild(CCNode* nd)
{
	if(nd == NULL){
		return;
	}
	CCSize ts = GetNodeBoundSize(nd);//nd->boundingBox().size;
	if(m_curX + ts.width > m_wrapWidth )
	{
		ChangeLine();
	}
	nd->setAnchorPoint(ccp(0,0));

	if (m_delayTime > 0)
	{
		//nd->setVisible(false);
		CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(nd);

		if (pRGBAProtocol && nd)
		{
			pRGBAProtocol->setOpacity(0);

			CCSequence* pAction = CCSequence::createWithTwoActions(CCDelayTime::create(m_curChildDelayTime/1000), 
				CCFadeIn::create(0.1));

			nd->runAction(pAction);

			m_curChildDelayTime += m_delayTime;
		}
	}

	addChild(nd,0,m_curTagY*100 + m_curTagX);
	m_curTagX++;

	if(m_curMaxHeight < ts.height)
		m_curMaxHeight = ts.height;
	m_curX += ts.width + m_spaceW;

}
void StringParser::ShowFinish(){
	isShowAll = true;
}
void StringParser::UpdatePos()
{
	float reWidth = m_wrapWidth;
	if(m_curTagY == 0 && m_curX < m_wrapWidth) //无需换行，需要重新计算容器大小
	{
		reWidth = m_curX;
	}
	UpdatePos(m_curTagY);
	setContentSize(CCSizeMake(reWidth,-m_curY));
}

void StringParser::UpdatePos( int line )
{
	float lineHeight = m_ConstLineHeight==0?m_curMaxHeight:m_ConstLineHeight;
	float x = 0; 
	if(m_Ali == kCCTextAlignmentCenter)
	{
		x = (m_wrapWidth - m_curX)/2;
	}
	else if(m_Ali == kCCTextAlignmentRight)
	{
		x = m_wrapWidth - m_curX;
	}

	for(int i = line*100; i < line*100 + m_curTagX; i++)
	{
		CCNode* nd = getChildByTag(i);
		if(nd)
		{	
			CCSize ts =GetNodeBoundSize(nd);// nd->boundingBox().size;//nd->getContentSize();
//真机字体bug
			float fixY = 0;
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
			CCLabelTTF* lab = dynamic_cast<CCLabelTTF*>(nd);
			if(lab)
				fixY = 3;
#endif

			float lineGap = (ts.height - lineHeight)/2; //上下居中
			nd->setPosition(ccp(x,m_curY + lineGap + fixY));
			nd->setAnchorPoint(ccp(0,1));
			x += ts.width + m_spaceW;
		}
	}
	m_curY  -= lineHeight + m_spaceH;
}

void StringParser::ChangeLine()
{
	UpdatePos(m_curTagY);
	m_curX = 0;
	//m_curMaxHeight = 0;
	m_curTagX = 0;
	m_curTagY++;
}


void StringParser::endElement( void *ctx, const char *name )
{
	CC_UNUSED_PARAM(ctx);
//	this->setContentSize(CCSizeMake(m_Rect.width,fabsf(m_CurY)));
}

void StringParser::textHandler( void *ctx, const char *ch, int len )
{
	CC_UNUSED_PARAM(ctx);
}


void StringParser::Set( float wrapWidth,CCTextAlignment ali )
{
	m_Ali = ali;
	m_wrapWidth = wrapWidth;
	if(m_wrapWidth <= 0)
		m_wrapWidth = CCDirector::sharedDirector()->getWinSize().width;
}

void StringParser::SetSpaceGap( float w,float h )
{
	m_spaceW = w;
	m_spaceH = h;
}
/*
void StringParser::menuDownAction( CCObject *sender, CCControlEvent controlEvent )
{
	CCControlButton* ctrl = dynamic_cast<CCControlButton*>(sender);
	CCNode* labNode = ctrl->getTitleLabel();
	int menuId = 0;
	if(labNode)
	{
		menuId = labNode->getTag();
		if(menuId > 0 && menuId < 100)
			Launcher::GetInstance()->HandleMessageBox(menuId);
	}
	CCLog("Menu Down ...%d",menuId);
}

void StringParser::MenuItemImgCallBack( CCObject* pSender )
{
	CCNode* node = (CCNode*) pSender;
	if(node)
	{
		int menuId = node->getTag();
		if(menuId > 0 && menuId < 100)
			Launcher::GetInstance()->HandleMessageBox(menuId);
		CCLog("MenuItem Down ... %d",menuId);
	}
}
*/



ccColor3B CompLabel::s_CommonColor = ccWHITE; //ccc3(222,222,222);
uint CompLabel::s_FontSize = EFontSize_Mid;

void CompLabel::ShowFileString( CCNode* panl,const char* filename,const CCSize& size ,const CCPoint& pos,CCTextAlignment ali )
{
	CCNode* sp = GetCompLabelWithFile(filename,size.width,ali);
	sp->setPosition(pos);
	float ch = sp->getContentSize().height;
	panl->addChild(sp,10,-1);
}

void CompLabel::ShowString( CCNode* panl,const char* patstr,const CCSize& size ,const CCPoint& pos,CCTextAlignment ali )
{
	CCNode* sp = GetCompLabel( patstr,size.width,ali );
	sp->setPosition(pos);
	float ch = sp->getContentSize().height;
	panl->addChild(sp,10,-1);
}

Label* CompLabel::GetDefaultLab( const char* str )
{
	Label* lab = Label::create();
	lab->setFontSize(EFontSize_Mid);
	lab->setText(str);
	//lab->setFontName(GetFontName(EFontType_Default));
	lab->setAnchorPoint(ccp(0.5,0.5));
	lab->setColor(s_CommonColor);
	return lab;
}

Label* CompLabel::GetDefaultLabWithSize( const char* str,int size )
{
	Label* lab = Label::create();
	lab->setFontSize(size);
	lab->setText(str);
	//lab->setFontName(GetFontName(EFontType_Default));
	lab->setAnchorPoint(ccp(0.5,0.5));
	lab->setColor(s_CommonColor);

	//float strokeSize = 5;
	//CCSize textureSize = lab->getContentSize();
	//textureSize.width += 2 * strokeSize;
	//textureSize.height += 2 * strokeSize;
	////call to clear error
	//glGetError();
	//CCRenderTexture *rt = CCRenderTexture::create(textureSize.width, textureSize.height);

	//lab->setColor(ccc3(0, 0, 0));

	//ccBlendFunc originalBlend = lab->getBlendFunc();
	//ccBlendFunc func = { GL_SRC_ALPHA, GL_ONE};
	//lab->setBlendFunc(func);

	//lab->setAnchorPoint(ccp(0.5, 0.5));

	//rt->begin();
	//for(int i = 0; i < 360; i += 15)
	//{
	//	float r = CC_DEGREES_TO_RADIANS(i);
	//	lab->setPosition(ccp(
	//		textureSize.width * 0.5f + sin(r) * strokeSize,
	//		textureSize.height * 0.5f + cos(r) * strokeSize));
	//	lab->visit();
	//}
	//lab->setColor(s_CommonColor);
	//lab->setBlendFunc(originalBlend);
	//lab->setPosition(ccp(textureSize.width * 0.5f, textureSize.height * 0.5f));
	//lab->visit();
	//rt->end();

	//CCTexture2D *texture = rt->getSprite()->getTexture();
	//texture->setAliasTexParameters();
	//CCSprite* spr = CCSprite::createWithTexture(rt->getSprite()->getTexture());
	//lab->setContentSize(spr->getContentSize());
	//spr->setAnchorPoint(ccp(0, 0));
	//spr->setPosition(ccp(0, 0));
	//((CCSprite *)spr)->setFlipY(true);

	//lab->addChild(spr);

	return lab;
}

Label* CompLabel::GetDefaultLabScript( const char* str,int size )
{
	return CompLabel::GetDefaultLabWithSize( str, size );
}

Label* CompLabel::GetDefaultLabWithAnchorRect( const char* str,int size,const CCSize& recsize,CCTextAlignment ali )
{
	//CCLabelTTF* lab = CCLabelTTF::create(str,GetFontName(EFontType_Default),size,recsize,ali);
	Label* lab = Label::create();
	lab->setFontSize(size);
	lab->setText(str);
	//lab->setFontName(GetFontName(EFontType_Default));
	lab->setTextAreaSize(recsize);
	lab->setTextHorizontalAlignment(ali);
	lab->setAnchorPoint(ccp(0.5,0.5));
	lab->setColor(s_CommonColor);
	return lab;
}

Label* CompLabel::GetDefaultLabWithRect( const char* str,int size,const CCSize& recsize )
{
	return GetDefaultLabWithAnchorRect(str,size,recsize,kCCTextAlignmentLeft);
}

Widget* CompLabel::GetMenuLab( const char* str )
{
	vector<Var> args;
	args.push_back( Var(str) );
	string ns = StringMgr::GetInstance().FormatString( "CompMenuMake", args );
	Widget* lab = GetDefaultCompLabel(ns.c_str(),0);
	//CCLabelTTF* lab = GetLabWithSize( str,EFontType_Menu,EFontSize_Mid );// CCLabelTTF::create(str,GetFontName(EFontType_Menu),EFontSize_Mid);
	//lab->setAnchorPoint(ccp(0,0));
	//lab->setColor(ccc3(255, 255, 255));
	////lab->enableStroke(ccc3(200,10,0),3);
	return lab;
}


Widget* CompLabel::GetDefaultCompLabel( const char * str,float wrapwidth )
{
	return GetCompLabel( str,wrapwidth,kCCTextAlignmentLeft);
}

Widget* CompLabel::GetCompLabel( const char * str,float wrapwidth,CCTextAlignment ali  )
{
	return GetCompLabelWithSpaces(str,wrapwidth,ali,1,1);
}



void CompLabel::ConvertToCompStrStyle( string& str )
{
	if(StringUtil::StartsWith(str,"<s>")&&StringUtil::EndsWith(str,"</s>"))
	{

	}
	else //if(ts.find("<s>") == std::string::npos && ts.find("</s>") == std::string::npos)
	{
		if (
			StringUtil::StartsWith(str,"<lb") &&StringUtil::EndsWith(str,"/>") 
			||StringUtil::StartsWith(str,"<p") &&StringUtil::EndsWith(str,"/>") 
			||StringUtil::StartsWith(str,"<hi") &&StringUtil::EndsWith(str,"/>") 
			||StringUtil::StartsWith(str,"<ii") &&StringUtil::EndsWith(str,"/>") 
			||StringUtil::StartsWith(str,"<f") &&StringUtil::EndsWith(str,"/>") 
			)
		{
			vector<Var> args;
			args.push_back( Var( str.c_str() ) );
			str = StringMgr::GetInstance().FormatString( "CompLabPack", args );
		}
		else
		{
			vector<Var> args;
			args.push_back( Var( str.c_str() ) );
			str = StringMgr::GetInstance().FormatString( "CommonLabel", args );
		}
	}
}

Widget* CompLabel::GetCompLabelWithSpaces( const char * str,float wrapwidth,CCTextAlignment ali,float spaceW,float spaceH )
{
	CCSAXParser parser;
	if (false == parser.init("UTF-8") )
	{
		return NULL;
	}

	std::string ts = str;
	ConvertToCompStrStyle(ts);
	
	//replace tab sign for android <Android Crash bug>
	string searchString( "	" ); 
	string replaceString( " " );
	string::size_type pos = 0;
	while ( (pos = ts.find(searchString, pos)) != string::npos ) {
		ts.replace( pos, searchString.size(), replaceString );
		pos++;
	}

	//if(StringUtil::StartsWith(ts,"<s>")&&StringUtil::EndsWith(ts,"</s>"))
	//{

	//}
	//else //if(ts.find("<s>") == std::string::npos && ts.find("</s>") == std::string::npos)
	//{
	//	if (StringUtil::StartsWith(ts,"<lb"))
	//	{
	//		vector<Var> args;
	//		args.push_back( Var( str ) );
	//		ts = StringMgr::GetInstance().FormatString( "CompLabPack", args );
	//	}
	//	else
	//	{
	//		vector<Var> args;
	//		args.push_back( Var( str ) );
	//		ts = StringMgr::GetInstance().FormatString( "CommonLabel", args );
	//	}
	//}


	int len = strlen(ts.c_str());
	
	StringParser* sp = StringParser::create();
	sp->Set(wrapwidth,ali);
	sp->SetSpaceGap(spaceW,spaceH);
	parser.setDelegator(sp);

	/*
	//encodeing check log
	CCLog("CompString Len %d >>> %s ",len,ts.c_str());
	string memBuf;
	const char* p = ts.c_str();
	for(int i = 0; i < len; i++)
	{
		char tmp[64] = { 0 };
		sprintf(tmp,"%x ",p[i]);
		memBuf += tmp;
	}
	CCLog("MEM: >>> %s ",memBuf.c_str());
	*/
	if(parser.parse(ts.c_str(),len) == false)
		return NULL;
	sp->UpdatePos();
	sp->ignoreAnchorPointForPosition(true);
	sp->setPosition(ccp(0, sp->getContentSize().height));
	Layout* widget = Layout::create();
	widget->setSize(sp->getContentSize());
	widget->addNode(sp);
	widget->setAnchorPoint(ccp(0, 1));
	return widget;
}


Widget* CompLabel::GetCompLabelWithFile( const char * file ,float wrapwidth,CCTextAlignment ali )
{
	CCSAXParser parser;
	if (false == parser.init("UTF-8") )
	{
		return NULL;
	}
	StringParser* sp = StringParser::create();
	sp->Set(wrapwidth,ali);
	parser.setDelegator(sp);
	if(parser.parse(file) == false)
	{
		CCLog("Parse Error %s ",file);
	}
	sp->UpdatePos();

	Layout* layout = Layout::create();
	layout->setSize(sp->getContentSize());
	layout->addNode(sp);
	return layout;
}

Label* CompLabel::GetLabWithSize( const char* str,int fontType,int size )
{
	if(fontType < 0 || fontType >= EFontType_Max)
		return NULL;
	Label* lab = Label::create();
	lab->setFontSize(size);
	lab->setText(str);
	//lab->setFontName(GetFontName(fontType));
	lab->setAnchorPoint(ccp(0.5,0.5));
	if (fontType == EFontType_Menu)
	{
		lab->setColor(ccc3(93, 50, 0));
	}
	else
	{
		lab->setColor(s_CommonColor);
	}
	return lab;
}

Label* CompLabel::GetLab( const char* str,int fontType )
{
	return GetLabWithSize(str,fontType,EFontSize_Mid );
}

Label* CompLabel::GetLabWithAnchorRect( const char* str,int fontType,int size,const CCSize& recsize,CCTextAlignment ali )
{
	if(fontType < 0 || fontType >= EFontType_Max)
		return NULL;
	Label* lab = Label::create();
	lab->setFontSize(size);
	lab->setText(str);
	//lab->setFontName(GetFontName(EFontType_Default));
	lab->setTextAreaSize(recsize);
	lab->setTextHorizontalAlignment(ali);
	lab->setAnchorPoint(ccp(0.5,0.5));
	lab->setColor(s_CommonColor);
	return lab;
}

int ppp = 0;
CCNode* CompLabel::createStroke(CCLabelTTF *lab, float size, ccColor3B incolor,ccColor3B outcolor)
{
	CCSize textureSize = lab->getContentSize();
	textureSize.width += 1.5f * size;
	textureSize.height += 1.5f * size;
	//call to clear error
	glGetError();
	CCRenderTexture *rt = CCRenderTexture::create(textureSize.width, textureSize.height);

	lab->setColor(outcolor);

	ccBlendFunc originalBlend = lab->getBlendFunc();
	ccBlendFunc func;
	if(ppp % 3 == 0)
	{
		func.src = GL_SRC_ALPHA;
		func.dst =  GL_ONE_MINUS_SRC_ALPHA;
	}
	else if(ppp % 3 == 1)
	{
		func.src = GL_SRC_ALPHA_SATURATE;
		func.dst = GL_ONE_MINUS_SRC_ALPHA;
	}
	else
	{
		func.src = GL_SRC_ALPHA_SATURATE;
		func.dst = GL_ONE;
	}
	ppp++;

	func.src = GL_SRC_ALPHA_SATURATE;
	func.dst = GL_ONE_MINUS_SRC_ALPHA;
	lab->setBlendFunc(func);

	lab->setAnchorPoint(ccp(0.5, 0.5));

	rt->begin();
	for(int i = 0; i < 360; i += 30)
	{
		float r = CC_DEGREES_TO_RADIANS(i);
		lab->setPosition(ccp(
			textureSize.width * 0.5f + sin(r) * size * 0.9,
			textureSize.height * 0.5f + cos(r) * size * 0.9));
		lab->visit();
	}
	lab->setColor(incolor);
	lab->setBlendFunc(originalBlend);
	lab->setPosition(ccp(textureSize.width * 0.5f, textureSize.height * 0.5f));
	lab->visit();
	rt->end();

	CCTexture2D *texture = rt->getSprite()->getTexture();
	texture->setAliasTexParameters();
	CCSprite* spr = CCSprite::createWithTexture(rt->getSprite()->getTexture());
	lab->setContentSize(spr->getContentSize());
	spr->setAnchorPoint(ccp(0, 0));
	spr->setPosition(ccp(0, 0));
	((CCSprite *)spr)->setFlipY(true);
	return spr;
}

string CompLabel::SetColorInLab( string& str,string colorStr )
{
	if(!StringUtil::StartsWith(str,"<lb"))
	{
		return str;
	}
	char buf[128] = {0};
	sprintf(buf," cl=\"%s\"",colorStr.c_str());
	str.insert(3,buf);
	return str;
}

Label* CompLabel::GetMenuTTFLab( const char* str )
{
	return GetLabWithSize(str, EFontType_Menu, EFontSize_Mid);
}

bool CompLabel::IsShowAll( Widget* widget )
{
	StringParser* sp = (StringParser*)widget->getNodes()->objectAtIndex(0);
	return sp->getIsShowAll();
}

NS_FM_END