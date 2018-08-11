
#include "Launcher.h"
#include "FmMainScene.h"

#include "Util/FmStringMgr.h"
#include "UI/FmUIMgr.h"

#include "Script/MyLuaEngine.h"
#include "Script/FmScriptSys.h"
#include "Sound/AudioEngine.h"

#include "Json/Cjson.h"
#include "Common.h"
#include "GameData/DataManager.h"
#include "Effect/FmEffect.h"
#include "Player/FmPlayer.h"
#include "GameEntity/FmHero.h"
#include "FmFightMgr.h"
#include "FightSoul/FmFightSoulBag.h"
#include "Player/FmGodAnimal.h"
#include "Face/FaceMgr.h"
#include "Equip/FmEquip.h"
#include "Asset/FileMgr.h"
#include "Asset/HttpConnect.h"
#include "../CCFileUtils.h"
#include "UI/CompLabel.h"
#include "FmXmlWrapper.h"
#include "NetWork/ClientConect.h"
#include "NetWork/ClientSinkCallbackMgr.h"
#include "VersionConfig.h"

NS_FM_BEGIN

static Launcher* s_LauncherInst = NULL;

static FmAudioEngineDelegate* s_AudioEngineDelegate = NULL;
Launcher* Launcher::GetInstance()
{
	if (s_LauncherInst == NULL)
	{
		s_LauncherInst = Launcher::create();
		//s_LauncherInst->retain();

		MainScene::GetInstance().addChild(s_LauncherInst, -100);
	}
	return s_LauncherInst;
}

Launcher::Launcher()
{
	//s_StrMgrDelegate = new FmStringMgrDelegate();
	//s_AudioEngineDelegate = new FmAudioEngineDelegate;

	m_UpdateVerUrl = "";
	m_uiScale = 1;
	m_DestProgressValue = 0;
	m_TotalProgressValue = 0;
	m_CurProgressValue = 0;

	m_LocalMgr = NULL;
	m_ServerMgr = NULL;
	m_AppResMgr = NULL;

	ClientSinkCallbackMgr::GetInstance().RegisterRS(ServerEngine::RS_ROUTER_VERSION, RouterVersion);
}

Launcher::~Launcher()
{
	s_LauncherInst = NULL;
}

bool Launcher::init()
{
	CCLayer::init();

	Connect();
	scheduleOnce(schedule_selector(Launcher::WaitRS), 30);
	schedule(schedule_selector(Launcher::ReConnect), 15);

	//AsynDownLoad::GetInstance().SetResUrl(StringMgr::GetInstance().GetString("CharUpdateUrl"));

	m_LauncherStep = E_LauncherSteo_GetVersion;
	m_HasGetVersion = false;
	m_Finished = false;
	m_RequestFailCounter = 0;

	return true;
}

void Launcher::Connect()
{
	XmlWrapper xmlWrapper;
	FmXmlDocument& doc = xmlWrapper.GetDoc();


	if( !xmlWrapper.LoadFile( "Config/Network.xml" ) )
	{
		Assert( false );
		return;
	}
	FmXmlElement* rootNode = doc.FirstChildElement( "Servers" );
	if( rootNode == NULL )
	{
		Assert( false );
		return;
	}
	FmXmlElement* node = rootNode->FirstChildElement( "Server" );
	string ip =  node->Attribute("IP");
	int port = node->IntAttribute("Port");
    printf("%s,%d",ip.c_str(),port);

	ClientSink::GetInstance().Connect(ip, port);
}

void Launcher::ReConnect(float time)
{
	if (ClientSink::GetInstance().IsConnect())
	{
		unschedule(schedule_selector(Launcher::ReConnect));
	}
	else
	{
		Connect();
	}
}

void Launcher::WaitRS(float time)
{
	if (E_LauncherSteo_GetVersion == m_LauncherStep)
	{
		ShowMessageBox(
			StringMgr::GetInstance().GetString("HttpFailTitle"),
			StringMgr::GetInstance().GetString("HttpFailRemind"),
			StringMgr::GetInstance().GetString("HttpFailContinue"),
			StringMgr::GetInstance().GetString("HttpFailQuit"),
			101
			);
	}
}

