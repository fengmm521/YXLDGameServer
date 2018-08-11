-- xh add, use to debug ios login under win32
-- other case should comment this define
-- XH_DEBUG_IOS_LOGIN_UNDER_WIN32 = true;
-- xh end

-- avoid memory leak
collectgarbage("setpause", 100)
collectgarbage("setstepmul", 5000)

package.path = package.path .. ';./Script/protobuf/?.lua'
package.cpath = package.cpath .. ';./Script/protobuf/?.so' .. ';./Script/protobuf/?.dll'

require 'RouterServer_pb'
require 'GameServer_pb'

LoadFile( "Util/Util.lua" );
LoadFile( "Util/Freeman.lua" );
LoadFile( "Test.lua" );
LoadFile( "CgConfig.lua" )
LoadFile( "GuideConfig.lua" )
--LoadFile( "ClientPkg/ClientPkg.lua" )
LoadFile( "Login/LoginLoad.lua" )
LoadFile( "UI/UILoad.lua" )
LoadFile( "Enum/EnumAutoGenerate.lua" )
LoadFile( "Enum/FmPropertyIndex.lua" )
LoadFile( "BagSys/BagSysLoad.lua" )
LoadFile( "Scene/SceneLoad.lua" )
LoadFile( "Hero/HeroLoad.lua" )
LoadFile( "GameEntity/GameEntityLoad.lua" )
LoadFile("CashCow/CashCowload.lua")
LoadFile( "Formation/FormationLoad.lua" )
LoadFile( "Dungeon/DungeonLoad.lua" )
LoadFile( "Skill/SkillTip.lua" )
LoadFile( "FightSoul/FightSoulLoad.lua" )
LoadFile( "Global.lua" )
LoadFile( "GodAnimal/LoadGodAnimalSys.lua" );
LoadFile( "Territory/TerritoryLoad.lua" )
LoadFile( "Legion/HeroLegionLoad.lua" )
LoadFile( "Chat/Chat.lua" )
LoadFile( "Babel/Babel.lua" )
LoadFile( "Shop/Shop.lua" )
LoadFile( "Arena/Arena.lua" )
LoadFile( "Gifts/GiftsLoad.lua" )
LoadFile( "CompBattle/CompBattle.lua" )
LoadFile( "WorldBoss/WorldBossLoad.lua" )
LoadFile( "Friend/Friend.lua" )
LoadFile( "OtherFunction/OtherFunctionLoad.lua" )
LoadFile( "Activity/ActivityLoad.lua" )
LoadFile( "Guide/Guide.lua" )
LoadFile( "Fight/Fight.lua" )
LoadFile("Task/TaskLoad.lua")
LoadFile("Dialog.lua")
LoadFile("VIPPay/VIPPayLoad.lua")
--LoadFile("Equip/Equip.lua")
LoadFile("NineSky/NineSky.lua")
LoadFile("MailBox/MailBoxload.lua")
LoadFile("IOSLogin/IOSLogin.lua");

--LoadFile("GodAnimal/LoadGodAnimalSys.lua")
-- 初始化
function Script_Main()
	Log( "Script_Main invoked.==============" );
	--[[-- 加载登录界面
	Log( "Script_Main:LoginUI_Load" )
	LoginUI_Load()
	-- 主界面
	MainUILayer_Init()
	
	UI_LoadInitUI()
	
	--强化界面
	-- StrengthenUI_Init()
	
	--法宝界面
	--MagicWeaponUI_Init()
	--英雄法术界面
	--HeroSkillUI_Init()
	--丹药界面
	--PiffItemUI_Init()
	--培养界面
	--PracticeUI_Init()
	--培养返还界面
	--PracticeReturnUI_Init()
	]]
	--音效加载
	Log( "1.==============" );
	InitGameEffectMusicFile()
	Log( "2.==============" );
	--背景音乐加载
    Log( "3.==============" );
	InitGameBgMusicFile()
	if getOSID() == "IOS" or XH_DEBUG_IOS_LOGIN_UNDER_WIN32 ~= nil or XH_DEBUG_IOS_LOGIN_UNDER_WIN32 then
	else
		Login_LoadRecordUserData();
	end
	
    Log( "5.==============" );
-- XH: 暂时关闭该方法,在IOS上会报错:[NSDictionary dictionaryWithObject:StrforKey:]: unrecognized selector sent to class 0x5263d74
    -- PushMentSystem_Push_AZ();
-- END
	-- Login_Create();
    if getOSID() == "IOS" or XH_DEBUG_IOS_LOGIN_UNDER_WIN32 ~= nil or XH_DEBUG_IOS_LOGIN_UNDER_WIN32 then
	    IOSLogin_Create();
    else
        Login_Create();
    end
    Log( "7.==============" );
end

--进入游戏
function Script_EnterGame()
	--[[if( IsRobet() )then
		g_RobetState = RobetState_GamePlay
	end]]
end
-- 测试函数
function Test()
	-- todo: 弹出调试菜单
	--[[Test_ShowMenus();]]
end
