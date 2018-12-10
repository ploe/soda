#include <SDL.h>
#include <SDL_image.h>

#include "Crew.h"
#include "Window.h"

SDL_Texture *hogberg = NULL;

static SDL_Texture *TextureNew(char *path, SDL_Renderer *renderer) {
	SDL_Texture *t = NULL;
	SDL_Surface *surface = IMG_Load(path);
	t = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return t;
}

typedef struct Actor {
	char *tag;
	SDL_Texture *texture;
//	ActorMethod type, update, destroy;
	struct Actor *next;
} Actor;

CrewStatus ActorsUpdate(Crew *c) {
	static double angle = 0;
	angle += 0.1;

	SDL_Renderer *renderer = WindowGetRenderer();
	SDL_RenderCopyEx(renderer, hogberg, NULL, NULL, angle, NULL, 0);
	return LIVE;
}

CrewStatus ActorsDestroy() {
	SDL_DestroyTexture(hogberg);

	return EXIT;
}

CrewStatus ActorsType(Crew *c) {
	c->tag = "actors";
	c->update = ActorsUpdate;
	c->destroy = ActorsDestroy;

	SDL_Renderer *renderer = WindowGetRenderer();
	hogberg = TextureNew("./png/george-goblin.png", renderer);
	return LIVE;
}

