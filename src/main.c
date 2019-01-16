#include <stdio.h>
#include <stdbool.h>

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

	CrewInit(
		WindowInit,
		ActorsInit,
		JoystickInit,
		EventInit,
		NULL
	);

	LuaImport("bootstrap.lua");
	while (CrewRoll()) continue;

	CrewPurge();
	LuaDestroy();

	return 0;
}
