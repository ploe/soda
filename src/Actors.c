#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include <lua.h>
#include <lauxlib.h>

#include "Crew.h"
#include "LuaState.h"
#include "Window.h"

static SDL_Texture *TextureNew(char *path, SDL_Renderer *renderer) {
	SDL_Texture *t = NULL;
	SDL_Surface *surface = IMG_Load(path);
	t = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return t;
}

typedef struct ActorSDLMembers {
	SDL_Texture *texture;
	SDL_Rect src, dest;
	double angle;
	SDL_RendererFlip flip;
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

static int ActorLuaNew(lua_State *L) {
	enum {
		TABLE = 1
	};

	if (lua_istable(L, TABLE)) {
		Actor *a = ActorNew();
		a->lua.table = luaL_ref(L, LUA_REGISTRYINDEX);
		
		SDL_Renderer *renderer = WindowGetRenderer();
		a->sdl.texture = TextureNew("./png/george-goblin.png", renderer);

		lua_pushboolean(L, true);
	}
	else lua_pushboolean(L, false);

	return 1;
}

static Actor *ActorDestroy(Actor *a) {
	Actor **current = &top, *next;
	while ((*current != a) && (*current != NULL)) {
		current = &(*current)->next;
	}

	*current = a->next;
	SDL_DestroyTexture(a->sdl.texture);
	free(a);

	return NULL;
}

static Actor *ActorUpdateSDLMembers(Actor *a) {
	ActorSDLMembers *sdl = &(a->sdl);

	// push the Actor's Lua table to the top of the stack
	lua_State *L = LuaGet();
	lua_rawgeti(L, LUA_REGISTRYINDEX, a->lua.table);

	// maps the Lua keys on to their respective C values, if the
	// map fails the Actor won't render.
	a->render = LuaKeysToInts(
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
	Actor *a;
	for (a = top; a != NULL; a = a->next) method(a);
}

static CrewStatus ActorsUpdate(Crew *c) {
	ActorsForEach(ActorUpdateSDLMembers);
	ActorsForEach(ActorRender);

	return LIVE;
}

static CrewStatus ActorsDestroy() {
	ActorsForEach(ActorDestroy);

	return EXIT;
}

CrewStatus ActorsType(Crew *c) {
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