Launcher* Launcher::create()
{
	Launcher * pLayer = new Launcher();
	if( pLayer && pLayer->init())
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return NULL;
}


void Launcher::OnVersionResponse( const char * resp )
{
	bool checkRes = true;
	if(strcmp(StringMgr::GetInstance().GetString("UpdateVersion"),"N")==0)
	{
		checkRes = false;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	checkRes = false;
#endif
	if(checkRes)
	{
	//	CCLog(">>>>>>>>> OnVersionResponse Check Res  TRUE  ");
		m_LauncherStep = E_LauncherStep_Init;
	}
	else
	{
	//	CCLog(">>>>>>>>> OnVersionResponse Check Res  FALSE  ");
		m_LauncherStep = E_LauncherStep_ResInit;
	}

	/*Json::Reader reader;
	Json::Value cInfo;
	Json::Value verConfig;
	if (!reader.parse(Common::GetChannelInfor(), cInfo))
	{
		return;
	}
	if (!reader.parse(resp, verConfig))
	{
		return;
	}
	string channel = cInfo["Channel"].asString();
	string version = cInfo["Version"].asString();
	string platform = cInfo["Platform"].asString();

	char propVal[256] ={0};
	sprintf(propVal,"MinVer_%s_%s",channel.c_str(),platform.c_str());
	string minVer =  verConfig[propVal].asString();
	sprintf(propVal,"MaxVer_%s_%s",channel.c_str(),platform.c_str());
	string maxVer =  verConfig[propVal].asString();

	sprintf(propVal,"Url_%s_%s",channel.c_str(),platform.c_str());
	m_UpdateVerUrl  =  verConfig[propVal].asString();

	int minVerNum = atoi(minVer.c_str());
	int maxVerNum = atoi(maxVer.c_str());
	int curVerNum = atoi(version.c_str());

	if(curVerNum < minVerNum)
	{
		vector<Var> args;
		args.push_back( Var(maxVer) );
		string remind = StringMgr::GetInstance().FormatString("VersionUpdateRemind",args);
		ShowMessageBox(
			StringMgr::GetInstance().GetString("VersionUpdateTitle"),
			remind.c_str(),
			StringMgr::GetInstance().GetString("VersionUpdateYes"),
			StringMgr::GetInstance().GetString("LauncherQuit")
			);
		m_LauncherStep = E_LauncherStep_MsgBox;
	}*/
}
void Launcher::MainLoop( uint delta )
{
	if(m_Finished)
		return;
	if (UIMgr::GetInstance().GetBaseWidgetByName("ShowBox"))
	{
		return;
	}

	if (UIMgr::GetInstance().GetBaseWidgetByName("Logo") == NULL)
	{
		UIWidget* widget = UIMgr::GetInstance().CreateBaseWidgetByFileName("UIExport/DemoLogin/Logo.json");
		UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),0);

		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("UIEffect/Loding/Loding.ExportJson");
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("UIEffect/Loding/Loding.ExportJson");
		CCArmature* pAnimature = CCArmature::create("Loding");
		pAnimature->getAnimation()->playWithIndex(1);
		widget->getChildByName("BG")->addNode(pAnimature);
		pAnimature->setPosition(ccp(0, 0));
	}

	switch(m_LauncherStep)
	{
	case E_LauncherSteo_GetVersion:
		{
			bool checkRes = true;
			if(strcmp(StringMgr::GetInstance().GetString("UpdateVersion"),"N")==0)
			{
				checkRes = false;
			}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			checkRes = false;
#endif
			if(!checkRes)
			{
				m_LauncherStep = E_LauncherStep_ResInit;
				return;
			}
			if(m_HasGetVersion == false && ClientSink::GetInstance().IsConnect())
			{
				UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResUpdate").c_str(),0);
				//UpdateTips(StringMgr::GetInstance().GetString("LauncherWaiting"));	;
				//HttpConnect::GetInstance().RequestUrlFile(StringMgr::GetInstance().GetString("VarUpdateUrl"),"verCheck");

				ServerEngine::CSMessage versionPkg;
				versionPkg.set_icmd(ServerEngine::RS_ROUTER_VERSION);
				ClientSink::GetInstance().SendPkg(versionPkg);
				m_HasGetVersion = true;
			}
		}
		break;
	case E_LauncherSteo_MediaVerCheck:
		{
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResUpdate").c_str(),5);
			//UpdateTips(StringMgr::GetInstance().GetString("LauncherWaiting"));	

			m_LocalMgr = new FileMgr(); 
			m_LocalMgr->autorelease();
			m_LocalMgr->InitLocalMgr(CCFileUtils::sharedFileUtils()->getWritablePath().c_str(),s_ResConfigFileName.c_str());
			m_LocalMgr->SetResUrl(m_url.c_str());
			m_LocalMgr->retain();
			m_ServerMgr = new FileMgr();
			m_ServerMgr->autorelease();
			m_ServerMgr->InitServerMgr(m_url.c_str(),s_VersionNumFileName.c_str(),s_ResConfigFileName.c_str());
			m_ServerMgr->retain();

			m_AppResMgr = new FileMgr();
			m_AppResMgr->autorelease();
#ifdef USE_ZIPRES
			m_AppResMgr->InitLocalMgr(CCFileUtils::sharedFileUtils()->getWritablePath().c_str(),"boundle.config");
#else
			m_AppResMgr->InitLocalMgr("","boundle.config");
#endif
			m_AppResMgr->SetResUrl(m_url.c_str());
			m_AppResMgr->retain();

			m_LauncherStep = E_LauncherStep_Init;
		}
		break;
	case E_LauncherStep_Init:
		{
			//FaceMgr::GetInstance().LoadFile("ImgUi/emoticon");
			
			m_RequestFailCounter = 0;
			m_LocalMgr->LoadLocalConfig();
			m_ServerMgr->RequestVersionNum();
			m_LauncherStep = E_LauncherStep_VerCheck;
		}
		break;
	case E_LauncherStep_VerCheck:
		{
			if(m_ServerMgr->HasGetVersionNum())
			{
				//if(m_ServerMgr->CodeVersionIsLowerThan(s_CodeVerion)) //??????????????,????
				//{
				//	CCLog(">>>>>>>>> Code Version Client Is Higher %d ",s_CodeVerion.c_str());
				//	m_LauncherStep = E_LauncherStep_ResLoadFinished;
				//}
				//else if(!m_ServerMgr->CodeVersionMatched(s_CodeVerion)) //???????
				//{
				//	//ShowMessageBox(
				//	//	StringMgr::GetInstance().GetString("VersionUpdateTitle"),
				//	//	StringMgr::GetInstance().GetString("VersionUpdateRemind"),
				//	//	StringMgr::GetInstance().GetString("VersionUpdateYes"),
				//	//	StringMgr::GetInstance().GetString("LauncherQuit")
				//	//	);
				//	//m_LauncherStep = E_LauncherStep_MsgBox;
				//	m_LauncherStep = E_LauncherStep_ResLoadFinished;
				//	CCLog(">>>>>>>>> Code Version is Not Match %d  ",s_CodeVerion.c_str());
				//}
				//else
				{
					bool zipOk = false;
#ifdef USE_ZIPRES
					//检查是否需要解压缩包
					if(!m_LocalMgr->CodeVersionMatched(s_CodeVerion))
					{
						CCLog(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> UnCompress ");
						//解压应用程序中的压缩包
						zipOk = FileMgr::UnCompress(CCFileUtils::sharedFileUtils()->fullPathForFilename("assets.zip").c_str(),CCFileUtils::sharedFileUtils()->getWritablePath().c_str());
						if(zipOk == false)
							CCLog(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> UnCompress failed ");
					}
#endif
					if(!FileMgr::VersionMatched(m_LocalMgr,m_ServerMgr)) //资源需要更新
					{
						//CCLog(">>>>>>>>> Update Resource ");
						//UpdateTips(StringMgr::GetInstance().GetString("LauncherResLoad"));
						m_AppResMgr->LoadLocalConfig();
						m_ServerMgr->RequestResConfig();
						m_LauncherStep = E_LauncherStep_LoadConfig;
#ifdef USE_ZIPRES
						if(zipOk)
						{
							m_LocalMgr->MergeWithFileMgr(m_AppResMgr);
						}
						m_LocalMgr->SetPrepare(true);
#else
						m_LocalMgr->CheckPackRes();
						m_LocalMgr->MergeWithFileMgr(m_AppResMgr);
#endif
					}
					else
					{
						//	CCLog(">>>>>>>>>  Resource is Matched");
						m_LauncherStep = E_LauncherStep_ResLoadFinished;
					}
				}
			}
		}
		break;
	case E_LauncherStep_LoadConfig:
		{
			if(m_LocalMgr->IsPreapred() && m_ServerMgr->IsPreapred())
			{
				m_LocalMgr->SynWithFileMgr(m_ServerMgr);

				int netInfor = CCDevice::GetNetInfor(); // -1 无连接 0 移动网络  1 WIFI
				CCLog(">>>>>> NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNet status: %d",netInfor);
				int resKb = m_LocalMgr->GetTotalDownSize()/1000;
				//判断当前网络连接
				if(netInfor != 1 && resKb>5000)
				{
					vector<Var> args;
					args.push_back( Var(resKb) );
					string remind = StringMgr::GetInstance().FormatString("ResUpdateRemind",args);
					ShowMessageBox(
						StringMgr::GetInstance().GetString("ResUpdateTitle"),
						remind.c_str(),
						StringMgr::GetInstance().GetString("ResUpdateYes"),
						StringMgr::GetInstance().GetString("LauncherQuit"),
						100
						);
					m_LauncherStep = E_LauncherStep_MsgBox;
				}
				else
				{
					StartLoadRes();
				}
			}
		}
		break;
	case E_LauncherStep_LoadUrlFile:
		{
			if(m_LocalMgr->IsRequestFinished())
			{
				m_LauncherStep = E_LauncherStep_LocalVerify;
				unschedule( schedule_selector(Launcher::UpdateLoadDummyProgressTips));
				//unschedule( schedule_selector(Launcher::UpdateLoadDummyProgressTips));
			}

		}
		break;
	case E_LauncherStep_LocalVerify:
		{
			if(m_LocalMgr->OnLoadFinished())
			{
				m_LauncherStep = E_LauncherStep_ResLoadFinished;
			}
		}
		break;
	case E_LauncherStep_ResLoadFinished:
		{
			m_LauncherStep = E_LauncherStep_ResInit;
		}
		break;
	
	case E_LauncherStep_ResInit:
		{
			if(m_LocalMgr)
				m_LocalMgr->release();
			if(m_ServerMgr)
				m_ServerMgr->release();
			if(m_AppResMgr)
				m_AppResMgr->release();
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),20);
			ResInitLoad1();
			m_LauncherStep = E_LauncherStep_ResInit2;
		}
		break;
	case E_LauncherStep_ResInit2:
		{
		//	Sleep(1000);
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),50);
			ResInitLoad2();
			m_LauncherStep = E_LauncherStep_ResInitEnd;
		}
		break;
	case E_LauncherStep_ResInitEnd:
		{
		//	Sleep(1000);
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),70);
			ResInitLoad3();
			m_LauncherStep = E_LauncherStep_SwitchGame;
		}
		break;
	case E_LauncherStep_SwitchGame:
		{
		//	Sleep(1000);
			UIMgr::GetInstance().CloseAllBaseWidget(true);

			ScriptSys::GetInstance().Execute( "Script_Main" );

			MainScene::GetInstance().SwitchState(EState_MainMenu);

			MainScene::GetInstance().removeChild(s_LauncherInst);
			s_LauncherInst = NULL;
		}
		break;
	}

	if(m_LauncherStep >=E_LauncherSteo_MediaVerCheck && m_LauncherStep < E_LauncherStep_ResLoadFinished )
	{
		if(m_RequestFailCounter > 3)
		{
			ShowMessageBox(
				StringMgr::GetInstance().GetString("HttpFailTitle"),
				StringMgr::GetInstance().GetString("HttpFailRemind"),
				StringMgr::GetInstance().GetString("HttpFailContinue"),
				StringMgr::GetInstance().GetString("HttpFailQuit"),
				101
				);
			m_RequestFailCounter = 0;
		}
	}
}

