/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated
** All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the
** terms of the Adobe license agreement accompanying it.  If you have received this file from a
** source other than Adobe, then your use, modification, or distribution of it requires the prior
** written permission of Adobe.
*/
// Flash Runtime interop

#define lbitlib_c
#define LUA_LIB

#include "AS3/AS3.h"
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

// ===============================================================
//                    Flash Object Interop
// ===============================================================

static int flash_getprop (lua_State *L);
static int flash_setprop (lua_State *L);
static int flash_call (lua_State *L);
static int flash_apply (lua_State *L);


package_as3(
  "#package public\n"
  "import flash.utils.Dictionary;\n"
  "public var __lua_objrefs:Dictionary = new Dictionary();\n"
);

#define FlashObjectType "flash"
#define FlashObj unsigned int

static int flashref = -1;

static int typeerror (lua_State *L, int narg, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                    tname, luaL_typename(L, narg));
  return luaL_argerror(L, narg, msg);
}

void clean_flashstate()
{
  flashref = -1;
}

static void *luaL_testflashudata (lua_State *L, int ud) {
  void *p = lua_touserdata(L, ud);
  if (p != NULL) {  /* value is a userdata? */
    if (lua_getmetatable(L, ud)) {  /* does it have a metatable? */
      if(flashref == -1) {
        luaL_getmetatable(L, FlashObjectType);
        flashref = luaL_ref(L, LUA_REGISTRYINDEX);
      }
      lua_rawgeti(L, LUA_REGISTRYINDEX, flashref);
      if (!lua_rawequal(L, -1, -2)) { /* not the same? */
        p = NULL;  /* value is a userdata with wrong metatable */
      }
      lua_pop(L, 2);  /* remove both metatables */
      return p;
    }
  }
  return NULL;  /* value is not a userdata with a metatable */
}

static FlashObj* getObjRef(lua_State *L, int idx)
{
  FlashObj* r = (FlashObj*)luaL_testflashudata(L, idx);
  if (r == NULL) typeerror(L, idx, FlashObjectType);
  return r;
}

static int FlashObj_gc(lua_State *L)
{
  FlashObj *obj = getObjRef(L, 1);
  //inline_as3("trace(\"gc: \" + %0);\n" :  : "r"(obj));
  lua_pop(L, 1);
  return 0;
}

static int FlashObj_tostring(lua_State *L)
{
  FlashObj obj = getObjRef(L, 1);
  char *str = NULL;
  lua_pop(L, 1);
  inline_as3("%0 = CModule.mallocString(\"\"+__lua_objrefs[%1]);\n" : "=r"(str) : "r"(obj));
  lua_pushfstring(L, "%s", str);
  free(str);
  return 1;
}

static const luaL_Reg FlashObj_meta[] = {
  {"__gc",        FlashObj_gc},
  {"__tostring",  FlashObj_tostring},
  {"__index",     flash_getprop},
  {"__newindex",  flash_setprop},
  {"__call",      flash_apply},
  {0, 0}
};

FlashObj* push_newflashref(lua_State *L)
{
  // Push the new userdata onto the stack
  FlashObj *result = (FlashObj*)lua_newuserdata(L, sizeof(FlashObj));
  luaL_getmetatable(L, FlashObjectType);
  lua_setmetatable(L, -2);
  //inline_nonreentrant_as3("trace(\"ref \" + %0);\n" :  : "r"(result));
  return result;
}

// ===============================================================
//                    Flash API Interop
// ===============================================================

static int flash_trace (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  AS3_DeclareVar(str, String);
  AS3_CopyCStringToVar(str, s, l);
  lua_pop(L, 1);
  inline_nonreentrant_as3("trace(str);\n");
  return 1;
}

static int flash_gettimer (lua_State *L) {
  int result;
  inline_nonreentrant_as3("import flash.utils.getTimer;\n%0 = getTimer();\n" : "=r"(result));
  lua_pushinteger(L, result);
  return 1;
}

// getters

