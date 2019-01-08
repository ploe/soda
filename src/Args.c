#include "Panic.h"
#define ARGS_FIRST_CHAR(i) (argv[i][0])
#define ARGS_FLAGS_START(i) &(argv[i][1])
#define REQUIRES_VALUE(arg) (arg[1] == ':')

char *ArgsAccept(char f, char *accepted) {
	while (*accepted != '\0') {
		if (f == *accepted) return accepted;
		accepted++;
	}

	return NULL;
}

bool ArgsLuaInit() {
	enum {
		TOP = -1,		
	};

	LuaImportWithStack("ArgsParse.lua", 0, 1);

	lua_State *L = LuaGet();
	if (!lua_istable(L, TOP)) {
		lua_pop(L, 1);

		return false;
	}

	lua_setglobal(L, "Args");
	return true;

}

bool ArgsLuaCall(char *method, char *opt) {
	enum {
		TOP = -1,		
	};

	lua_State *L = LuaGet();

	bool success = false;
	lua_getglobal(L, "Args");
	if (lua_istable(L, TOP)) {
		lua_getfield(L, TOP, method);

		lua_getglobal(L, "Args");
		lua_pushstring(L, opt);

		lua_pcall(L, 2, 1, 0);

		if (lua_isboolean(L, TOP) && lua_toboolean(L, TOP)) success = true;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	return success;
}

void ArgsParse(char *accepted, int argc, char *argv[]) {
	int i = 1, opt = 0;
	for (i = 1; i < argc; i++) {
		if (ARGS_FIRST_CHAR(i) != '-') continue;

		if (opt <= i) {
			opt = i + 1;
			while(ARGS_FIRST_CHAR(opt) == '-') opt++;
			printf("opt => %s\n", argv[opt]);
			if (opt >= argc) Panic(-1, "out of args");
		}

		char *f;
		for (f = ARGS_FLAGS_START(i); *f != '\0'; f++) {
			char *arg;
			if ( (arg = ArgsAccept(*f, accepted)) ) {
				printf("flag -%c input\n", *f);
				if (REQUIRES_VALUE(arg)) {
					if (opt >= argc) Panic(-1, "No opt");

					puts(argv[opt]);
					opt++;
				}
			}
		}
	}
}


