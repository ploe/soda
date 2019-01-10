#include <SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <IL/il.h>

#include "Crew.h"
#include "Text.h"
#include "Panic.h"
#include "Window.h"

GLuint vao;
GLuint vbo;
GLuint ebo;
GLuint program;
GLuint vert;
GLuint frag;
GLuint texture;
SDL_GLContext context;

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

CrewStatus WindowGLDestroy() {
	glDeleteTextures(1, &texture);

	glDeleteProgram(program);
	glDeleteShader(frag);
	glDeleteShader(vert);

	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	return CUT;
}

CrewStatus WindowGLInit(Crew *c) {
	c->update =  WindowGLUpdate;
	c->destroy = WindowGLDestroy;

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

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float vertices[] = {
		//  Position      Color             Texcoords
    		-0.5f,  0.5f, 0.0f, 0.0f, // Top-left
     		0.5f,  0.5f, 1.0f, 0.0f, // Top-right
     		0.5f, -0.5f, 1.0f, 1.0f, // Bottom-right
    		-0.5f, -0.5f, 0.0f, 1.0f  // Bottom-left
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
    	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	program = glCreateProgram();

	vert = ShaderLoad("./shaders/default.vert", GL_VERTEX_SHADER);
	glAttachShader(program, vert);

	frag = ShaderLoad("./shaders/default.frag", GL_FRAGMENT_SHADER);
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

	texture = TextureILLoad("png/george-goblin.png");
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
