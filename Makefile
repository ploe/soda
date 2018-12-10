compile = cc -g -O2 -I./include
SDL_FLAGS := `pkg-config sdl2 SDL2_image --cflags --libs`

app: ./src/main.c ./src/Crew.c
	$(compile) $(SDL_FLAGS) -o $@ $^
