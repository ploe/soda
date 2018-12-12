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

/*	LuaTableImporter that imports a single integer from the Lua 
	table that's at the top of the stack to an integer in C. */
bool LuaTableImportInt(const char *key, int *dest) {
	enum {
		TOP = -1
	};

	bool set = false;

	lua_getfield(L, TOP, key);
	if (lua_isnumber(L, TOP)) {
		(*dest) = lua_tointeger(L, TOP);
		set = true;
	}
	lua_pop(L, 1);

	return set;
}

/*	Method that takes a LuaTableImporter and list of keys and 
	dests, it also expects a Lua table to be on top of the stack.

	It iterates over the key/dest pairs and runs the 
	LuaTableImporter on each. If it fails to set any of them, it
	returns false.	*/
bool LuaTableImport(LuaTableImporter import, void *key, void *dest, ...) {
	enum {
		TOP = -1
	};

	if (!lua_istable(L, TOP)) return false;

	bool set = false;

	va_list vl;
        va_start(vl, dest);
        while (key && dest) {
		set = import(key, dest);
		if (!set) break;

		key = va_arg(vl, void *);
		dest = va_arg(vl, void *);
	}
	va_end(vl);

	return set;
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
