#include <lua.h>

#include <SDL.h>

#include "Crew.h"
#include "LuaState.h"
#include "Panic.h"

#define JOYSTICKS_LUA_MODULE "Joysticks"

void LuaInit(lua_State* L, int index) {
	int tp = lua_type(L, index);
	puts(lua_typename(L, tp));
}

typedef struct Joystick {
	struct Joystick *next;
	int index;
	bool on;
	SDL_Joystick *joy;
} Joystick;

static Joystick *top = NULL;

Joystick *JoystickAttach(int index) {
	Joystick *j;
	for (j = top; j != NULL; j = j->next) {
	}
	return j;
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
	SDL_Joystick *joy = SDL_JoystickOpen(i);
	for (i = 0; i < SDL_NumJoysticks(); i++) {
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

CrewStatus JoystickInit(Crew *c) {
	c->tag = "joystick";
	c->update = JoystickUpdate;

	lua_State *L = LuaGet();
	lua_newtable(L);
	lua_setglobal(L, JOYSTICKS_LUA_MODULE);

	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	return LIVE;
}
