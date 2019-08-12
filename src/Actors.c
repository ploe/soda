#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include <lua.h>
#include <lauxlib.h>

#include "Crew.h"
#include "LuaState.h"
#include "Panic.h"
#include "Text.h"
#include "Window.h"

static SDL_Texture *TextureNew(char *path) {
	SDL_Texture *t = NULL;
	SDL_Surface *surface = IMG_Load(path);
	
	if (surface) {
		SDL_Renderer *renderer = WindowGetRenderer();

		t = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	}

	return t;
}

typedef struct ActorSDLMembers {
	SDL_Texture *texture;
	SDL_Rect src, dest;
	double angle;
	SDL_RendererFlip flip;
	Text filename;
	Uint8 alpha;
} ActorSDLMembers;

typedef struct ActorLuaMembers {
	int table;
} ActorLuaMembers;

typedef struct Actor {
	struct Actor *next;
	bool render;
	ActorSDLMembers sdl;
	ActorLuaMembers lua;
} Actor;

Actor *top = NULL;

static Actor *ActorNew() {
	Actor *a = calloc(1, sizeof(Actor));
	if (a) {
		a->next = top;
		top = a;
	}

	return a;
}

static bool ActorSetTexture(Actor *a, Text filename) {
	Text current = a->sdl.filename;
	if (current && (strcmp(current, filename) != 0))
		TextFree(current);
	
	a->sdl.filename = filename;

	SDL_Texture *texture = a->sdl.texture;
	if (texture) SDL_DestroyTexture(texture);

	a->sdl.texture = texture = TextureNew(filename);

	return (texture != NULL);
}

static int ActorLuaNew(lua_State *L) {
	enum {
		TABLE = 1
	};

	if (!lua_istable(L, TABLE))
		return luaL_error(L, "Actor.new: Expected table");

	Actor *a = ActorNew();
	a->lua.table = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, LUA_REGISTRYINDEX, a->lua.table);

	Text filename = NULL;
	a->render = LuaTableImportTexts(
		"texture", &(filename), 
		NULL, NULL
	);

	if (filename) ActorSetTexture(a, filename);

	return 1;
}

static Actor *ActorDestroy(Actor *a) {
	Actor **current = &top, *next;
	while ((*current != a) && (*current != NULL)) {
		current = &(*current)->next;
	}

	if (*current) {
		*current = a->next;
		SDL_DestroyTexture(a->sdl.texture);
		free(a);
	}

	return NULL;
}

static Actor *ActorUpdateSDLMembers(Actor *a) {
	ActorSDLMembers *sdl = &(a->sdl);

	// push the Actor's Lua table to the top of the stack
	lua_State *L = LuaGet();
	lua_rawgeti(L, LUA_REGISTRYINDEX, a->lua.table);

	// maps the Lua keys on to their respective C values, if the
	// map fails the Actor won't render.
	a->render = LuaTableImportInts(
		"x", &(sdl->dest.x),
		"y", &(sdl->dest.y), 
		"w", &(sdl->dest.w), 
		"h", &(sdl->dest.h),
		"alpha",  &(sdl->alpha),
		NULL, NULL
	);

	// remove the table
	lua_pop(L, 1);

	return a;
}

static Actor *ActorLuaMethod(Actor *a, const char *method) {
	enum {
		TOP = -1,
		ACTOR = -2
	};

	lua_State *L = LuaGet();
	lua_rawgeti(L, LUA_REGISTRYINDEX, a->lua.table);

	lua_getfield(L, TOP, method);
	if (lua_isfunction(L, TOP)) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, a->lua.table);

		if (lua_pcall(L, 1, 1, 0)) Panic(-1, lua_tostring(L, TOP));

		lua_setfield(L, ACTOR, method);
	}

	lua_pop(L, 1);

	return a;
}

static Actor *ActorUpdate(Actor *a) {
	return ActorLuaMethod(a, "update");
}

static Actor *ActorRender(Actor *a) {
	SDL_Renderer *renderer = WindowGetRenderer();
	if (a->render) {
		ActorSDLMembers *sdl = &(a->sdl);
		SDL_SetTextureBlendMode(sdl->texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(sdl->texture, sdl->alpha);
		SDL_RenderCopyEx(renderer, sdl->texture, NULL, &(sdl->dest), sdl->angle, NULL, 0);
	}

	return a;
}

typedef Actor *(*ActorMethod)(Actor *);

static void ActorsForEach(ActorMethod method) {
	Actor *a, *next = NULL;
	for (a = top; a != NULL; a = next) {
		next = a->next;
		method(a);
	}
}

static CrewStatus ActorsUpdate(Crew *c) {
	ActorMethod methods[] = {
		ActorUpdate,
		ActorUpdateSDLMembers,
		ActorRender,
		NULL
	};

	ActorMethod *m;
	for (m = methods; *m != NULL; m++) {
		ActorsForEach(*m);
	}

	return LIVE;
}

static CrewStatus ActorsDestroy() {
	ActorsForEach(ActorDestroy);

	return EXIT;
}

CrewStatus ActorsInit(Crew *c) {
	c->tag = "actors";
	c->update = ActorsUpdate;
	c->destroy = ActorsDestroy;

	LuaBind("Actor",
		"new", ActorLuaNew,
		NULL, NULL
	);

	SDL_Renderer *renderer = WindowGetRenderer();

	return LIVE;
}