void Launcher::SwitchStep( E_LauncherStep step )
{
	m_LauncherStep = step;
	//if(m_LauncherStep == E_LauncherStep_LoadUrlFile)
	//{
	//	removeChildByTag(E_LauncherTag_LoadTip);
	//}
}


void Launcher::UpdateLoadDummyProgressTips( float delta )
{
	if(m_CurProgressValue < m_DestProgressValue)
	{
		m_CurProgressValue += 500;
		UpdateLoadProgress( m_TotalProgressValue,m_CurProgressValue);
	}
}

void Launcher::InitLoadProgressVal( int total,int destVal )
{
	m_DestProgressValue = destVal;
	m_TotalProgressValue = total;
	schedule( schedule_selector(Launcher::UpdateLoadDummyProgressTips), 1.0f);
}




void Launcher::UpdateTips( const char* remind )
{
	if(0)
	{
		Label* lab = dynamic_cast<Label*>(UIMgr::GetInstance().GetBaseWidgetByName("Logo")->getChildByTag(E_LauncherTag_Tips));
		if(lab)
		{
			lab->setText(remind);
		}
		else
		{
			lab = CompLabel::GetDefaultLab(remind);
			lab->setColor(ccc3(250,255,255));
			lab->setAnchorPoint(ccp(0.5,0.5));
			lab->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2,CCDirector::sharedDirector()->getWinSize().height *1/4));
			UIMgr::GetInstance().GetBaseWidgetByName("Logo")->addChild(lab,1,E_LauncherTag_Tips);
		}
	}
}


