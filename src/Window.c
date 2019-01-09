#include <SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "Crew.h"
#include "Text.h"
#include "Panic.h"

//#define WINDOW_WIDTH 1920
#define WINDOW_WIDTH 160
//#define WINDOW_HEIGHT 1080
#define WINDOW_HEIGHT 144

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_GLContext context;

const char *platform = NULL;

#define WINDOW_FPS 60
#define WINDOW_TICKS (1000 / WINDOW_FPS)

#include <IL/il.h>

/*	Method that renders the Window and checks to see if the 
	player has clicked the QUIT button. If so, we tear down the 
	app.	*/
static CrewStatus WindowUpdate(Crew *c) {
	static Uint32 start = 0;

	Uint32 elapsed = SDL_GetTicks() - start;
	if (elapsed < WINDOW_TICKS) SDL_Delay(WINDOW_TICKS-elapsed);

	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);

	start = SDL_GetTicks();

	return LIVE;
}

/*	Method that deallocates the SDL_Renderer and 
	SDL_Window.	*/
static CrewStatus WindowDestroy(Crew *c) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return CUT;
}

/*	Method that initialises the Window Crew member.	*/
CrewStatus WindowInit(Crew *c) {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		puts("failed to create window");
		return EXIT;
	}

	platform = SDL_GetPlatform();

        window = SDL_CreateWindow(
		"soda: SDL",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT, 
		SDL_WINDOW_SHOWN
	);

	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	SDL_SetRenderDrawColor( renderer, 0x00, 0xFF, 0x00, 0xFF );
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	SDL_SetWindowFullscreen(window, 0);

	c->tag = "window";
	c->update = WindowUpdate;
	c->destroy = WindowDestroy;

	return LIVE;
}

SDL_Renderer *WindowGetRenderer() {
	return renderer;
}

CrewStatus WindowGLUpdate(Crew *c) {
	glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	SDL_GL_SwapWindow(window);
	
	return LIVE;
}

CrewStatus SDL_WindowGLDestroy(Crew *c) {
	SDL_GL_DeleteContext(context);  
	return CUT;
}


GLuint ShaderLoad(char *path, GLenum type);

ILuint TextureILLoad(char *path) {
	ILuint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);

	if ( ilLoadImage(path) == IL_TRUE ) {
		if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) Panic(-1, "oh my!");
	}

	return id;
}

CrewStatus WindowGLInit(Crew *c) {
	c->update =  WindowGLUpdate;

	if (SDL_Init(SDL_INIT_VIDEO)) {
		puts("failed to create window");
		return EXIT;
	}

	ilInit();

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        window = SDL_CreateWindow(
		"soda: OpenGL",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT, 
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
	);

	context = SDL_GL_CreateContext(window);
	if (!context) Panic(-1, "EGL");

	glewExperimental = GL_TRUE;
	glewInit();

	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	SDL_SetWindowFullscreen(window, 0);

	glClearColor(0.f, 1.f, 0.f, 1.f);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float vertices[] = {
		//  Position      Color             Texcoords
    		-0.5f,  0.5f, 0.0f, 0.0f, // Top-left
     		0.5f,  0.5f, 1.0f, 0.0f, // Top-right
     		0.5f, -0.5f, 1.0f, 1.0f, // Bottom-right
    		-0.5f, -0.5f, 0.0f, 1.0f  // Bottom-left
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
    	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	GLuint program = glCreateProgram();

	GLuint vert = ShaderLoad("./shaders/default.vert", GL_VERTEX_SHADER);
	glAttachShader(program, vert);

	GLuint frag = ShaderLoad("./shaders/default.frag", GL_FRAGMENT_SHADER);
	glAttachShader(program, frag);

	glBindFragDataLocation(program, 0, "outColor");
	glLinkProgram(program);

	glUseProgram(program);

	GLint position = glGetAttribLocation(program, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint texcoord = glGetAttribLocation(program, "texcoord");
	glEnableVertexAttribArray(texcoord);
	glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint texture = TextureILLoad("png/george-goblin.png");
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLuint w = ilGetInteger(IL_IMAGE_WIDTH), h = ilGetInteger(IL_IMAGE_HEIGHT);
	GLuint *pixels = (GLuint *) ilGetData();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return LIVE;
}

GLuint ShaderLoad(char *path, GLenum type) {
	Text src = TextFromFile(path);

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar **) &src, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (!status) {
		const GLuint len = 1024;
		char msg[len];
		glGetShaderInfoLog(shader, len, NULL, msg);
		Panic(-1, msg);
	}

	src = TextFree(src);
	return shader;
}
