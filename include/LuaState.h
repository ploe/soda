#ifndef TR_LUA
#define TR_LUA

#include <stdbool.h>

#include <lua.h>

lua_State *LuaInit();
bool LuaImport(const char *file);
lua_State *LuaGet();
bool LuaBind(const char *table, const char *key, lua_CFunction func, ...);
bool LuaKeysToInts(const char *key,  int *integer, ...);
bool LuaDestroy();

#endif
