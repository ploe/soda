#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

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

int main(int argc, char *argv[]) {
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	if (SDL_Init(SDL_INIT_VIDEO)) {
		puts("failed to open window");
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
	SDL_Texture *hogberg = TextureNew("./png/george-goblin.png", renderer);
	SDL_SetRenderDrawColor( renderer, 0x00, 0xFF, 0x00, 0xFF );
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	SDL_SetWindowFullscreen(window, 0);

	int live = 1;

	SDL_Event e;
	double angle = 0;
	while (live) {
		SDL_RenderClear( renderer );
		angle += 0.1;
		SDL_RenderCopyEx( renderer, hogberg, NULL, NULL, angle, NULL, 0);
		SDL_RenderPresent( renderer );

		SDL_Event event;
    		while (SDL_PollEvent(&event)) {
        		if (event.type == SDL_QUIT) {
				live = 0;
			}
    		}

	}

	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
