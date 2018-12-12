#include <stdbool.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "LuaState.h"
#include "Panic.h"
#include "Text.h"

enum {
	ELUA = -1,
};

static lua_State *L = NULL;

lua_State *LuaInit() {
	L = luaL_newstate();
	if (!L) Panic(ELUA, "LuaInit failed.");

	luaL_openlibs(L);

	return L;
}

bool LuaBind(const char *table, const char *key, lua_CFunction func, ...) {
	enum {
		TABLE_GET = -1,
		TABLE_SET = -2,
		NIL = 1,
	};

	va_list vl;
	va_start(vl, func);
	while (key && func) {
		lua_getglobal(L, table);
		if (lua_isnil(L, TABLE_GET)) {
			lua_pop(L, NIL);
			lua_newtable(L);
		}

		if (lua_istable(L, TABLE_GET)) {
			lua_pushcfunction(L, func);
			lua_setfield(L, TABLE_SET, key);
			lua_setglobal(L, table);
		}
		else Panic(ELUA, "namespace is already set in Lua state.");

		key = (const char *) va_arg(vl, const char *);
		func = va_arg(vl, void *);
	}
	va_end(vl);

	return true;
}

/*	Lua Method that expects a table to be at the top of the 
	stack. If it fails to set them all it returns false.	*/
bool LuaKeysToInts(const char *key,  int *integer, ...) {
	enum {
		TOP = -1
	};

	// ensure TOP is a table, otherwise we fail
	if (!lua_istable(L, TOP)) return false;

	// args are given as a list of key/dest pairs
	va_list vl;
        va_start(vl, integer);
        while (key && integer) {
		lua_getfield(L, TOP, key);

		// if the value is a number...
		if (lua_isnumber(L, TOP)) {
			// ...set integer to it
			(*integer) = lua_tointeger(L, TOP);
			lua_pop(L, 1);
		}
		else {
			// ...otherwise we just bin it, and fail the 
			// whole lot
			lua_pop(L, 1);
			return false;
		}

		key = (const char *) va_arg(vl, const char *);
		integer = va_arg(vl, int *);
	}
	va_end(vl);

	return true;
}

bool LuaImport(const char *file) {
	if (luaL_loadfile(L, file) || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	return true;
}

lua_State *LuaGet() {
	return L;
}

bool LuaDestroy() {
	if (L) {
		lua_close(L);
		L = NULL;
		return true;
	}

	return false;
}
