compile = cc -g -O2 -I./include

SDL_FLAGS := `pkg-config sdl2 SDL2_image --cflags --libs`
SDL_CFLAGS := `pkg-config sdl2 SDL2_image --cflags`

GL_FLAGS := -framework OpenGL `pkg-config glew ILU --cflags --libs` 
GL_CFLAGS := `pkg-config glew ILU --cflags`

LUA_FLAGS := `pkg-config lua5.3 --cflags --libs`
LUA_CFLAGS := `pkg-config lua5.3 --cflags`

ALL_CFLAGS := $(SDL_CFLAGS) $(LUA_CFLAGS) $(GL_CFLAGS)

app: ./src/main.c ./src/Actors.c ./src/Crew.c ./src/Event.c ./src/LuaState.c ./src/Joystick.c ./src/Panic.c ./src/Text.c Window.o
	$(compile) $^ -o $@ $(SDL_FLAGS) $(LUA_FLAGS) $(GL_FLAGS)

Window.o: ./src/Window/Common.c ./src/Window/OpenGL.c ./src/Window/SDL.c
	$(compile) -c $^ $(ALL_CFLAGS)
	ld -r -o Window.o Common.o OpenGL.o SDL.o
clean:
	rm -v *.o