static int flash_getprop (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s, l);
  lua_pop(L, 2);
  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  // Get the prop, and store it with the new key
  inline_as3("__lua_objrefs[%0] = __lua_objrefs[%1][propname];\n" : : "r"(result), "r"(obj));
  return 1;
}

static int flash_getx (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_Number result = 0;
  lua_pop(L, 1);
  inline_as3("%0 = Number(__lua_objrefs[%1].x);\n" : "=r"(result) : "r"(obj));
  lua_pushnumber(L, result);
  return 1;
}

static int flash_gety (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_Number result = 0;
  lua_pop(L, 1);
  inline_as3("%0 = Number(__lua_objrefs[%1].y);\n" : "=r"(result) : "r"(obj));
  lua_pushnumber(L, result);
  return 1;
}

static int flash_getnumber (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s, l);
  lua_Number result = 0;
  lua_pop(L, 2);
  inline_as3("%0 = Number(__lua_objrefs[%1][propname]);\n" : "=r"(result) : "r"(obj));
  lua_pushnumber(L, result);
  return 1;
}

static int flash_getuint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s, l);
  unsigned int result = 0;
  lua_pop(L, 2);
  inline_as3("%0 = uint(__lua_objrefs[%1][propname]);\n" : "=r"(result) : "r"(obj));
  lua_pushunsigned(L, result);
  return 1;
}

static int flash_getint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s, l);
  int result = 0;
  lua_pop(L, 2);
  inline_as3("%0 = int(__lua_objrefs[%1][propname]);\n" : "=r"(result) : "r"(obj));
  lua_pushinteger(L, result);
  return 1;
}

static int flash_getstring (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s1 = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s1, l);
  char *str = NULL;
  lua_pop(L, 2);
  inline_as3("%0 = CModule.mallocString(\"\"+__lua_objrefs[%1][%2]);\n" : "=r"(str) : "r"(obj), "r"(s1));
  lua_pushfstring(L, "%s", str);
  free(str);
  return 1;
}

static int flash_getidxint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  int result;
  unsigned int idx = luaL_checkunsigned(L, 2);
  inline_as3("%0 = __lua_objrefs[%2][uint(%2)];\n" : "=r"(result) : "r"(obj), "r"(idx));
  lua_pop(L, 2);
  lua_pushinteger(L, result);
  return 1;
}

static int flash_getidxuint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  unsigned int result;
  unsigned int idx = luaL_checkunsigned(L, 2);
  inline_as3("%0 = __lua_objrefs[%2][uint(%2)];\n" : "=r"(result) : "r"(obj), "r"(idx));
  lua_pop(L, 2);
  lua_pushunsigned(L, result);
  return 1;
}

static int flash_getidxnumber (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_Number result;
  unsigned int idx = luaL_checkunsigned(L, 2);
  inline_as3("%0 = __lua_objrefs[%2][uint(%2)];\n" : "=r"(result) : "r"(obj), "r"(idx));
  lua_pop(L, 2);
  lua_pushnumber(L, result);
  return 1;
}

// setters

static int flash_setprop (lua_State *L) {
  FlashObj o1 = getObjRef(L, 1);
  size_t l;
  const char *s = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s, l);

  switch(lua_type(L, 3)) {
      case LUA_TBOOLEAN: inline_as3("__lua_objrefs[%0][propname] = %1;\n" : : "r"(o1), "r"(lua_toboolean(L, 3))); break;
      case LUA_TNUMBER: inline_as3("__lua_objrefs[%0][propname] = %1;\n" : : "r"(o1), "r"(luaL_checknumber(L, 3))); break;
      case LUA_TFUNCTION:
      {
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        AS3_DeclareVar(luastate, int);
        AS3_CopyScalarToVar(luastate, L);
        inline_as3(
        "__lua_objrefs[%0][propname] = function(...vaargs):void"
        "{"
        "  Lua.lua_rawgeti(luastate, %2, %1);"
        "  for(var i:int = 0; i<vaargs.length;i++) {"
        "    var udptr:int = Lua.push_flashref(luastate);"
        "    __lua_objrefs[udptr] = vaargs[i];"
        "  };"
        "  Lua.lua_callk(luastate, vaargs.length, 0, 0, null);"
        "};\n" : : "r"(o1), "r"(ref), "r"(LUA_REGISTRYINDEX));
        break;
      }
      case LUA_TUSERDATA: inline_as3("__lua_objrefs[%0][propname] = %1;\n" : : "r"(o1), "r"(getObjRef(L, 1))); break;
      case LUA_TSTRING:
      {
        const char *s = luaL_checklstring(L, 3, &l);
        AS3_DeclareVar(strvar, String);
        AS3_CopyCStringToVar(strvar, s, l);
        inline_as3("__lua_objrefs[%0][propname] = strvar;\n" : : "r"(o1));
        break;
      }
      default:
        inline_as3("trace(\"unknown: \" + %0);\n" :  : "r"(lua_type(L, 3)));
        return 0;
    }

  lua_pop(L, 3);
  return 1;
}

