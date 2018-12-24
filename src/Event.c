#include <SDL.h>

#include "Crew.h"

CrewStatus EventUpdate(Crew *c) {
	SDL_Event e;

    	while (SDL_PollEvent(&e)) {
		switch(e.type) {
        		case SDL_QUIT:
				return EXIT;
				break;
			case SDL_JOYDEVICEADDED:
				break;
			case SDL_JOYDEVICEREMOVE:
				break;
		}
	}

	return LIVE;
}



CrewStatus EventType(Crew *c) {
	c->update = EventUpdate;

	return LIVE;
}


