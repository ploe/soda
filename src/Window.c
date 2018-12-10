#include <SDL.h>
#include <SDL_image.h>

#include "Crew.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

SDL_Texture *TextureNew(char *path, SDL_Renderer *renderer) {
	SDL_Texture *t = NULL;
	SDL_Surface *surface = IMG_Load(path);
	t = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return t;
}

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *hogberg = NULL;

/*	Method that renders the Window and checks to see if the 
	player has clicked the QUIT button. If so, we tear down the 
	app.	*/
static double angle;
static CrewStatus WindowUpdate(Crew *c) {
	SDL_RenderClear( renderer );
	angle += 0.1;
	SDL_RenderCopyEx( renderer, hogberg, NULL, NULL, angle, NULL, 0);
	SDL_RenderPresent( renderer );

	SDL_Event e;
    	while (SDL_PollEvent(&e)) {
        	if (e.type == SDL_QUIT) return EXIT;
	}

	return LIVE;
}

/*	Method that deallocates the SDL_Renderer and 
	SDL_Window.	*/
static CrewStatus WindowDestroy(Crew *c) {
	SDL_DestroyTexture(hogberg);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return CUT;
}

/*	Method that initialises the Window Crew member.	*/
CrewStatus WindowType(Crew *c) {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		puts("failed to open window");
		return EXIT;
	}

        window = SDL_CreateWindow(
		"hi helo",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT, 
		SDL_WINDOW_SHOWN
	);

	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	hogberg = TextureNew("./png/george-goblin.png", renderer);
	SDL_SetRenderDrawColor( renderer, 0x00, 0xFF, 0x00, 0xFF );
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	SDL_SetWindowFullscreen(window, 0);

	c->tag = "Window";
	c->update = WindowUpdate;
	c->destroy = WindowDestroy;

	return LIVE;
}
