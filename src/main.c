#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

#include "Actors.h"
#include "Crew.h"
#include "Window.h"

int main(int argc, char *argv[]) {
	CrewNew(WindowType);
	CrewNew(ActorsType);
	
	while (CrewRoll()) continue;

	return 0;
}
