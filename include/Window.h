#ifndef SODA_WINDOW_H
#define SODA_WINDOW_H

#include <SDL.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_FPS 60
#define WINDOW_TICKS (1000 / WINDOW_FPS)

extern SDL_Window *window;

SDL_Renderer *WindowGetRenderer();

CrewStatus WindowInit(Crew *c);
CrewStatus WindowGLInit(Crew *c);

#endif
