#ifndef SODA_WINDOW_H
#define SODA_WINDOW_H

#include <SDL.h>

SDL_Renderer *WindowGetRenderer();

CrewStatus WindowInit(Crew *c);
CrewStatus WindowGLInit(Crew *c);

#endif
