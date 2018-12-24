#include <SDL.h>

#include "Crew.h"

CrewStatus EventUpdate(Crew *c) {
	SDL_Event e;

    	while (SDL_PollEvent(&e)) {
        	if (e.type == SDL_QUIT) return EXIT;
	}

	return LIVE;
}

CrewStatus EventType(Crew *c) {
	c->update = EventUpdate;

	return LIVE;
}


