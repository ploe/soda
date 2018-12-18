#include <lua.h>

#include <SDL.h>

#include "Crew.h"
#include "LuaState.h"
#include "Panic.h"

#define JOYSTICKS_LUA_MODULE "Joysticks"

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

void LuaType(lua_State* L, int index) {
	int tp = lua_type(L, index);
	puts(lua_typename(L, tp));
}

typedef struct Joystick {
	struct Joystick *next;
	char *guid;
	bool on;
	SDL_Joystick *joy;
} Joystick;

static Joystick *top = NULL;

Joystick *JoystickAttach() {
	Joystick *j;
	for (j = top; j != NULL; j = j->next) {
		
	}
}

/*	Method that assumes a Joystick table is pushed to the stack	*/
bool JoystickLuaGetButtons(lua_State *L, SDL_Joystick *joy) {
	enum {
		TOP = -1,
		BUTTONS = -2,
	};

	LuaFetchNestedTableByKey(L, TOP, "buttons");

	int button;
	for (button = 0; button < SDL_JoystickNumButtons(joy); button++) {
		Uint8 down = SDL_JoystickGetButton(joy, button);
		lua_pushboolean(L, down);
		lua_rawseti(L, BUTTONS, button + 1);
	}

	lua_pop(L, 1);

	return true;
}

CrewStatus JoystickUpdate(Crew *c) {
	enum {
		MODULE = -2,
		TOP = -1,
	};

	lua_State *L = LuaGet();

	// push Joysticks table to stack
	lua_getglobal(L, JOYSTICKS_LUA_MODULE);
	int i = 0;
	for (i = 0; i < SDL_NumJoysticks(); i++) {
		SDL_Joystick *joy = SDL_JoystickOpen(i);
		if (joy) {
			// lua indices start from 1
			int n = i + 1;
			LuaFetchNestedTableByIndex(L, TOP, n);
			JoystickLuaGetButtons(L, joy);
		}
	//	SDL_JoystickClose(joy);
	}
	lua_pop(L, 1);

	return LIVE;
}

CrewStatus JoystickType(Crew *c) {
	c->tag = "joystick";
	c->update = JoystickUpdate;

	lua_State *L = LuaGet();
	lua_newtable(L);
	lua_setglobal(L, JOYSTICKS_LUA_MODULE);

	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	return LIVE;
}