static int flash_setx (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_Number val = luaL_checknumber(L, 2);
  inline_as3("__lua_objrefs[%0].x = %1;\n" : : "r"(obj), "r"(val));
  lua_pop(L, 2);
  return 1;
}

static int flash_sety (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_Number val = luaL_checknumber(L, 2);
  inline_as3("__lua_objrefs[%0].y = %1;\n" : : "r"(obj), "r"(val));
  lua_pop(L, 2);
  return 1;
}

static int flash_setnumber (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s, l);
  lua_Number val = luaL_checknumber(L, 3);
  inline_as3("__lua_objrefs[%0][propname] = %1;\n" : : "r"(obj), "r"(val));
  lua_pop(L, 3);
  return 1;
}

static int flash_setuint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s, l);
  unsigned int val = luaL_checkunsigned(L, 3);
  inline_as3("__lua_objrefs[%0][propname] = %1;\n" : : "r"(obj), "r"(val));
  lua_pop(L, 3);
  return 1;
}

static int flash_setint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s, l);
  int val = luaL_checkinteger(L, 3);
  inline_as3("__lua_objrefs[%0][propname] = %1;\n" : : "r"(obj), "r"(val));
  lua_pop(L, 3);
  return 1;
}

static int flash_setstring (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  const char *s1 = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s1, l);
  const char *s2 = luaL_checklstring(L, 3, &l);
  AS3_DeclareVar(propname, String);
  AS3_CopyCStringToVar(propname, s2, l);
  inline_as3("__lua_objrefs[%0][propname] = val;\n" : : "r"(obj));
  lua_pop(L, 3);
  return 1;
}

static int flash_setidxint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  unsigned int idx = luaL_checkunsigned(L, 2);
  int val = luaL_checkinteger(L, 3);
  inline_as3("__lua_objrefs[%0][uint(%1)] = %2;\n" : : "r"(obj), "r"(idx), "r"(val));
  lua_pop(L, 3);
  return 1;
}

static int flash_setidxuint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  unsigned int idx = luaL_checkunsigned(L, 2);
  unsigned int val = luaL_checkunsigned(L, 3);
  inline_as3("__lua_objrefs[%0][uint(%1)] = %2;\n" : : "r"(obj), "r"(idx), "r"(val));
  lua_pop(L, 3);
  return 1;
}

static int flash_setidxnumber (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  size_t l;
  unsigned int idx = luaL_checkunsigned(L, 2);
  lua_Number val = luaL_checknumber(L, 3);
  inline_as3("__lua_objrefs[%0][uint(%1)] = %2;\n" : : "r"(obj), "r"(idx), "r"(val));
  lua_pop(L, 3);
  return 1;
}

// coercion

static int flash_asnumber (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_pop(L, 1);
  lua_Number result;
  inline_as3("%0 = __lua_objrefs[%1] as Number;\n" : "=r"(result) : "r"(obj));
  lua_pushnumber(L, result);
  return 1;
}

static int flash_asint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_pop(L, 1);
  int result;
  inline_as3("%0 = __lua_objrefs[%1] as int;\n" : "=r"(result) : "r"(obj));
  lua_pushinteger(L, result);
  return 1;
}

