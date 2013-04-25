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
#ifdef SWIG

%apply long{unsigned long};
%apply int{unsigned int};

%{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "AS3/AS3.h"

int main() { AS3_GoAsync(); }

#undef lua_pop
void lua_pop(lua_State *L, int n) { lua_settop(L, -(n)-1); };

int push_flashref(lua_State *L)
{
  // Push the new userdata onto the stack
  int result = (int)lua_newuserdata(L, 4);
  luaL_getmetatable(L, "flash");
  lua_setmetatable(L, -2);
  return result;
}

void* get_cfunction(lua_State *L, int n)
{
	inline_as3(
		"trace(\"getcfunc: \" + %0);"
		::"r"(lua_tocfunction(L, n))
	);
	return (void*)lua_tocfunction(L, n);
}

void push_cfunction(lua_State *L, void* fn)
{
	lua_pushcfunction(L, (lua_CFunction)fn);
}


%}
#endif

#define SWIGPP
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#undef lua_pop
LUA_API void lua_pop(lua_State *L, int n);

LUA_API int push_flashref(lua_State *L);

LUA_API void* get_cfunction(lua_State *L, int n);
LUA_API void push_cfunction(lua_State *L, void* fn);
