/*
** Lua binding: JZServerExport
** Generated automatically by tolua++-1.0.92 on Sun Jul 15 01:16:12 2018.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_JZServerExport_open (lua_State* tolua_S);

#include "../Src/LuaWrapper/LuaWrapperPch.h"
#include "../Src/LuaWrapper/CommWrapper.h"
#include "../Include/Property/EntityProperty.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_EventArgs (lua_State* tolua_S)
{
 EventArgs* self = (EventArgs*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_vector_double_ (lua_State* tolua_S)
{
 vector<double>* self = (vector<double>*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_vector_string_ (lua_State* tolua_S)
{
 vector<string>* self = (vector<string>*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_vector_int_ (lua_State* tolua_S)
{
 vector<int>* self = (vector<int>*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"EventArgs");
 tolua_usertype(tolua_S,"_PropertySetT<string>");
 tolua_usertype(tolua_S,"vector<string>");
 tolua_usertype(tolua_S,"vector<int>");
 tolua_usertype(tolua_S,"IObject");
 tolua_usertype(tolua_S,"vector<double>");
 tolua_usertype(tolua_S,"IEntity");
 tolua_usertype(tolua_S,"PropertySet");
}

/* function: getWorkDir */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_getWorkDir00
static int tolua_JZServerExport_getWorkDir00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   string tolua_ret = (string)  getWorkDir();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getWorkDir'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: sendCommDlg */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_sendCommDlg00
static int tolua_JZServerExport_sendCommDlg00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,4,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,5,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 dwActor = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  const string strMsg = ((const string)  tolua_tocppstring(tolua_S,2,0));
  const string strLeftName = ((const string)  tolua_tocppstring(tolua_S,3,0));
  const string strLeftCmd = ((const string)  tolua_tocppstring(tolua_S,4,0));
  const string strRightName = ((const string)  tolua_tocppstring(tolua_S,5,0));
  const string strRightCmd = ((const string)  tolua_tocppstring(tolua_S,6,0));
  {
   sendCommDlg(dwActor,strMsg,strLeftName,strLeftCmd,strRightName,strRightCmd);
   tolua_pushcppstring(tolua_S,(const char*)strMsg);
   tolua_pushcppstring(tolua_S,(const char*)strLeftName);
   tolua_pushcppstring(tolua_S,(const char*)strLeftCmd);
   tolua_pushcppstring(tolua_S,(const char*)strRightName);
   tolua_pushcppstring(tolua_S,(const char*)strRightCmd);
  }
 }
 return 5;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendCommDlg'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: doFightSoulCombine */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_doFightSoulCombine00