static int flash_asuint (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_pop(L, 1);
  unsigned int result;
  inline_as3("%0 = __lua_objrefs[%1] as int;\n" : "=r"(result) : "r"(obj));
  lua_pushunsigned(L, result);
  return 1;
}

static int flash_asstring (lua_State *L) {
  FlashObj obj = getObjRef(L, 1);
  lua_pop(L, 1);
  char *result = NULL;
  inline_as3("%0 = CModule.mallocString(\"\"+__lua_objrefs[%1] as String);\n" : "=r"(result) : "r"(obj));
  lua_pushfstring(L, result);
  free(result);
  return 1;
}

// Object construction

static int flash_new (lua_State *L) {
  int top = lua_gettop(L);

  size_t l;
  const char *s1 = luaL_checklstring(L, 1, &l);
  AS3_DeclareVar(classname, String);
  AS3_CopyCStringToVar(classname, s1, l);

  inline_as3("import flash.utils.getDefinitionByName;\n");
  inline_as3("var clz:Class = getDefinitionByName(classname);\n");
  inline_as3("var args:Array = [];\n");

  int i = 2;
  while(i <= top) {
    switch(lua_type(L, i)) {
      case LUA_TBOOLEAN: inline_as3("args.push(%0);\n" : : "r"(lua_toboolean(L, i))); break;
      case LUA_TNUMBER: inline_as3("args.push(%0);\n" : : "r"(luaL_checknumber(L, i))); break;
      case LUA_TFUNCTION:
      {
        lua_settop(L, top+1);
        lua_copy(L, i, top+1);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_settop(L, top);
        AS3_DeclareVar(luastate, int);
        AS3_CopyScalarToVar(luastate, L);
        inline_as3(
        "args.push(function(...vaargs):void"
        "{"
        "  Lua.lua_rawgeti(luastate, %1, %0);"
        "  for(var i:int = 0; i<vaargs.length;i++) {"
        "    var udptr:int = Lua.push_flashref(luastate);"
        "    __lua_objrefs[udptr] = vaargs[i];"
        "  };"
        "  Lua.lua_callk(luastate, vaargs.length, 0, 0, null);"
        "});" : : "r"(ref), "r"(LUA_REGISTRYINDEX));
        break;
      }
      case LUA_TUSERDATA: inline_as3("args.push(__lua_objrefs[%0]);\n" : : "r"(luaL_checkudata(L, i, FlashObjectType))); break;
      case LUA_TSTRING:
      {
        const char *s = luaL_checklstring(L, i, &l);
        AS3_DeclareVar(strvar, String);
        AS3_CopyCStringToVar(strvar, s, l);
        inline_as3("args.push(strvar);\n");
        break;
      }
      default:
        inline_as3("trace(\"unknown: \" + %0 + \",\" + %1+ \",\" + %2);\n" :  : "r"(i), "r"(top), "r"(lua_type(L, i)));
        return 0;
    }
    i++;
  }
  
  // Flush all args off the stack
  lua_pop(L, top);

  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  inline_as3("switch(args.length) { \n"
    "  case 0: __lua_objrefs[%0] = new clz(); break;\n"
    "  case 1: __lua_objrefs[%0] = new clz(args[0]); break;\n"
    "  case 2: __lua_objrefs[%0] = new clz(args[0], args[1]); break;\n"
    "  case 3: __lua_objrefs[%0] = new clz(args[0], args[1], args[2]); break;\n"
    "  case 4: __lua_objrefs[%0] = new clz(args[0], args[1], args[2], args[3]); break;\n"
    "  case 5: __lua_objrefs[%0] = new clz(args[0], args[1], args[2], args[3], args[4]); break;\n" 
    "  case 6: __lua_objrefs[%0] = new clz(args[0], args[1], args[2], args[3], args[4], args[5]); break;\n"
    "  case 7: __lua_objrefs[%0] = new clz(args[0], args[1], args[2], args[3], args[4], args[5], args[6]); break;\n"
    "  case 8: __lua_objrefs[%0] = new clz(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]); break;\n"
    "  case 9: __lua_objrefs[%0] = new clz(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]); break;\n"
    "};\n"
    : : "r"(result)
  );
  return 1;
}

