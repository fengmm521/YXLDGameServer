/*
** Lua binding: AegisRobotInnerExport
** Generated automatically by tolua++-1.0.92 on Sun Jul 15 01:15:17 2018.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_AegisRobotInnerExport_open (lua_State* tolua_S);

#include "RobotServerPch.h"
#include "ClientPlayer.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_EventArgs (lua_State* tolua_S)
{
 EventArgs* self = (EventArgs*) tolua_tousertype(tolua_S,1,0);
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

static int tolua_collect_Int64 (lua_State* tolua_S)
{
 Int64* self = (Int64*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_vector_double_ (lua_State* tolua_S)
{
 vector<double>* self = (vector<double>*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"PropertySet");
 tolua_usertype(tolua_S,"_PropertySetT<string>");
 tolua_usertype(tolua_S,"vector<string>");
 tolua_usertype(tolua_S,"vector<int>");
 tolua_usertype(tolua_S,"Int64");
 tolua_usertype(tolua_S,"vector<double>");
 tolua_usertype(tolua_S,"EventArgs");
 tolua_usertype(tolua_S,"ClientPlayer");
}

/* method: sendBuff of class  ClientPlayer */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_ClientPlayer_sendBuff00
static int tolua_AegisRobotInnerExport_ClientPlayer_sendBuff00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ClientPlayer",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ClientPlayer* self = (ClientPlayer*)  tolua_tousertype(tolua_S,1,0);
  const char* pData = ((const char*)  tolua_tostring(tolua_S,2,0));
  int iLen = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendBuff'", NULL);
