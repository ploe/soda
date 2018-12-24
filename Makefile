compile = cc -g -O2 -I./include
SDL_FLAGS := `pkg-config sdl2 SDL2_image --cflags --libs`
LUA_FLAGS := `pkg-config lua5.3 --cflags --libs`

app: ./src/main.c ./src/Actors.c ./src/Crew.c ./src/Event.c ./src/LuaState.c ./src/Joystick.c ./src/Panic.c ./src/Text.c ./src/Window.c
	$(compile) $^ -o $@ $(SDL_FLAGS) $(LUA_FLAGS)
