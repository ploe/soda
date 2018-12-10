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

	bool bootstrap = LuaImport("bootstrap");
	if (!bootstrap) {
		lua_close(L);
		Panic(ELUA, "cannot find bootstrap script: No such file or directory");
	}

	return L;
}

bool LuaBind(const char *key, lua_CFunction func, ...) {
	enum {
		TABLE_GET = -1,
		TABLE_SET = -2,
		NIL = 1,
	};

	va_list vl;
	va_start(vl, func);
	while (key && func) {
		lua_getglobal(L, "tr");
		if (lua_isnil(L, TABLE_GET)) {
			lua_pop(L, NIL);
			lua_newtable(L);
		}

		if (lua_istable(L, TABLE_GET)) {
			lua_pushcfunction(L, func);
			lua_setfield(L, TABLE_SET, key);
			lua_setglobal(L, "tr");
		}
		else Panic(ELUA, "namespace 'tr' is already set in Lua state.");

		key = (const char *) va_arg(vl, const char *);
		func = va_arg(vl, void *);
	}
	va_end(vl);


	return true;
}

bool LuaImport(const char *slug) {
	const char *extensions[] = {
		".luac",
		".lua",
		"",
		NULL,
	};

	const char **ext = NULL;
	for (ext = extensions; *ext != NULL; ext++) {
		Text file = TextNew("./%s%s", slug, *ext);
		if (luaL_loadfile(L, file) || lua_pcall(L, 0, 0, 0)) {
			file = TextFree(file);
			fprintf(stderr, "%s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
			continue;
		}
		else {
			file = TextFree(file);
			break;
		}
	}

	return (*ext != NULL) ? true : false;
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
