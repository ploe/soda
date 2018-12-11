#include <stdio.h>

#include <lua.h>

#include <SDL.h>
#include <SDL_image.h>

#include "Actors.h"
#include "Crew.h"
#include "LuaState.h"
#include "Window.h"

int main(int argc, char *argv[]) {
	LuaInit();

	CrewNew(WindowType);
	CrewNew(ActorsType);
	
	LuaImport("bootstrap");
	while (CrewRoll()) continue;

	LuaDestroy();

	return 0;
}
