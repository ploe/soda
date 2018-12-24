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

#define ARGS_FIRST_CHAR(i) (argv[i][0])
#define ARGS_FLAGS_START(i) &(argv[i][1])

void ArgsParse(char *accepted, int argc, char *argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		if (ARGS_FIRST_CHAR(i) != '-') continue;
		puts(argv[i]);

		char *f;
		for (f = ARGS_FLAGS_START(i); *f != '\0'; f++) {
			printf("flag %c input\n", *f);
		}
	}
}

int main(int argc, char *argv[]) {
	LuaInit();

	CrewInit(
		WindowInit,
		ActorsInit,
		JoystickInit,
		EventInit,
		NULL
	);

	ArgsParse("hi:elo", argc, argv);

	LuaImport("bootstrap.lua");
	while (CrewRoll()) continue;

	CrewPurge();
	LuaDestroy();

	return 0;
}