static int flash_newcallback (lua_State *L) {
  size_t l;
  const char *s1 = luaL_checklstring(L, 1, &l);
  AS3_DeclareVar(funcname, String);
  AS3_CopyCStringToVar(funcname, s1, l);
  // Flush all args off the stack
  lua_pop(L, 1);

  AS3_DeclareVar(luastate, int);
  AS3_CopyScalarToVar(luastate, L);

  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  inline_as3("__lua_objrefs[%0] = function(arg1:*):void"
  "{"
  "  Lua.lua_getglobal(luastate, funcname);"
  "  for(var i:int = 0; i<arguments.length;i++) {"
  "    var udptr:int = Lua.push_flashref(luastate);"
  "    __lua_objrefs[udptr] = arguments[i];"
  "  };"
  "  Lua.lua_callk(luastate, arguments.length, 0, 0, null);"
  "};\n"
  ::"r"(result));

  return 1;
}

static int flash_newintvec (lua_State *L) {
  int veclen = luaL_checkinteger(L, 1);

  // Flush all args off the stack
  lua_pop(L, 1);

  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  inline_as3("__lua_objrefs[%0] = new Vector.<int>(%1);\n" : : "r"(result), "r"(veclen));
  return 1;
}

static int flash_newuintvec (lua_State *L) {
  int veclen = luaL_checkinteger(L, 1);
  int top = lua_gettop(L);
  // Flush all args off the stack
  lua_pop(L, 1);

  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  inline_as3("__lua_objrefs[%0] = new Vector.<uint>(%1);\n" : : "r"(result), "r"(veclen), "r"(top));
  return 1;
}

static int flash_newnumbervec (lua_State *L) {
  int veclen = luaL_checkinteger(L, 1);
  int top = lua_gettop(L);
  // Flush all args off the stack
  lua_pop(L, 1);

  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  inline_as3("__lua_objrefs[%0] = new Vector.<Number>(%1);\n" : : "r"(result), "r"(veclen), "r"(top));
  return 1;
}

// method calls

static int flash_apply (lua_State *L) {
  int top = lua_gettop(L);

  FlashObj *funcobj = getObjRef(L, 1);
  FlashObj *thisobj = getObjRef(L, 2);

  inline_as3("var args:Array = [];\n");

  int i = 3;
  while(i <= top) {
    switch(lua_type(L, i)) {
      case LUA_TBOOLEAN: inline_as3("args.push(%0);\n" : : "r"(lua_toboolean(L, i))); break;
      case LUA_TNUMBER: inline_as3("args.push(%0);\n" : : "r"(luaL_checknumber(L, i))); break;
      case LUA_TFUNCTION:
      {
        lua_settop(L, top+1);
        lua_copy(L, i, top+1);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        AS3_DeclareVar(luastate, int);
        AS3_CopyScalarToVar(luastate, L);
        inline_as3(
        "args.push(function(...vaargs):void"
        "{"
        "  Lua.lua_rawgeti(luastate, %1, %0);"
        "  for(var i:int = 0; i<vaargs.length;i++) {"
        "    var udptr:int = Lua.push_flashref(luastate);"
        "    __lua_objrefs[udptr] = vaargs[i];"
        "  };"
        "  Lua.lua_callk(luastate, vaargs.length, 0, 0, null);"
        "});" : : "r"(ref), "r"(LUA_REGISTRYINDEX));
        break;
      }
      case LUA_TUSERDATA: inline_as3("args.push(__lua_objrefs[%0]);\n" : : "r"(getObjRef(L, i))); break;
      case LUA_TSTRING:
      {
        size_t l=0;
        const char *s = luaL_checklstring(L, i, &l);
        AS3_DeclareVar(strvar, String);
        AS3_CopyCStringToVar(strvar, s, l);
        inline_as3("args.push(strvar);\n");
        break;
      }
      default:
        inline_as3("trace(\"unknown: \" + %0 + \",\" + %1+ \",\" + %2);\n" :  : "r"(i), "r"(top), "r"(lua_type(L, i)));
        return 0;
    }
    i++;
  }
  // Flush all args off the stack
  lua_pop(L, top);

  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  inline_as3("__lua_objrefs[%0] = __lua_objrefs[%1].apply(%2, args);\n"
    : : "r"(result), "r"(funcobj), "r"(thisobj)
  );
  return 1;
}