void Launcher::UpdateLoadTips( const char* filename )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	char buf[1024] = {0};
	sprintf(buf,"load file: %s",filename);
	Label* lab = dynamic_cast<Label*>(UIMgr::GetInstance().GetBaseWidgetByName("Logo")->getChildByTag(E_LauncherTag_LoadTip));
	if(lab)
	{
		lab->setText(buf);
	}
	else
	{
		lab = CompLabel::GetDefaultLab(buf);
		lab->setColor(ccc3(250,10,10));
			lab->setPosition(ccp(0,100));
		UIMgr::GetInstance().GetBaseWidgetByName("Logo")->addChild(lab,1,E_LauncherTag_LoadTip);
	}
#endif
}

void Launcher::UpdateLoadProgress( int total,int cur)
{
	m_CurProgressValue = cur;
	int val = 0;
	if(total > 0)
	{
		val = cur/(total*100);
	}
	//CCLog("Loading....  %d  %d ",total,cur);
	char buf[1024] = {0};
	sprintf(buf,"%s(%.3f M / %.3f M)",StringMgr::GetInstance().FormatString("LauncherResUpdate").c_str(),cur/1000000.0f,total/1000000.0f);
	UpdateLoadBar(buf,val);

}


void Launcher::UpdateLoadBar( const char* tips,int val )
{
    cocos2d::ui::UIWidget* widget = UIMgr::GetInstance().GetBaseWidgetByName("Logo");
	if (widget)
	{
		UILoadingBar* bar = dynamic_cast<UILoadingBar*>(widget->getChildByTag(1));
		if (bar)
		{
			bar->setPercent(val);
		}
		cocos2d::ui::UILabel* label = dynamic_cast<cocos2d::ui::UILabel*>(widget->getChildByTag(2));
		if (label)
		{
			label->setText(tips);
		}
	}

	/*CCNode* bar = getChildByTag(E_LauncherTag_LoadBar);
	if(bar == NULL)
		return;
	CCControlSlider* slider = dynamic_cast<CCControlSlider*>(bar);
	if(slider)
	{
		slider->setValue(val);
	}

	bar->removeChildByTag(11);
	if(tips != NULL)
	{
		//CCLog("Loading....  %f  ",val);
		CCSize cs = bar->getContentSize();
		vector<Var> args;
		args.push_back( Var( tips ) );
		std::string txt = StringMgr::GetInstance().FormatString( "ProgressPatten", args );
		CCNode* lab = CompLabel::GetDefaultCompLabel(txt.c_str(),0);
		lab->setPosition(ccp((cs.width-lab->getContentSize().width)/2,(cs.height+lab->getContentSize().height)/2));
		bar->addChild(lab,1,11);
	}*/
}


