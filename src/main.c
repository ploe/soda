#include <stdio.h>

#include <lua.h>

#include <SDL.h>
#include <SDL_image.h>

#include "Actors.h"
#include "Crew.h"
#include "Event.h"
#include "Joystick.h"
#include "LuaState.h"
#include "Window.h"

int main(int argc, char *argv[]) {
	LuaInit();

	CrewMethod *c, crew[] = { 
		WindowType,
		EventType,
		JoystickType, 
		ActorsType, 
		NULL 
	};

	for (c = crew; *c != NULL; c++) {
		CrewNew(*c);
	}

	LuaImport("bootstrap.lua");
	while (CrewRoll()) continue;

	LuaDestroy();

	return 0;
}