static int flash_call (lua_State *L) {
  int top = lua_gettop(L);

  FlashObj *thisobj = getObjRef(L, 1);

  size_t l;
  const char *s1 = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(funcname, String);
  AS3_CopyCStringToVar(funcname, s1, l);

  inline_as3("var args:Array = [];\n");

  int i = 3;
  while(i <= top) {
    switch(lua_type(L, i)) {
      case LUA_TBOOLEAN: inline_as3("args.push(%0);\n" : : "r"(lua_toboolean(L, i))); break;
      case LUA_TNUMBER: inline_as3("args.push(%0);\n" : : "r"(luaL_checknumber(L, i))); break;
      case LUA_TFUNCTION:
      {
        lua_settop(L, top+1);
        lua_copy(L, i, top+1);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_settop(L, top);
        AS3_DeclareVar(luastate, int);
        AS3_CopyScalarToVar(luastate, L);
        inline_as3(
        "args.push(function(...vaargs):void"
        "{"
        "  Lua.lua_rawgeti(luastate, %1, %0);"
        "  for(var i:int = 0; i<vaargs.length;i++) {"
        "    var udptr:int = Lua.push_flashref(luastate);"
        "    __lua_objrefs[udptr] = vaargs[i];"
        "  };"
        "  Lua.lua_callk(luastate, vaargs.length, 0, 0, null);"
        "});" : : "r"(ref), "r"(LUA_REGISTRYINDEX));
        break;
      }
      case LUA_TUSERDATA: inline_as3("args.push(__lua_objrefs[%0]);\n" : : "r"(getObjRef(L, i))); break;
      case LUA_TSTRING:
      {
        const char *s = luaL_checklstring(L, i, &l);
        AS3_DeclareVar(strvar, String);
        AS3_CopyCStringToVar(strvar, s, l);
        inline_as3("args.push(strvar);\n");
        break;
      }
      default:
        inline_as3("trace(\"unknown: \" + %0 + \",\" + %1+ \",\" + %2);\n" :  : "r"(i), "r"(top), "r"(lua_type(L, i)));
        return 0;
    }
    i++;
  }
  // Flush all args off the stack
  lua_pop(L, top);

  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  inline_as3(
    "switch(args.length) { \n"
    "  case 0: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](); break;\n"
    "  case 1: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0]); break;\n"
    "  case 2: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0], args[1]); break;\n"
    "  case 3: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0], args[1], args[2]); break;\n"
    "  case 4: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0], args[1], args[2], args[3]); break;\n"
    "  case 5: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0], args[1], args[2], args[3], args[4]); break;\n" 
    "  case 6: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0], args[1], args[2], args[3], args[4], args[5]); break;\n"
    "  case 7: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0], args[1], args[2], args[3], args[4], args[5], args[6]); break;\n"
    "  case 8: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]); break;\n"
    "  case 9: __lua_objrefs[%0] = __lua_objrefs[%1][funcname](args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]); break;\n"
    "};\n"
    : : "r"(result), "r"(thisobj)
  );
  return 1;
}

