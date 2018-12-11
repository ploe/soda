#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include <lua.h>

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

typedef struct Actor {
	struct Actor *next;
	char *tag;
	SDL_Texture *texture;
	SDL_Rect src, dest;
	double angle;
	SDL_RendererFlip flip;
	bool render;
	Uint8 alpha;
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
	lua_pushboolean(L, true);
	return 1;
}

static Actor *ActorDestroy(Actor *a) {
	Actor **current = &top, *next;
	while ((*current != a) && (*current != NULL)) {
		current = &(*current)->next;
	}

	*current = a->next;
	SDL_DestroyTexture(a->texture);
	free(a);

	return NULL;
}

static Actor *ActorRender(Actor *a) {
	SDL_Renderer *renderer = WindowGetRenderer();
	if (a->render) {
		SDL_SetTextureBlendMode(a->texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(a->texture, a->alpha);
		SDL_RenderCopyEx(renderer, a->texture, NULL, &(a->dest), a->angle, NULL, 0);
	}

	return a;
}

typedef Actor *(*ActorMethod)(Actor *);

static void ActorsForEach(ActorMethod method) {
	Actor *a;
	for (a = top; a != NULL; a = a->next) method(a);
}

static CrewStatus ActorsUpdate(Crew *c) {
	ActorsForEach(ActorRender);

	return LIVE;
}

static CrewStatus ActorsDestroy() {
	Actor *a;
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

	Actor *a = ActorNew();

	a = ActorNew();
	a->texture = TextureNew("./png/george-goblin.png", renderer);
	a->dest = (SDL_Rect) {0, 0, 32, 32};
	a->render = true;
	a->alpha = 255;

	a= ActorNew();
	a->texture = TextureNew("./png/george-goblin.png", renderer);
	a->dest = (SDL_Rect) {64, 64, 32, 32};
	a->render = true;
	a->alpha = 128;

	a = ActorNew();
	ActorNew();
	a = ActorNew();

	ActorDestroy(a);

	return LIVE;
}
