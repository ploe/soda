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
bool LuaTableImportInt(const char *key, void *dest) {
	enum {
		TOP = -1
	};

	int *integer =  (int *) dest;

	bool set = false;
	lua_getfield(L, TOP, key);
	if (lua_isnumber(L, TOP)) {
		(*integer) = lua_tointeger(L, TOP);
		set = true;
	}

	lua_pop(L, 1);

	return set;
}

bool LuaTableImportText(const char *key, void *dest) {
	enum {
		TOP = -1
	};

	bool set = false;
	Text *text = (Text *) dest;

	lua_getfield(L, TOP, key);
	if (lua_isstring(L, TOP)) {
		const char *str = lua_tostring(L, TOP);
		(*text) = TextNew(str);
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


bool LuaImportWithStack(const char *file, int nargs, int nresults) {
	if (luaL_loadfile(L, file) || lua_pcall(L, nargs, nresults, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	return true;
}

lua_State *LuaGet() {
	return L;
}

void LuaFetchNestedTableByIndex(lua_State *L, int index, int n) {
	if (!lua_istable(L, index)) Panic(-1, "index is not a table");

	enum { 
		TOP = -1 
	};

	// get the nested table
	lua_rawgeti(L, index, n);
	if (!lua_istable(L, TOP)) {
		// throw the old value away
		lua_pop(L, 1);

		// create the new one and link it to our parent table
		lua_newtable(L);
		if (index < 0) index--;
		lua_rawseti(L, index, n);

		// then we get the reference to the table we just set
		if (index < 0) index++;
		lua_rawgeti(L, index, n);
	}
}

void LuaFetchNestedTableByKey(lua_State *L, int index, const char *key) {
	if (!lua_istable(L, index)) Panic(-1, "index is not a table");

	enum {
		TOP = -1,
	};

	// get the table by key
	lua_pushstring(L, key);
	if (index < 0) index--;
	lua_rawget(L, index);

	if (!lua_istable(L, TOP)) {
		// throw away the old value
		lua_pop(L, 1);

		// create new table and link it in the parent table with key
		lua_pushstring(L, key);
		lua_newtable(L);
		if (index < 0) index--;
		lua_rawset(L, index);

		// then get the reference to the new table
		lua_pushstring(L, key);
		if (index < 0) index++;
		lua_rawget(L, index);
	}
}

bool LuaDestroy() {
	if (L) {
		lua_close(L);
		L = NULL;
		return true;
	}

	return false;
}