static int tolua_JZServerExport_doFightSoulCombine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 dwEntityID = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  int iSrcConType = ((int)  tolua_tonumber(tolua_S,2,0));
  int iSrcPos = ((int)  tolua_tonumber(tolua_S,3,0));
  int iDstConType = ((int)  tolua_tonumber(tolua_S,4,0));
  int iDstPos = ((int)  tolua_tonumber(tolua_S,5,0));
  Uint32 dwHeroObjectID = ((Uint32)  tolua_tonumber(tolua_S,6,0));
  {
   doFightSoulCombine(dwEntityID,iSrcConType,iSrcPos,iDstConType,iDstPos,dwHeroObjectID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'doFightSoulCombine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: enableFSAdvance */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_enableFSAdvance00
static int tolua_JZServerExport_enableFSAdvance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 dwEntityID = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   enableFSAdvance(dwEntityID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'enableFSAdvance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LogMsg */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_LogMsg00
static int tolua_JZServerExport_LogMsg00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const string strMsg = ((const string)  tolua_tocppstring(tolua_S,1,0));
  {
   LogMsg(strMsg);
   tolua_pushcppstring(tolua_S,(const char*)strMsg);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LogMsg'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: autoCombineFightSoul */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_autoCombineFightSoul00
static int tolua_JZServerExport_autoCombineFightSoul00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 dwEntityID = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   autoCombineFightSoul(dwEntityID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'autoCombineFightSoul'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: addTowerResetCount */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_addTowerResetCount00
static int tolua_JZServerExport_addTowerResetCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 dwEntityID = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  bool ensure = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   addTowerResetCount(dwEntityID,ensure);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addTowerResetCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: buyArenaCount */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_buyArenaCount00
static int tolua_JZServerExport_buyArenaCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 dwEntityID = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   buyArenaCount(dwEntityID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'buyArenaCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: buyPhystrength */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_buyPhystrength00
static int tolua_JZServerExport_buyPhystrength00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 dwEntityID = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   bool tolua_ret = (bool)  buyPhystrength(dwEntityID);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'buyPhystrength'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: manorSilverResGoldAddSpeed */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_manorSilverResGoldAddSpeed00
static int tolua_JZServerExport_manorSilverResGoldAddSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 dwEntityID = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  int iCost = ((int)  tolua_tonumber(tolua_S,2,0));
  int iResID = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   manorSilverResGoldAddSpeed(dwEntityID,iCost,iResID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'manorSilverResGoldAddSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: manorHeroExpResGoldAddSpeed */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_manorHeroExpResGoldAddSpeed00
static int tolua_JZServerExport_manorHeroExpResGoldAddSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  int iCost = ((int)  tolua_tonumber(tolua_S,2,0));
  int iResID = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   manorHeroExpResGoldAddSpeed(hEntity,iCost,iResID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'manorHeroExpResGoldAddSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: manorGoldAddWuHunDianLevelUpSpeed */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_manorGoldAddWuHunDianLevelUpSpeed00
static int tolua_JZServerExport_manorGoldAddWuHunDianLevelUpSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  int iCost = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   manorGoldAddWuHunDianLevelUpSpeed(hEntity,iCost);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'manorGoldAddWuHunDianLevelUpSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: manorGoldAddTieJiangPuLevelUpSpeed */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_manorGoldAddTieJiangPuLevelUpSpeed00
static int tolua_JZServerExport_manorGoldAddTieJiangPuLevelUpSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  int iCost = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   manorGoldAddTieJiangPuLevelUpSpeed(hEntity,iCost);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'manorGoldAddTieJiangPuLevelUpSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ShopsytemGoldRefreshHonorConvet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_ShopsytemGoldRefreshHonorConvet00
static int tolua_JZServerExport_ShopsytemGoldRefreshHonorConvet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   ShopsytemGoldRefreshHonorConvet(hEntity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ShopsytemGoldRefreshHonorConvet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ShopsystemYuanbaoRefreshNormalShop */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_ShopsystemYuanbaoRefreshNormalShop00
static int tolua_JZServerExport_ShopsystemYuanbaoRefreshNormalShop00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   ShopsystemYuanbaoRefreshNormalShop(hEntity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ShopsystemYuanbaoRefreshNormalShop'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: confirmArenaGlodResetCD */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_confirmArenaGlodResetCD00
static int tolua_JZServerExport_confirmArenaGlodResetCD00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   confirmArenaGlodResetCD(hEntity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'confirmArenaGlodResetCD'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: manorResHarvestReturn */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_manorResHarvestReturn00
static int tolua_JZServerExport_manorResHarvestReturn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   manorResHarvestReturn(hEntity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'manorResHarvestReturn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: manorNoCanLootActorReturn */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_manorNoCanLootActorReturn00
static int tolua_JZServerExport_manorNoCanLootActorReturn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   manorNoCanLootActorReturn(hEntity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'manorNoCanLootActorReturn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: contributeShopRefreshConfirm */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_contributeShopRefreshConfirm00
static int tolua_JZServerExport_contributeShopRefreshConfirm00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   contributeShopRefreshConfirm(hEntity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'contributeShopRefreshConfirm'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: leaveLegionCallBack */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_leaveLegionCallBack00
static int tolua_JZServerExport_leaveLegionCallBack00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Uint32 hEntity = ((Uint32)  tolua_tonumber(tolua_S,1,0));
  {
   leaveLegionCallBack(hEntity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'leaveLegionCallBack'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getInt of class  _PropertySetT<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport__PropertySetT_string__getInt00
static int tolua_JZServerExport__PropertySetT_string__getInt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const _PropertySetT<string>",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const _PropertySetT<string>* self = (const _PropertySetT<string>*)  tolua_tousertype(tolua_S,1,0);
  string key = ((string)  tolua_tocppstring(tolua_S,2,0));
  int nDef = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getInt'", NULL);
#endif
  {
    int tolua_ret = (  int)  self->getInt(key,nDef);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getFloat of class  _PropertySetT<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport__PropertySetT_string__getFloat00
static int tolua_JZServerExport__PropertySetT_string__getFloat00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const _PropertySetT<string>",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const _PropertySetT<string>* self = (const _PropertySetT<string>*)  tolua_tousertype(tolua_S,1,0);
  string key = ((string)  tolua_tocppstring(tolua_S,2,0));
  float fDef = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getFloat'", NULL);
#endif
  {
   float tolua_ret = (float)  self->getFloat(key,fDef);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getFloat'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getString of class  _PropertySetT<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport__PropertySetT_string__getString00
static int tolua_JZServerExport__PropertySetT_string__getString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const _PropertySetT<string>",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const _PropertySetT<string>* self = (const _PropertySetT<string>*)  tolua_tousertype(tolua_S,1,0);
  string key = ((string)  tolua_tocppstring(tolua_S,2,0));
  const char* strDef = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->getString(key,strDef);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getInt64 of class  _PropertySetT<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport__PropertySetT_string__getInt6400
static int tolua_JZServerExport__PropertySetT_string__getInt6400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const _PropertySetT<string>",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const _PropertySetT<string>* self = (const _PropertySetT<string>*)  tolua_tousertype(tolua_S,1,0);
  string key = ((string)  tolua_tocppstring(tolua_S,2,0));
  long nDef64 = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getInt64'", NULL);
#endif
  {
   long tolua_ret = (long)  self->getInt64(key,nDef64);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInt64'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInt of class  PropertySet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_PropertySet_setInt00
static int tolua_JZServerExport_PropertySet_setInt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PropertySet* self = (PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
  unsigned int iValue = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setInt'", NULL);
#endif
  {
   self->setInt(iKey,iValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setInt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setString of class  PropertySet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_PropertySet_setString00
static int tolua_JZServerExport_PropertySet_setString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PropertySet* self = (PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* pszValue = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setString'", NULL);
#endif
  {
   self->setString(iKey,pszValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setFloat of class  PropertySet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_PropertySet_setFloat00
static int tolua_JZServerExport_PropertySet_setFloat00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PropertySet* self = (PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
  float fValue = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setFloat'", NULL);
#endif
  {
   self->setFloat(iKey,fValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setFloat'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInt64 of class  PropertySet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_PropertySet_setInt6400
static int tolua_JZServerExport_PropertySet_setInt6400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PropertySet* self = (PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
   long i64Value = ((  long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setInt64'", NULL);
#endif
  {
   self->setInt64(iKey,i64Value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setInt64'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getInt of class  PropertySet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_PropertySet_getInt00
static int tolua_JZServerExport_PropertySet_getInt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PropertySet* self = (const PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
   int iDef = ((  int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getInt'", NULL);
#endif
  {
    int tolua_ret = (  int)  self->getInt(iKey,iDef);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getFloat of class  PropertySet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_PropertySet_getFloat00
static int tolua_JZServerExport_PropertySet_getFloat00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PropertySet* self = (const PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
  float fDef = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getFloat'", NULL);
#endif
  {
   float tolua_ret = (float)  self->getFloat(iKey,fDef);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getFloat'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getString of class  PropertySet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_PropertySet_getString00
static int tolua_JZServerExport_PropertySet_getString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PropertySet* self = (const PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* strDef = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->getString(iKey,strDef);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getInt64 of class  PropertySet */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_PropertySet_getInt6400
static int tolua_JZServerExport_PropertySet_getInt6400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PropertySet* self = (const PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
   long i64Def = ((  long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getInt64'", NULL);
#endif
  {
    long tolua_ret = (  long)  self->getInt64(iKey,i64Def);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInt64'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  EventArgs */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_EventArgs_new00
static int tolua_JZServerExport_EventArgs_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"EventArgs",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   EventArgs* tolua_ret = (EventArgs*)  Mtolua_new((EventArgs)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"EventArgs");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  EventArgs */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_EventArgs_new00_local
static int tolua_JZServerExport_EventArgs_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"EventArgs",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   EventArgs* tolua_ret = (EventArgs*)  Mtolua_new((EventArgs)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"EventArgs");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  EventArgs */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_EventArgs_delete00
static int tolua_JZServerExport_EventArgs_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"EventArgs",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  EventArgs* self = (EventArgs*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: context of class  EventArgs */
#ifndef TOLUA_DISABLE_tolua_get_EventArgs_context
static int tolua_get_EventArgs_context(lua_State* tolua_S)
{
  EventArgs* self = (EventArgs*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'context'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->context,"_PropertySetT<string>");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: context of class  EventArgs */
#ifndef TOLUA_DISABLE_tolua_set_EventArgs_context
static int tolua_set_EventArgs_context(lua_State* tolua_S)
{
  EventArgs* self = (EventArgs*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'context'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"_PropertySetT<string>",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->context = *((_PropertySetT<string>*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int__clear00
static int tolua_JZServerExport_vector_int__clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<int>* self = (vector<int>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear'", NULL);
#endif
  {
   self->clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: size of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int__size00
static int tolua_JZServerExport_vector_int__size00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const vector<int>* self = (const vector<int>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'size'", NULL);
#endif
  {
   int tolua_ret = (int)  self->size();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'size'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int___geti00
static int tolua_JZServerExport_vector_int___geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const vector<int>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const vector<int>* self = (const vector<int>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   const int tolua_ret = (const int)  self->operator[](index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int___seti00
static int tolua_JZServerExport_vector_int___seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<int>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<int>* self = (vector<int>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
  int tolua_value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](index) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int___geti01
static int tolua_JZServerExport_vector_int___geti01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<int>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  vector<int>* self = (vector<int>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   int tolua_ret = (int)  self->operator[](index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_JZServerExport_vector_int___geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int__push_back00
static int tolua_JZServerExport_vector_int__push_back00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<int>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<int>* self = (vector<int>*)  tolua_tousertype(tolua_S,1,0);
  int val = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'push_back'", NULL);
#endif
  {
   self->push_back(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'push_back'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int__new00
static int tolua_JZServerExport_vector_int__new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   vector<int>* tolua_ret = (vector<int>*)  Mtolua_new((vector<int>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"vector<int>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int__new00_local
static int tolua_JZServerExport_vector_int__new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   vector<int>* tolua_ret = (vector<int>*)  Mtolua_new((vector<int>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"vector<int>");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_int__delete00
static int tolua_JZServerExport_vector_int__delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<int>* self = (vector<int>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string__clear00
static int tolua_JZServerExport_vector_string__clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<string>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<string>* self = (vector<string>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear'", NULL);
#endif
  {
   self->clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: size of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string__size00
static int tolua_JZServerExport_vector_string__size00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const vector<string>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const vector<string>* self = (const vector<string>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'size'", NULL);
#endif
  {
   int tolua_ret = (int)  self->size();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'size'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string___geti00
static int tolua_JZServerExport_vector_string___geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const vector<string>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const vector<string>* self = (const vector<string>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   const string tolua_ret = (const string)  self->operator[](index);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string___seti00
static int tolua_JZServerExport_vector_string___seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<string>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<string>* self = (vector<string>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
  string tolua_value = ((string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](index) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string___geti01
static int tolua_JZServerExport_vector_string___geti01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<string>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  vector<string>* self = (vector<string>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   string tolua_ret = (string)  self->operator[](index);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_JZServerExport_vector_string___geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string__push_back00
static int tolua_JZServerExport_vector_string__push_back00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<string>",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<string>* self = (vector<string>*)  tolua_tousertype(tolua_S,1,0);
  string val = ((string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'push_back'", NULL);
#endif
  {
   self->push_back(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'push_back'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string__new00
static int tolua_JZServerExport_vector_string__new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"vector<string>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   vector<string>* tolua_ret = (vector<string>*)  Mtolua_new((vector<string>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"vector<string>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string__new00_local
static int tolua_JZServerExport_vector_string__new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"vector<string>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   vector<string>* tolua_ret = (vector<string>*)  Mtolua_new((vector<string>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"vector<string>");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_string__delete00
static int tolua_JZServerExport_vector_string__delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<string>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<string>* self = (vector<string>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double__clear00
static int tolua_JZServerExport_vector_double__clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<double>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<double>* self = (vector<double>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear'", NULL);
#endif
  {
   self->clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: size of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double__size00
static int tolua_JZServerExport_vector_double__size00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const vector<double>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const vector<double>* self = (const vector<double>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'size'", NULL);
#endif
  {
   int tolua_ret = (int)  self->size();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'size'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double___geti00
static int tolua_JZServerExport_vector_double___geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const vector<double>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const vector<double>* self = (const vector<double>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   const double tolua_ret = (const double)  self->operator[](index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double___seti00
static int tolua_JZServerExport_vector_double___seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<double>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<double>* self = (vector<double>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
  double tolua_value = ((double)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](index) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double___geti01
static int tolua_JZServerExport_vector_double___geti01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<double>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  vector<double>* self = (vector<double>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   double tolua_ret = (double)  self->operator[](index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_JZServerExport_vector_double___geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double__push_back00
static int tolua_JZServerExport_vector_double__push_back00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<double>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<double>* self = (vector<double>*)  tolua_tousertype(tolua_S,1,0);
  double val = ((double)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'push_back'", NULL);
#endif
  {
   self->push_back(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'push_back'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double__new00
static int tolua_JZServerExport_vector_double__new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"vector<double>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   vector<double>* tolua_ret = (vector<double>*)  Mtolua_new((vector<double>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"vector<double>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double__new00_local
static int tolua_JZServerExport_vector_double__new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"vector<double>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   vector<double>* tolua_ret = (vector<double>*)  Mtolua_new((vector<double>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"vector<double>");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_vector_double__delete00
static int tolua_JZServerExport_vector_double__delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"vector<double>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  vector<double>* self = (vector<double>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getProperty of class  IObject */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IObject_getProperty00
static int tolua_JZServerExport_IObject_getProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  IObject* self = (IObject*)  tolua_tousertype(tolua_S,1,0);
  int key = ((int)  tolua_tonumber(tolua_S,2,0));
   int nDef = ((  int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getProperty'", NULL);
#endif
  {
    int tolua_ret = (  int)  self->getProperty(key,nDef);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getProperty of class  IObject */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IObject_getProperty01
static int tolua_JZServerExport_IObject_getProperty01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  IObject* self = (IObject*)  tolua_tousertype(tolua_S,1,0);
  int key = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* pDef = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getProperty'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->getProperty(key,pDef);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_JZServerExport_IObject_getProperty00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: getProperty of class  IObject */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IObject_getProperty02
static int tolua_JZServerExport_IObject_getProperty02(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  IObject* self = (IObject*)  tolua_tousertype(tolua_S,1,0);
  int key = ((int)  tolua_tonumber(tolua_S,2,0));
  float fDef = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getProperty'", NULL);
#endif
  {
   float tolua_ret = (float)  self->getProperty(key,fDef);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_JZServerExport_IObject_getProperty01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: getInt64Property of class  IObject */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IObject_getInt64Property00
static int tolua_JZServerExport_IObject_getInt64Property00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  IObject* self = (IObject*)  tolua_tousertype(tolua_S,1,0);
  int key = ((int)  tolua_tonumber(tolua_S,2,0));
   long nDef = ((  long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getInt64Property'", NULL);
#endif
  {
    long tolua_ret = (  long)  self->getInt64Property(key,nDef);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInt64Property'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setProperty of class  IObject */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IObject_setProperty00
static int tolua_JZServerExport_IObject_setProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  IObject* self = (IObject*)  tolua_tousertype(tolua_S,1,0);
  int key = ((int)  tolua_tonumber(tolua_S,2,0));
   int nValue = ((  int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setProperty'", NULL);
#endif
  {
   self->setProperty(key,nValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setProperty of class  IObject */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IObject_setProperty01
static int tolua_JZServerExport_IObject_setProperty01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  IObject* self = (IObject*)  tolua_tousertype(tolua_S,1,0);
  int key = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* strValue = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setProperty'", NULL);
#endif
  {
   self->setProperty(key,strValue);
  }
 }
 return 0;
tolua_lerror:
 return tolua_JZServerExport_IObject_setProperty00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: setProperty of class  IObject */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IObject_setProperty02
static int tolua_JZServerExport_IObject_setProperty02(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  IObject* self = (IObject*)  tolua_tousertype(tolua_S,1,0);
  int key = ((int)  tolua_tonumber(tolua_S,2,0));
  float fValue = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setProperty'", NULL);
#endif
  {
   self->setProperty(key,fValue);
  }
 }
 return 0;
tolua_lerror:
 return tolua_JZServerExport_IObject_setProperty01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInt64Property of class  IObject */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IObject_setInt64Property00
static int tolua_JZServerExport_IObject_setInt64Property00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  IObject* self = (IObject*)  tolua_tousertype(tolua_S,1,0);
  int key = ((int)  tolua_tonumber(tolua_S,2,0));
   long nValue = ((  long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setInt64Property'", NULL);
#endif
  {
   self->setInt64Property(key,nValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setInt64Property'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getHandle of class  IEntity */
#ifndef TOLUA_DISABLE_tolua_JZServerExport_IEntity_getHandle00
static int tolua_JZServerExport_IEntity_getHandle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"IEntity",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  IEntity* self = (IEntity*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getHandle'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getHandle();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getHandle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_JZServerExport_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_function(tolua_S,"getWorkDir",tolua_JZServerExport_getWorkDir00);
  tolua_function(tolua_S,"sendCommDlg",tolua_JZServerExport_sendCommDlg00);
  tolua_function(tolua_S,"doFightSoulCombine",tolua_JZServerExport_doFightSoulCombine00);
  tolua_function(tolua_S,"enableFSAdvance",tolua_JZServerExport_enableFSAdvance00);
  tolua_function(tolua_S,"LogMsg",tolua_JZServerExport_LogMsg00);
  tolua_function(tolua_S,"autoCombineFightSoul",tolua_JZServerExport_autoCombineFightSoul00);
  tolua_function(tolua_S,"addTowerResetCount",tolua_JZServerExport_addTowerResetCount00);
  tolua_function(tolua_S,"buyArenaCount",tolua_JZServerExport_buyArenaCount00);
  tolua_function(tolua_S,"buyPhystrength",tolua_JZServerExport_buyPhystrength00);
  tolua_function(tolua_S,"manorSilverResGoldAddSpeed",tolua_JZServerExport_manorSilverResGoldAddSpeed00);
  tolua_function(tolua_S,"manorHeroExpResGoldAddSpeed",tolua_JZServerExport_manorHeroExpResGoldAddSpeed00);
  tolua_function(tolua_S,"manorGoldAddWuHunDianLevelUpSpeed",tolua_JZServerExport_manorGoldAddWuHunDianLevelUpSpeed00);
  tolua_function(tolua_S,"manorGoldAddTieJiangPuLevelUpSpeed",tolua_JZServerExport_manorGoldAddTieJiangPuLevelUpSpeed00);
  tolua_function(tolua_S,"ShopsytemGoldRefreshHonorConvet",tolua_JZServerExport_ShopsytemGoldRefreshHonorConvet00);
  tolua_function(tolua_S,"ShopsystemYuanbaoRefreshNormalShop",tolua_JZServerExport_ShopsystemYuanbaoRefreshNormalShop00);
  tolua_function(tolua_S,"confirmArenaGlodResetCD",tolua_JZServerExport_confirmArenaGlodResetCD00);
  tolua_function(tolua_S,"manorResHarvestReturn",tolua_JZServerExport_manorResHarvestReturn00);
  tolua_function(tolua_S,"manorNoCanLootActorReturn",tolua_JZServerExport_manorNoCanLootActorReturn00);
  tolua_function(tolua_S,"contributeShopRefreshConfirm",tolua_JZServerExport_contributeShopRefreshConfirm00);
  tolua_function(tolua_S,"leaveLegionCallBack",tolua_JZServerExport_leaveLegionCallBack00);
  tolua_cclass(tolua_S,"_PropertySetT_string_","_PropertySetT<string>","",NULL);
  tolua_beginmodule(tolua_S,"_PropertySetT_string_");
   tolua_function(tolua_S,"getInt",tolua_JZServerExport__PropertySetT_string__getInt00);
   tolua_function(tolua_S,"getFloat",tolua_JZServerExport__PropertySetT_string__getFloat00);
   tolua_function(tolua_S,"getString",tolua_JZServerExport__PropertySetT_string__getString00);
   tolua_function(tolua_S,"getInt64",tolua_JZServerExport__PropertySetT_string__getInt6400);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"PropertySet","PropertySet","",NULL);
  tolua_beginmodule(tolua_S,"PropertySet");
   tolua_function(tolua_S,"setInt",tolua_JZServerExport_PropertySet_setInt00);
   tolua_function(tolua_S,"setString",tolua_JZServerExport_PropertySet_setString00);
   tolua_function(tolua_S,"setFloat",tolua_JZServerExport_PropertySet_setFloat00);
   tolua_function(tolua_S,"setInt64",tolua_JZServerExport_PropertySet_setInt6400);
   tolua_function(tolua_S,"getInt",tolua_JZServerExport_PropertySet_getInt00);
   tolua_function(tolua_S,"getFloat",tolua_JZServerExport_PropertySet_getFloat00);
   tolua_function(tolua_S,"getString",tolua_JZServerExport_PropertySet_getString00);
   tolua_function(tolua_S,"getInt64",tolua_JZServerExport_PropertySet_getInt6400);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"EventArgs","EventArgs","",tolua_collect_EventArgs);
  #else
  tolua_cclass(tolua_S,"EventArgs","EventArgs","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"EventArgs");
   tolua_function(tolua_S,"new",tolua_JZServerExport_EventArgs_new00);
   tolua_function(tolua_S,"new_local",tolua_JZServerExport_EventArgs_new00_local);
   tolua_function(tolua_S,".call",tolua_JZServerExport_EventArgs_new00_local);
   tolua_function(tolua_S,"delete",tolua_JZServerExport_EventArgs_delete00);
   tolua_variable(tolua_S,"context",tolua_get_EventArgs_context,tolua_set_EventArgs_context);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"vector_int_","vector<int>","",tolua_collect_vector_int_);
  #else
  tolua_cclass(tolua_S,"vector_int_","vector<int>","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"vector_int_");
   tolua_function(tolua_S,"clear",tolua_JZServerExport_vector_int__clear00);
   tolua_function(tolua_S,"size",tolua_JZServerExport_vector_int__size00);
   tolua_function(tolua_S,".geti",tolua_JZServerExport_vector_int___geti00);
   tolua_function(tolua_S,".seti",tolua_JZServerExport_vector_int___seti00);
   tolua_function(tolua_S,".geti",tolua_JZServerExport_vector_int___geti01);
   tolua_function(tolua_S,"push_back",tolua_JZServerExport_vector_int__push_back00);
   tolua_function(tolua_S,"new",tolua_JZServerExport_vector_int__new00);
   tolua_function(tolua_S,"new_local",tolua_JZServerExport_vector_int__new00_local);
   tolua_function(tolua_S,".call",tolua_JZServerExport_vector_int__new00_local);
   tolua_function(tolua_S,"delete",tolua_JZServerExport_vector_int__delete00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"vector_string_","vector<string>","",tolua_collect_vector_string_);
  #else
  tolua_cclass(tolua_S,"vector_string_","vector<string>","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"vector_string_");
   tolua_function(tolua_S,"clear",tolua_JZServerExport_vector_string__clear00);
   tolua_function(tolua_S,"size",tolua_JZServerExport_vector_string__size00);
   tolua_function(tolua_S,".geti",tolua_JZServerExport_vector_string___geti00);
   tolua_function(tolua_S,".seti",tolua_JZServerExport_vector_string___seti00);
   tolua_function(tolua_S,".geti",tolua_JZServerExport_vector_string___geti01);
   tolua_function(tolua_S,"push_back",tolua_JZServerExport_vector_string__push_back00);
   tolua_function(tolua_S,"new",tolua_JZServerExport_vector_string__new00);
   tolua_function(tolua_S,"new_local",tolua_JZServerExport_vector_string__new00_local);
   tolua_function(tolua_S,".call",tolua_JZServerExport_vector_string__new00_local);
   tolua_function(tolua_S,"delete",tolua_JZServerExport_vector_string__delete00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"vector_double_","vector<double>","",tolua_collect_vector_double_);
  #else
  tolua_cclass(tolua_S,"vector_double_","vector<double>","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"vector_double_");
   tolua_function(tolua_S,"clear",tolua_JZServerExport_vector_double__clear00);
   tolua_function(tolua_S,"size",tolua_JZServerExport_vector_double__size00);
   tolua_function(tolua_S,".geti",tolua_JZServerExport_vector_double___geti00);
   tolua_function(tolua_S,".seti",tolua_JZServerExport_vector_double___seti00);
   tolua_function(tolua_S,".geti",tolua_JZServerExport_vector_double___geti01);
   tolua_function(tolua_S,"push_back",tolua_JZServerExport_vector_double__push_back00);
   tolua_function(tolua_S,"new",tolua_JZServerExport_vector_double__new00);
   tolua_function(tolua_S,"new_local",tolua_JZServerExport_vector_double__new00_local);
   tolua_function(tolua_S,".call",tolua_JZServerExport_vector_double__new00_local);
   tolua_function(tolua_S,"delete",tolua_JZServerExport_vector_double__delete00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"IObject","IObject","",NULL);
  tolua_beginmodule(tolua_S,"IObject");
   tolua_function(tolua_S,"getProperty",tolua_JZServerExport_IObject_getProperty00);
   tolua_function(tolua_S,"getProperty",tolua_JZServerExport_IObject_getProperty01);
   tolua_function(tolua_S,"getProperty",tolua_JZServerExport_IObject_getProperty02);
   tolua_function(tolua_S,"getInt64Property",tolua_JZServerExport_IObject_getInt64Property00);
   tolua_function(tolua_S,"setProperty",tolua_JZServerExport_IObject_setProperty00);
   tolua_function(tolua_S,"setProperty",tolua_JZServerExport_IObject_setProperty01);
   tolua_function(tolua_S,"setProperty",tolua_JZServerExport_IObject_setProperty02);
   tolua_function(tolua_S,"setInt64Property",tolua_JZServerExport_IObject_setInt64Property00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"IEntity","IEntity","IObject",NULL);
  tolua_beginmodule(tolua_S,"IEntity");
   tolua_function(tolua_S,"getHandle",tolua_JZServerExport_IEntity_getHandle00);
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"PROP_ENTITY_NAME",PROP_ENTITY_NAME);
  tolua_constant(tolua_S,"PROP_ENTITY_CLASS",PROP_ENTITY_CLASS);
  tolua_constant(tolua_S,"PROP_ENTITY_LEVEL",PROP_ENTITY_LEVEL);
  tolua_constant(tolua_S,"PROP_ACTOR_ACCOUNT",PROP_ACTOR_ACCOUNT);
  tolua_constant(tolua_S,"PROP_ACTOR_ROLEPOS",PROP_ACTOR_ROLEPOS);
  tolua_constant(tolua_S,"PROP_ACTOR_CREATEFINISH",PROP_ACTOR_CREATEFINISH);
  tolua_constant(tolua_S,"PROP_ENTITY_BATTLEPOS",PROP_ENTITY_BATTLEPOS);
  tolua_constant(tolua_S,"PROP_ENTITY_FIGHTVALUE",PROP_ENTITY_FIGHTVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_JOB",PROP_ENTITY_JOB);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEID",PROP_ENTITY_BASEID);
  tolua_constant(tolua_S,"PROP_ENTITY_ANGER",PROP_ENTITY_ANGER);
  tolua_constant(tolua_S,"PROP_ENTITY_LEVELSTEP",PROP_ENTITY_LEVELSTEP);
  tolua_constant(tolua_S,"PROP_ENTITY_EXP",PROP_ENTITY_EXP);
  tolua_constant(tolua_S,"PROP_ENTITY_HEROEXP",PROP_ENTITY_HEROEXP);
  tolua_constant(tolua_S,"PROP_ACTOR_RS",PROP_ACTOR_RS);
  tolua_constant(tolua_S,"PROP_ACTOR_CON",PROP_ACTOR_CON);
  tolua_constant(tolua_S,"PROP_ENTITY_HP",PROP_ENTITY_HP);
  tolua_constant(tolua_S,"PROP_ENTITY_MAXHP",PROP_ENTITY_MAXHP);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEMAXHP",PROP_ENTITY_BASEMAXHP);
  tolua_constant(tolua_S,"PROP_ENTITY_MAXHPAVALUE",PROP_ENTITY_MAXHPAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_MAXHPAPERCENT",PROP_ENTITY_MAXHPAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_HPGROW",PROP_ENTITY_HPGROW);
  tolua_constant(tolua_S,"PROP_ENTITY_ATT",PROP_ENTITY_ATT);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEATT",PROP_ENTITY_BASEATT);
  tolua_constant(tolua_S,"PROP_ENTITY_ATTAVALUE",PROP_ENTITY_ATTAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_ATTAPERCENT",PROP_ENTITY_ATTAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_ATTGROW",PROP_ENTITY_ATTGROW);
  tolua_constant(tolua_S,"PROP_ENTITY_HIT",PROP_ENTITY_HIT);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEHIT",PROP_ENTITY_BASEHIT);
  tolua_constant(tolua_S,"PROP_ENTITY_HITAVALUE",PROP_ENTITY_HITAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_HITAPERCENT",PROP_ENTITY_HITAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_HITGROW",PROP_ENTITY_HITGROW);
  tolua_constant(tolua_S,"PROP_ENTITY_DOGE",PROP_ENTITY_DOGE);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEDOGE",PROP_ENTITY_BASEDOGE);
  tolua_constant(tolua_S,"PROP_ENTITY_DOGEAVALUE",PROP_ENTITY_DOGEAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_DOGEAPERCENT",PROP_ENTITY_DOGEAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_KNOCK",PROP_ENTITY_KNOCK);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEKNOCK",PROP_ENTITY_BASEKNOCK);
  tolua_constant(tolua_S,"PROP_ENTITY_KNOCKAVALUE",PROP_ENTITY_KNOCKAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_KNOCKAPERCENT",PROP_ENTITY_KNOCKAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_KNOCKGROW",PROP_ENTITY_KNOCKGROW);
  tolua_constant(tolua_S,"PROP_ENTITY_ANTIKNOCK",PROP_ENTITY_ANTIKNOCK);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEANTIKNOCK",PROP_ENTITY_BASEANTIKNOCK);
  tolua_constant(tolua_S,"PROP_ENTITY_ANTIKNOCKAVALUE",PROP_ENTITY_ANTIKNOCKAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_ANTIKNOCKAPERCENT",PROP_ENTITY_ANTIKNOCKAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_BLOCK",PROP_ENTITY_BLOCK);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEBLOCK",PROP_ENTITY_BASEBLOCK);
  tolua_constant(tolua_S,"PROP_ENTITY_BLOCKAVALUE",PROP_ENTITY_BLOCKAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_BLOCKAPERCENT",PROP_ENTITY_BLOCKAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_WRECK",PROP_ENTITY_WRECK);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEWRECK",PROP_ENTITY_BASEWRECK);
  tolua_constant(tolua_S,"PROP_ENTITY_WRECKAVALUE",PROP_ENTITY_WRECKAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_WRECKAPERCENT",PROP_ENTITY_WRECKAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_ARMOR",PROP_ENTITY_ARMOR);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEARMOR",PROP_ENTITY_BASEARMOR);
  tolua_constant(tolua_S,"PROP_ENTITY_ARMORAVALUE",PROP_ENTITY_ARMORAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_ARMORAPERCENT",PROP_ENTITY_ARMORAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_SUNDER",PROP_ENTITY_SUNDER);
  tolua_constant(tolua_S,"PROP_ENTITY_BASESUNDER",PROP_ENTITY_BASESUNDER);
  tolua_constant(tolua_S,"PROP_ENTITY_SUNDERAVALUE",PROP_ENTITY_SUNDERAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_SUNDERAPERCENT",PROP_ENTITY_SUNDERAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_INITANGER",PROP_ENTITY_INITANGER);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEINITANGER",PROP_ENTITY_BASEINITANGER);
  tolua_constant(tolua_S,"PROP_ENTITY_INITANGERAVALUE",PROP_ENTITY_INITANGERAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_INITANGERAPERCENT",PROP_ENTITY_INITANGERAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_MASTER",PROP_ENTITY_MASTER);
  tolua_constant(tolua_S,"PROP_ENTITY_UUID",PROP_ENTITY_UUID);
  tolua_constant(tolua_S,"PROP_ENTITY_FIXTARGETSELECT",PROP_ENTITY_FIXTARGETSELECT);
  tolua_constant(tolua_S,"PROP_ENTITY_PHYSTRENGTH",PROP_ENTITY_PHYSTRENGTH);
  tolua_constant(tolua_S,"PROP_ENTITY_PACKFORGHOST",PROP_ENTITY_PACKFORGHOST);
  tolua_constant(tolua_S,"PROP_ACTOR_SILVER",PROP_ACTOR_SILVER);
  tolua_constant(tolua_S,"PROP_ACTOR_GOLD",PROP_ACTOR_GOLD);
  tolua_constant(tolua_S,"PROP_ACTOR_FSCHIPCOUNT",PROP_ACTOR_FSCHIPCOUNT);
  tolua_constant(tolua_S,"PROP_ENTITY_REPLACEHANDLE",PROP_ENTITY_REPLACEHANDLE);
  tolua_constant(tolua_S,"PROP_HERO_LVSTEPPROGRESS",PROP_HERO_LVSTEPPROGRESS);
  tolua_constant(tolua_S,"PROP_ENTITY_HEROCVTCOUNT",PROP_ENTITY_HEROCVTCOUNT);
  tolua_constant(tolua_S,"PROP_ENTITY_FUNCTIONMASK",PROP_ENTITY_FUNCTIONMASK);
  tolua_constant(tolua_S,"PROP_ENTITY_ISMACHINE",PROP_ENTITY_ISMACHINE);
  tolua_constant(tolua_S,"PROP_ACTOR_VIPLEVEL",PROP_ACTOR_VIPLEVEL);
  tolua_constant(tolua_S,"PROP_ACTOR_WORLD",PROP_ACTOR_WORLD);
  tolua_constant(tolua_S,"PROP_ENTITY_HONOR",PROP_ENTITY_HONOR);
  tolua_constant(tolua_S,"PROP_LAST_PVETIME",PROP_LAST_PVETIME);
  tolua_constant(tolua_S,"PROP_ENTITY_DIZZ",PROP_ENTITY_DIZZ);
  tolua_constant(tolua_S,"PROP_ENTITY_CONFUSE",PROP_ENTITY_CONFUSE);
  tolua_constant(tolua_S,"PROP_ENTITY_SEX",PROP_ENTITY_SEX);
  tolua_constant(tolua_S,"PROP_ENTITY_CONTINUEKILL",PROP_ENTITY_CONTINUEKILL);
  tolua_constant(tolua_S,"PROP_ENTITY_RELIVECOUNT",PROP_ENTITY_RELIVECOUNT);
  tolua_constant(tolua_S,"PROP_ENTITY_COMMSKILL_REPLACETTARGET",PROP_ENTITY_COMMSKILL_REPLACETTARGET);
  tolua_constant(tolua_S,"PROP_ENTITY_LASTPHYSTRENGTHTIME",PROP_ENTITY_LASTPHYSTRENGTHTIME);
  tolua_constant(tolua_S,"PROP_ENTITY_PHYSTRENGTHLIMIT",PROP_ENTITY_PHYSTRENGTHLIMIT);
  tolua_constant(tolua_S,"PROP_ACTOR_RECHARGED",PROP_ACTOR_RECHARGED);
  tolua_constant(tolua_S,"PROP_ENTITY_ISClOSEATTCOMMUPORNOT",PROP_ENTITY_ISClOSEATTCOMMUPORNOT);
  tolua_constant(tolua_S,"PROP_ACTOR_FIRSTCONSUME",PROP_ACTOR_FIRSTCONSUME);
  tolua_constant(tolua_S,"PROP_HERO_HASFIGHTSOUL",PROP_HERO_HASFIGHTSOUL);
  tolua_constant(tolua_S,"PROP_GODANIMALSOUL_ITEMID",PROP_GODANIMALSOUL_ITEMID);
  tolua_constant(tolua_S,"PROP_GODANIMALSOUL_ITEMCOUNT",PROP_GODANIMALSOUL_ITEMCOUNT);
  tolua_constant(tolua_S,"PROP_ACTOR_VIPEXP",PROP_ACTOR_VIPEXP);
  tolua_constant(tolua_S,"PROP_ACTOR_NOTICE",PROP_ACTOR_NOTICE);
  tolua_constant(tolua_S,"PROP_ENTITY_JUSTRELIVE",PROP_ENTITY_JUSTRELIVE);
  tolua_constant(tolua_S,"PROP_ENTITY_WRECKGROW",PROP_ENTITY_WRECKGROW);
  tolua_constant(tolua_S,"PROP_ENTITY_SKILLDAMAGE",PROP_ENTITY_SKILLDAMAGE);
  tolua_constant(tolua_S,"PROP_ENTITY_SKILLDEF",PROP_ENTITY_SKILLDEF);
  tolua_constant(tolua_S,"PROP_HERO_HASEQUIP",PROP_HERO_HASEQUIP);
  tolua_constant(tolua_S,"PROP_HERO_PRICE",PROP_HERO_PRICE);
  tolua_constant(tolua_S,"PROP_ENTITY_SAODANGLIMI",PROP_ENTITY_SAODANGLIMI);
  tolua_constant(tolua_S,"PROP_ENTITY_SAODANGPOINT",PROP_ENTITY_SAODANGPOINT);
  tolua_constant(tolua_S,"PROP_ENTITY_LASTSAODANG_RESUMETIME",PROP_ENTITY_LASTSAODANG_RESUMETIME);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEPROP_FIXPARAM",PROP_ENTITY_BASEPROP_FIXPARAM);
  tolua_constant(tolua_S,"PROP_ACTER_MAX_FIGHTVALUE",PROP_ACTER_MAX_FIGHTVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_RELIVEENHANCE",PROP_ENTITY_RELIVEENHANCE);
  tolua_constant(tolua_S,"PROP_ENTITY_REBOUNDPERCENT",PROP_ENTITY_REBOUNDPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_CONDITIONDEAD",PROP_ENTITY_CONDITIONDEAD);
  tolua_constant(tolua_S,"PROP_ENTITY_ACTOR_HEAD",PROP_ENTITY_ACTOR_HEAD);
  tolua_constant(tolua_S,"PROP_ENTITY_ACTOR_HEADTYPE",PROP_ENTITY_ACTOR_HEADTYPE);
  tolua_constant(tolua_S,"PROP_ENTITY_ACTOR_VIGOR",PROP_ENTITY_ACTOR_VIGOR);
  tolua_constant(tolua_S,"PROP_ENTITY_KNOCKENHANCEPERCENT",PROP_ENTITY_KNOCKENHANCEPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_ENHANCE_VAMPIREPERCENT",PROP_ENTITY_ENHANCE_VAMPIREPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_IMMUNITYDAMAGE",PROP_ENTITY_IMMUNITYDAMAGE);
  tolua_constant(tolua_S,"PROP_ENTITY_TOUCHDEADEFFECT_COUNT",PROP_ENTITY_TOUCHDEADEFFECT_COUNT);
  tolua_constant(tolua_S,"PROP_ENTITY_PERCENTDAMAGE_ENHANCE",PROP_ENTITY_PERCENTDAMAGE_ENHANCE);
  tolua_constant(tolua_S,"PROP_ENTITY_PERCENTDAMAGE_VAMPIREPERCENT",PROP_ENTITY_PERCENTDAMAGE_VAMPIREPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_KNOCK_REBACKPERCENT",PROP_ENTITY_KNOCK_REBACKPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_CONTINUESKILL_RATE",PROP_ENTITY_CONTINUESKILL_RATE);
  tolua_constant(tolua_S,"PROP_ENTITY_QUALITY",PROP_ENTITY_QUALITY);
  tolua_constant(tolua_S,"PROP_ENTITY_SMALLSKILLDAMAGE",PROP_ENTITY_SMALLSKILLDAMAGE);
  tolua_constant(tolua_S,"PROP_ENTITY_DEF",PROP_ENTITY_DEF);
  tolua_constant(tolua_S,"PROP_ENTITY_BASEDEF",PROP_ENTITY_BASEDEF);
  tolua_constant(tolua_S,"PROP_ENTITY_DEFAVALUE",PROP_ENTITY_DEFAVALUE);
  tolua_constant(tolua_S,"PROP_ENTITY_DEFAPERCENT",PROP_ENTITY_DEFAPERCENT);
  tolua_constant(tolua_S,"PROP_ENTITY_DEFGROW",PROP_ENTITY_DEFGROW);
  tolua_constant(tolua_S,"PROP_ENTITY_FIGHTVALUE_PREFIGHT",PROP_ENTITY_FIGHTVALUE_PREFIGHT);
  tolua_constant(tolua_S,"PROP_ENTITY_PREDEAD_USESKILLFLAG",PROP_ENTITY_PREDEAD_USESKILLFLAG);
  tolua_constant(tolua_S,"PROP_ENTITY_STRENGTHBUYTIMES",PROP_ENTITY_STRENGTHBUYTIMES);
  tolua_constant(tolua_S,"PROP_ACTOR_LEGION_CONTRIBUTE",PROP_ACTOR_LEGION_CONTRIBUTE);
  tolua_constant(tolua_S,"PROP_ACTOR_SHOWKNOCK_DAMAGE",PROP_ACTOR_SHOWKNOCK_DAMAGE);
  tolua_constant(tolua_S,"PROP_ACTOR_SHOWKNOCK_XIXUE",PROP_ACTOR_SHOWKNOCK_XIXUE);
  tolua_constant(tolua_S,"PROP_ACTOR_SHOWCONATTACK",PROP_ACTOR_SHOWCONATTACK);
  tolua_constant(tolua_S,"PROP_ACTOR_SHOWFIRSTPAY",PROP_ACTOR_SHOWFIRSTPAY);
  tolua_constant(tolua_S,"PROP_ACTOR_ACC_PAYMENT",PROP_ACTOR_ACC_PAYMENT);
  tolua_constant(tolua_S,"PROP_ENTITY_MAX",PROP_ENTITY_MAX);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_JZServerExport (lua_State* tolua_S) {
 return tolua_JZServerExport_open(tolua_S);
};
#endif