#endif
  {
   self->sendBuff(pData,iLen);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendBuff'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendGmMsg of class  ClientPlayer */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_ClientPlayer_sendGmMsg00
static int tolua_AegisRobotInnerExport_ClientPlayer_sendGmMsg00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ClientPlayer",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ClientPlayer* self = (ClientPlayer*)  tolua_tousertype(tolua_S,1,0);
  const string strCmd = ((const string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendGmMsg'", NULL);
#endif
  {
   self->sendGmMsg(strCmd);
   tolua_pushcppstring(tolua_S,(const char*)strCmd);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendGmMsg'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendMessage of class  ClientPlayer */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_ClientPlayer_sendMessage00
static int tolua_AegisRobotInnerExport_ClientPlayer_sendMessage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ClientPlayer",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ClientPlayer* self = (ClientPlayer*)  tolua_tousertype(tolua_S,1,0);
  int iMsgID = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendMessage'", NULL);
#endif
  {
   self->sendMessage(iMsgID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendMessage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getAccount of class  ClientPlayer */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_ClientPlayer_getAccount00
static int tolua_AegisRobotInnerExport_ClientPlayer_getAccount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ClientPlayer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ClientPlayer* self = (ClientPlayer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getAccount'", NULL);
#endif
  {
   string tolua_ret = (string)  self->getAccount();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getAccount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getInt of class  _PropertySetT<string> */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport__PropertySetT_string__getInt00
static int tolua_AegisRobotInnerExport__PropertySetT_string__getInt00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport__PropertySetT_string__getFloat00
static int tolua_AegisRobotInnerExport__PropertySetT_string__getFloat00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport__PropertySetT_string__getString00
static int tolua_AegisRobotInnerExport__PropertySetT_string__getString00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport__PropertySetT_string__getInt6400
static int tolua_AegisRobotInnerExport__PropertySetT_string__getInt6400(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_PropertySet_setInt00
static int tolua_AegisRobotInnerExport_PropertySet_setInt00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_PropertySet_setString00
static int tolua_AegisRobotInnerExport_PropertySet_setString00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_PropertySet_setFloat00
static int tolua_AegisRobotInnerExport_PropertySet_setFloat00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_PropertySet_setInt6400
static int tolua_AegisRobotInnerExport_PropertySet_setInt6400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"Int64",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PropertySet* self = (PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
  Int64 i64Value = *((Int64*)  tolua_tousertype(tolua_S,3,0));
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_PropertySet_getInt00
static int tolua_AegisRobotInnerExport_PropertySet_getInt00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_PropertySet_getFloat00
static int tolua_AegisRobotInnerExport_PropertySet_getFloat00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_PropertySet_getString00
static int tolua_AegisRobotInnerExport_PropertySet_getString00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_PropertySet_getInt6400
static int tolua_AegisRobotInnerExport_PropertySet_getInt6400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PropertySet",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"Int64",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PropertySet* self = (const PropertySet*)  tolua_tousertype(tolua_S,1,0);
  int iKey = ((int)  tolua_tonumber(tolua_S,2,0));
  Int64 i64Def = *((Int64*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getInt64'", NULL);
#endif
  {
   Int64 tolua_ret = (Int64)  self->getInt64(iKey,i64Def);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Int64)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Int64");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Int64));
     tolua_pushusertype(tolua_S,tolua_obj,"Int64");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_EventArgs_new00
static int tolua_AegisRobotInnerExport_EventArgs_new00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_EventArgs_new00_local
static int tolua_AegisRobotInnerExport_EventArgs_new00_local(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_EventArgs_delete00
static int tolua_AegisRobotInnerExport_EventArgs_delete00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int__clear00
static int tolua_AegisRobotInnerExport_vector_int__clear00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int__size00
static int tolua_AegisRobotInnerExport_vector_int__size00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int___geti00
static int tolua_AegisRobotInnerExport_vector_int___geti00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int___seti00
static int tolua_AegisRobotInnerExport_vector_int___seti00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int___geti01
static int tolua_AegisRobotInnerExport_vector_int___geti01(lua_State* tolua_S)
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
 return tolua_AegisRobotInnerExport_vector_int___geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  vector<int> */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int__push_back00
static int tolua_AegisRobotInnerExport_vector_int__push_back00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int__new00
static int tolua_AegisRobotInnerExport_vector_int__new00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int__new00_local
static int tolua_AegisRobotInnerExport_vector_int__new00_local(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_int__delete00
static int tolua_AegisRobotInnerExport_vector_int__delete00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string__clear00
static int tolua_AegisRobotInnerExport_vector_string__clear00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string__size00
static int tolua_AegisRobotInnerExport_vector_string__size00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string___geti00
static int tolua_AegisRobotInnerExport_vector_string___geti00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string___seti00
static int tolua_AegisRobotInnerExport_vector_string___seti00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string___geti01
static int tolua_AegisRobotInnerExport_vector_string___geti01(lua_State* tolua_S)
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
 return tolua_AegisRobotInnerExport_vector_string___geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  vector<string> */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string__push_back00
static int tolua_AegisRobotInnerExport_vector_string__push_back00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string__new00
static int tolua_AegisRobotInnerExport_vector_string__new00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string__new00_local
static int tolua_AegisRobotInnerExport_vector_string__new00_local(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_string__delete00
static int tolua_AegisRobotInnerExport_vector_string__delete00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double__clear00
static int tolua_AegisRobotInnerExport_vector_double__clear00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double__size00
static int tolua_AegisRobotInnerExport_vector_double__size00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double___geti00
static int tolua_AegisRobotInnerExport_vector_double___geti00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double___seti00
static int tolua_AegisRobotInnerExport_vector_double___seti00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double___geti01
static int tolua_AegisRobotInnerExport_vector_double___geti01(lua_State* tolua_S)
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
 return tolua_AegisRobotInnerExport_vector_double___geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  vector<double> */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double__push_back00
static int tolua_AegisRobotInnerExport_vector_double__push_back00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double__new00
static int tolua_AegisRobotInnerExport_vector_double__new00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double__new00_local
static int tolua_AegisRobotInnerExport_vector_double__new00_local(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_vector_double__delete00
static int tolua_AegisRobotInnerExport_vector_double__delete00(lua_State* tolua_S)
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

/* function: getDataPath */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_getDataPath00
static int tolua_AegisRobotInnerExport_getDataPath00(lua_State* tolua_S)
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
   string tolua_ret = (string)  getDataPath();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getDataPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: getRunSecond */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_getRunSecond00
static int tolua_AegisRobotInnerExport_getRunSecond00(lua_State* tolua_S)
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
   int tolua_ret = (int)  getRunSecond();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getRunSecond'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: logMsg */
#ifndef TOLUA_DISABLE_tolua_AegisRobotInnerExport_logMsg00
static int tolua_AegisRobotInnerExport_logMsg00(lua_State* tolua_S)
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
  const string strMgs = ((const string)  tolua_tocppstring(tolua_S,1,0));
  {
   logMsg(strMgs);
   tolua_pushcppstring(tolua_S,(const char*)strMgs);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'logMsg'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_AegisRobotInnerExport_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"ClientPlayer","ClientPlayer","",NULL);
  tolua_beginmodule(tolua_S,"ClientPlayer");
   tolua_function(tolua_S,"sendBuff",tolua_AegisRobotInnerExport_ClientPlayer_sendBuff00);
   tolua_function(tolua_S,"sendGmMsg",tolua_AegisRobotInnerExport_ClientPlayer_sendGmMsg00);
   tolua_function(tolua_S,"sendMessage",tolua_AegisRobotInnerExport_ClientPlayer_sendMessage00);
   tolua_function(tolua_S,"getAccount",tolua_AegisRobotInnerExport_ClientPlayer_getAccount00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"_PropertySetT_string_","_PropertySetT<string>","",NULL);
  tolua_beginmodule(tolua_S,"_PropertySetT_string_");
   tolua_function(tolua_S,"getInt",tolua_AegisRobotInnerExport__PropertySetT_string__getInt00);
   tolua_function(tolua_S,"getFloat",tolua_AegisRobotInnerExport__PropertySetT_string__getFloat00);
   tolua_function(tolua_S,"getString",tolua_AegisRobotInnerExport__PropertySetT_string__getString00);
   tolua_function(tolua_S,"getInt64",tolua_AegisRobotInnerExport__PropertySetT_string__getInt6400);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"PropertySet","PropertySet","",NULL);
  tolua_beginmodule(tolua_S,"PropertySet");
   tolua_function(tolua_S,"setInt",tolua_AegisRobotInnerExport_PropertySet_setInt00);
   tolua_function(tolua_S,"setString",tolua_AegisRobotInnerExport_PropertySet_setString00);
   tolua_function(tolua_S,"setFloat",tolua_AegisRobotInnerExport_PropertySet_setFloat00);
   tolua_function(tolua_S,"setInt64",tolua_AegisRobotInnerExport_PropertySet_setInt6400);
   tolua_function(tolua_S,"getInt",tolua_AegisRobotInnerExport_PropertySet_getInt00);
   tolua_function(tolua_S,"getFloat",tolua_AegisRobotInnerExport_PropertySet_getFloat00);
   tolua_function(tolua_S,"getString",tolua_AegisRobotInnerExport_PropertySet_getString00);
   tolua_function(tolua_S,"getInt64",tolua_AegisRobotInnerExport_PropertySet_getInt6400);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"EventArgs","EventArgs","",tolua_collect_EventArgs);
  #else
  tolua_cclass(tolua_S,"EventArgs","EventArgs","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"EventArgs");
   tolua_function(tolua_S,"new",tolua_AegisRobotInnerExport_EventArgs_new00);
   tolua_function(tolua_S,"new_local",tolua_AegisRobotInnerExport_EventArgs_new00_local);
   tolua_function(tolua_S,".call",tolua_AegisRobotInnerExport_EventArgs_new00_local);
   tolua_function(tolua_S,"delete",tolua_AegisRobotInnerExport_EventArgs_delete00);
   tolua_variable(tolua_S,"context",tolua_get_EventArgs_context,tolua_set_EventArgs_context);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"vector_int_","vector<int>","",tolua_collect_vector_int_);
  #else
  tolua_cclass(tolua_S,"vector_int_","vector<int>","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"vector_int_");
   tolua_function(tolua_S,"clear",tolua_AegisRobotInnerExport_vector_int__clear00);
   tolua_function(tolua_S,"size",tolua_AegisRobotInnerExport_vector_int__size00);
   tolua_function(tolua_S,".geti",tolua_AegisRobotInnerExport_vector_int___geti00);
   tolua_function(tolua_S,".seti",tolua_AegisRobotInnerExport_vector_int___seti00);
   tolua_function(tolua_S,".geti",tolua_AegisRobotInnerExport_vector_int___geti01);
   tolua_function(tolua_S,"push_back",tolua_AegisRobotInnerExport_vector_int__push_back00);
   tolua_function(tolua_S,"new",tolua_AegisRobotInnerExport_vector_int__new00);
   tolua_function(tolua_S,"new_local",tolua_AegisRobotInnerExport_vector_int__new00_local);
   tolua_function(tolua_S,".call",tolua_AegisRobotInnerExport_vector_int__new00_local);
   tolua_function(tolua_S,"delete",tolua_AegisRobotInnerExport_vector_int__delete00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"vector_string_","vector<string>","",tolua_collect_vector_string_);
  #else
  tolua_cclass(tolua_S,"vector_string_","vector<string>","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"vector_string_");
   tolua_function(tolua_S,"clear",tolua_AegisRobotInnerExport_vector_string__clear00);
   tolua_function(tolua_S,"size",tolua_AegisRobotInnerExport_vector_string__size00);
   tolua_function(tolua_S,".geti",tolua_AegisRobotInnerExport_vector_string___geti00);
   tolua_function(tolua_S,".seti",tolua_AegisRobotInnerExport_vector_string___seti00);
   tolua_function(tolua_S,".geti",tolua_AegisRobotInnerExport_vector_string___geti01);
   tolua_function(tolua_S,"push_back",tolua_AegisRobotInnerExport_vector_string__push_back00);
   tolua_function(tolua_S,"new",tolua_AegisRobotInnerExport_vector_string__new00);
   tolua_function(tolua_S,"new_local",tolua_AegisRobotInnerExport_vector_string__new00_local);
   tolua_function(tolua_S,".call",tolua_AegisRobotInnerExport_vector_string__new00_local);
   tolua_function(tolua_S,"delete",tolua_AegisRobotInnerExport_vector_string__delete00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"vector_double_","vector<double>","",tolua_collect_vector_double_);
  #else
  tolua_cclass(tolua_S,"vector_double_","vector<double>","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"vector_double_");
   tolua_function(tolua_S,"clear",tolua_AegisRobotInnerExport_vector_double__clear00);
   tolua_function(tolua_S,"size",tolua_AegisRobotInnerExport_vector_double__size00);
   tolua_function(tolua_S,".geti",tolua_AegisRobotInnerExport_vector_double___geti00);
   tolua_function(tolua_S,".seti",tolua_AegisRobotInnerExport_vector_double___seti00);
   tolua_function(tolua_S,".geti",tolua_AegisRobotInnerExport_vector_double___geti01);
   tolua_function(tolua_S,"push_back",tolua_AegisRobotInnerExport_vector_double__push_back00);
   tolua_function(tolua_S,"new",tolua_AegisRobotInnerExport_vector_double__new00);
   tolua_function(tolua_S,"new_local",tolua_AegisRobotInnerExport_vector_double__new00_local);
   tolua_function(tolua_S,".call",tolua_AegisRobotInnerExport_vector_double__new00_local);
   tolua_function(tolua_S,"delete",tolua_AegisRobotInnerExport_vector_double__delete00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"getDataPath",tolua_AegisRobotInnerExport_getDataPath00);
  tolua_function(tolua_S,"getRunSecond",tolua_AegisRobotInnerExport_getRunSecond00);
  tolua_function(tolua_S,"logMsg",tolua_AegisRobotInnerExport_logMsg00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_AegisRobotInnerExport (lua_State* tolua_S) {
 return tolua_AegisRobotInnerExport_open(tolua_S);
};
#endif

