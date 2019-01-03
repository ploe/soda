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
#define REQUIRES_VALUE(arg) (arg[1] == ':')

char *ArgsAccept(char f, char *accepted) {
	while (*accepted != '\0') {
		if (f == *accepted) return accepted;
		accepted++;
	}

	return NULL;
}

void ArgsParse(char *accepted, int argc, char *argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		if (ARGS_FIRST_CHAR(i) != '-') continue;
		puts(argv[i]);

		char *f;
		for (f = ARGS_FLAGS_START(i); *f != '\0'; f++) {
			char *arg;
			if ( (arg = ArgsAccept(*f, accepted)) ) {
				printf("flag -%c input\n", *f);
				if (REQUIRES_VALUE(arg)) {
					puts("this one requires a value...");
				}
			}
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
