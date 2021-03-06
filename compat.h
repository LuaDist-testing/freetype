#include <lua.h>
#include <lauxlib.h>

/****************************************************************************/

#ifndef LUAMOD_API
#	ifdef WIN32
#		define LUAMOD_API __declspec(dllexport)
#	else
#		define LUAMOD_API __attribute__((visibility("default")))
#	endif
#endif

#if LUA_VERSION_NUM==502

int typeerror(lua_State* L, int narg, const char* tname);
#define setfuncs luaL_setfuncs
#define setuservalue lua_setuservalue
#define getuservalue lua_getuservalue
#define rawlen lua_rawlen
#define objlen lua_rawlen /* :FIXME: use lua_len */
#define equal(L, index1, index2) lua_compare((L), (index1), (index2), LUA_OPEQ)

#elif LUA_VERSION_NUM==501

#define typeerror luaL_typerror
void setfuncs(lua_State* L, const luaL_Reg* l, int nup);
#define setuservalue lua_setfenv
#define getuservalue lua_getfenv
#define rawlen lua_objlen
#define objlen lua_objlen
#define equal lua_equal

#endif