static int flash_callstatic (lua_State *L) {
  int top = lua_gettop(L);

  size_t l;
  const char *s1 = luaL_checklstring(L, 1, &l);
  AS3_DeclareVar(classname, String);
  AS3_CopyCStringToVar(classname, s1, l);
  const char *s2 = luaL_checklstring(L, 2, &l);
  AS3_DeclareVar(staticname, String);
  AS3_CopyCStringToVar(staticname, s2, l);

  inline_as3("import flash.utils.getDefinitionByName;\n");
  inline_as3("var clz:Class = getDefinitionByName(classname);\n");
  inline_as3("var args:Array = [];\n");

  int i = 3;
  while(i <= top) {
    switch(lua_type(L, i)) {
      case LUA_TBOOLEAN: inline_as3("args.push(%0);\n" : : "r"(lua_toboolean(L, i))); break;
      case LUA_TNUMBER: inline_as3("args.push(%0);\n" : : "r"(luaL_checknumber(L, i))); break;
      case LUA_TFUNCTION:
      {
        lua_settop(L, top+1);
        lua_copy(L, i, top+1);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_settop(L, top);
        AS3_DeclareVar(luastate, int);
        AS3_CopyScalarToVar(luastate, L);
        inline_as3(
        "args.push(function(...vaargs):void"
        "{"
        "  Lua.lua_rawgeti(luastate,%1, %0);"
        "  for(var i:int = 0; i<vaargs.length;i++) {"
        "    var udptr:int = Lua.push_flashref(luastate);"
        "    __lua_objrefs[udptr] = vaargs[i];"
        "  };"
        "  Lua.lua_callk(luastate, vaargs.length, 0, 0, null);"
        "});" : : "r"(ref), "r"(LUA_REGISTRYINDEX));
        break;
      }
      case LUA_TUSERDATA: inline_as3("args.push(__lua_objrefs[%0]);\n" : : "r"(luaL_checkudata(L, i, FlashObjectType))); break;
      case LUA_TSTRING:
      {
        const char *s = luaL_checklstring(L, i, &l);
        AS3_DeclareVar(strvar, String);
        AS3_CopyCStringToVar(strvar, s, l);
        inline_as3("args.push(strvar);\n");
        break;
      }
      default:
        inline_as3("trace(\"unknown: \" + %0 + \",\" + %1+ \",\" + %2);\n" :  : "r"(i), "r"(top), "r"(lua_type(L, i)));
        return 0;
    }
    i++;
  }
  
  // Flush all args off the stack
  lua_pop(L, top);

  // Push the new userdata onto the stack
  FlashObj *result = push_newflashref(L);

  inline_as3("__lua_objrefs[%0] = clz[staticname].apply(null, args);\n" : : "r"(result));
  return 1;
}


// ===============================================================
//                          Registration
// ===============================================================

static const luaL_Reg flashlib[] = {
  {"trace", flash_trace},
  {"gettimer", flash_gettimer},

  {"newcallback", flash_newcallback},

  {"getprop", flash_getprop},
  {"getint", flash_getint},
  {"getuint", flash_getuint},
  {"getnumber", flash_getnumber},
  {"getstring", flash_getstring},

  {"setprop", flash_setprop},
  {"setint", flash_setint},
  {"setuint", flash_setuint},
  {"setnumber", flash_setnumber},
  {"setstring", flash_setstring},

  {"getx", flash_getx},
  {"gety", flash_gety},
  {"setx", flash_setx},
  {"sety", flash_sety},

  {"asnumber", flash_asnumber},
  {"asint", flash_asint},
  {"asuint", flash_asuint},
  {"asstring", flash_asstring},

  {"newintvec", flash_newintvec},
  {"newuintvec", flash_newuintvec},
  {"newnumbervec", flash_newnumbervec},

  {"setidxint", flash_setidxint},
  {"setidxuint", flash_setidxuint},
  {"setidxnumber", flash_setidxnumber},
  {"getidxint", flash_getidxint},
  {"getidxuint", flash_getidxuint},
  {"getidxnumber", flash_getidxnumber},

  {"new", flash_new},
  {"call", flash_call},
  {"callstatic", flash_callstatic},

  {NULL, NULL}
};

LUAMOD_API int luaopen_flash (lua_State *L) {
  luaL_newlib(L, flashlib);

  luaL_newmetatable(L, "flash");
  luaL_setfuncs(L, FlashObj_meta, 0);
  //lua_pushliteral(L, "__index");
  //lua_pushvalue(L, -3);
  //lua_rawset(L, -3);
  //lua_pushliteral(L, "__metatable");
  //lua_pushvalue(L, -3);
  //lua_rawset(L, -3);
  lua_pop(L, 1);

  return 1;
}