bool Launcher::ResInitLoad1()
{
	//StringMgr::GetInstance().LoadFilterString("filter.xml");
	PropertyDescMgr::GetInstance().Init( "Data/Entity/EntityList.xml", false );

	EffectMgr::GetInstance().Init();

	CDataManager::GetInstance().Init();

	FightMgr::GetInstance();

	return true;
}

bool Launcher::ResInitLoad2()
{
	// 初始化随机数
	Random::SetSeed_Now();

	FightSoul::InitInterface();
	Equip::InitInterface();
	//SceneEntity::InitInterface();
	Player::InitInterface();
	Hero::InitInterface();
	BagItem::InitInterface();
	GodAnimal::InitInterface();

	return true;
}

bool Launcher::ResInitLoad3()
{
	// 脚本系统
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	ScriptSys::GetInstance().Init();
	ScriptSys::GetInstance().LoadFile( "main.lua" );
	return true;
}
void Launcher::ShowMessageBox( const char * titleStr,const char* msgStr,const char* leftStr,const char* rightStr, int tag )
{
	Widget* widget = UIMgr::GetInstance().CreateBaseWidgetByFileName("ShowBox.json");

	if (widget)
	{
		Widget* info = CompLabel::GetDefaultCompLabel(msgStr, 300);
		widget->getChildByTag(4)->addChild(info);

		Button* left = dynamic_cast<Button*>(widget->getChildByTag(2));
		Button* right = dynamic_cast<Button*>(widget->getChildByTag(3));

		left->setTitleText(leftStr);
		right->setTitleText(rightStr);

		left->addTouchEventListener(this, toucheventselector(Launcher::HandleMessageBox));
		left->setTag(tag);

		right->addTouchEventListener(this, toucheventselector(Launcher::HandleMessageBox));
	}

	/*this->removeChildByTag(E_LauncherTag_MsgBox);

	CCSize frameSize = CCSizeMake(960,640);//CCEGLView::sharedOpenGLView()->getFrameSize();
	CCNode* box = CCNode::create();
	box->setScale(m_uiScale);
	float msgWidth = frameSize.width*2/3 - 40;
	CCScale9Sprite* msgBG = CCScale9Sprite::create("ImgUi/UI/messagebox.png");
	CCNode* title = CompLabel::GetCompLabel(titleStr,0);
	CCNode* msg = CompLabel::GetCompLabel(msgStr,msgWidth,kCCTextAlignmentCenter);
	CCNode* left = CompLabel::GetCompLabel(leftStr,0);
	CCNode* right = CompLabel::GetCompLabel(rightStr,0);
	float msgh = msg->getContentSize().height;
	CCSize size = CCSizeMake(frameSize.width*2/3, 180 + msgh);
	msgBG->setContentSize(CCSizeMake(size.width,size.height));
	
	msg->setPosition(ccp(-msgWidth/2,msgh/2));

	box->addChild(msgBG,0);

	box->addChild(title,2);
	box->addChild(msg,3);
	box->addChild(left);
	box->addChild(right);
	
	float cmdY = -((size.height/2) - left->getContentSize().height - 15);
	left->setPosition(20-size.width/2,cmdY);
	right->setPosition(size.width/2 - right->getContentSize().width - 20,cmdY);

	title->setPosition(ccp(-title->getContentSize().width/2,size.height/2 - 25));

	CCSize winSize = CCEGLView::sharedOpenGLView()->getFrameSize();
	box->setPosition(ccp(winSize.width/2,winSize.height/2));
	this->addChild(box,100,E_LauncherTag_MsgBox);*/
}

