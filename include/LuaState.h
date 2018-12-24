#ifndef SODA_LUA_H
#define SODA_LUA_H

#include <stdbool.h>

#include <lua.h>

typedef bool (*LuaTableImporter)(const char *key, int *dest);
bool LuaTableImportInt(const char *key, int *dest);
bool LuaTableImport(LuaTableImporter import, void *key, void *dest, ...);

#define LuaTableImportInts(key, dest, ...) LuaTableImport(LuaTableImportInt, key, dest, __VA_ARGS__)


lua_State *LuaInit();
bool LuaImport(const char *file);
void LuaFetchNestedTableByIndex(lua_State *L, int index, int n);
void LuaFetchNestedTableByKey(lua_State *L, int index, const char *key);
lua_State *LuaGet();
bool LuaBind(const char *table, const char *key, lua_CFunction func, ...);
bool LuaKeysToInts(const char *key,  int *integer, ...);
bool LuaDestroy();

#endif