void Launcher::HandleMessageBox( CCObject *pSender, TouchEventType type )
{
	Widget* widget = dynamic_cast<Widget*>(pSender);
	if (type == TOUCH_EVENT_ENDED)
	{
		switch(widget->getTag())
		{
		case 102: //版本更新Yes
			{
				Json::Reader reader;
				Json::Value cInfo;
				if (!reader.parse(Common::GetChannelInfor(), cInfo))
				{
					CCLOG("((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((");
					Launcher::GetInstance()->m_LauncherStep = E_LauncherStep_ResInit;
					return;
				}

				string channel = cInfo["Channel"].asString();
				string url = m_UpdateVerUrl  + "/BigHero" + channel + ".apk";

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
				CCDevice::OpenWeb(url.c_str());
				CCDirector::sharedDirector()->end();
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				CCDevice::OpenWeb(url.c_str());
				CCDirector::sharedDirector()->end();
#endif
			}	
			break;
		case 100: //资源更新YES
			StartLoadRes();
			break;
		case 101:
			m_LauncherStep = E_LauncherStep_ResLoadFinished;
			break;
		case 3://退出游戏
			CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			exit(0);
#endif
			break;
		}

		UIMgr::GetInstance().CloseCurBaseWidget();
	}
}

void Launcher::StartLoadRes()
{
	if(m_LocalMgr->StartRequest())
		m_LauncherStep = E_LauncherStep_LoadUrlFile;
	else
		m_LauncherStep = E_LauncherStep_LocalVerify;
}

void Launcher::RouterVersion( int iCmd, ServerEngine::SCMessage& pkg )
{
	ServerEngine::RS_ROUTER_VERSION_SC tmp;
	if(tmp.ParseFromString(pkg.strmsgbody() ) )
	{
		string url = tmp.strurldir();
		Launcher::GetInstance()->m_version = tmp.strversion();
		Launcher::GetInstance()->m_url = url + "/" +Launcher::GetInstance()->m_version + "/Resources";
		

		string minVersion = tmp.strminversion();
		vector<string> minV = StringUtil::Split(minVersion, ".");
		vector<string> codeV = StringUtil::Split(s_CodeVerion, ".");

		for (size_t i=0; i<minV.size(); i++)
		{
			if (TypeConvert::ToInt(minV[i].c_str()) > TypeConvert::ToInt(codeV[i].c_str()))
			{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				Launcher::GetInstance()->m_UpdateVerUrl = url + "/" +Launcher::GetInstance()->m_version;

				Launcher::GetInstance()->ShowMessageBox(
					StringMgr::GetInstance().GetString("VersionUpdateTitle"),
					StringMgr::GetInstance().GetString("VersionUpdateRemind"),
					StringMgr::GetInstance().GetString("VersionUpdateYes"),
					StringMgr::GetInstance().GetString("LauncherQuit"), 
					102
					);
				Launcher::GetInstance()->m_LauncherStep = E_LauncherStep_MsgBox;
#else		
				Launcher::GetInstance()->m_LauncherStep = E_LauncherStep_ResInit;
#endif
				return;
			}
			else if (TypeConvert::ToInt(minV[i].c_str()) < TypeConvert::ToInt(codeV[i].c_str()))
			{
				break;
			}
		}

		Launcher::GetInstance()->m_LauncherStep = E_LauncherSteo_MediaVerCheck;
	}
}

void FmAudioEngineDelegate::playEffect( const char* pszFilePath, bool bLoop )
{
	AudioEngine::GetInstance().playEffect(pszFilePath, bLoop);  
}

void FmAudioEngineDelegate::playBackgroundMusic( const char* pszFilePath, bool bLoop )
{
	AudioEngine::GetInstance().playBackgroundMusic(pszFilePath, bLoop);
}


NS_FM_END
