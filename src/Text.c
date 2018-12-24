#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "Panic.h"

typedef char *Text;

enum {
	ELOAD = -1,
};

Text TextNew(const char *format, ...) {
	va_list size_args, format_args;
	va_start(format_args, format);
	va_copy(size_args, format_args);

	int size = 1 + vsnprintf(NULL, 0, format, size_args);
	va_end(size_args);

	Text t = calloc(size, sizeof(char));
	vsnprintf (t, size, format, format_args);
	va_end(format_args);

	return t;
}

Text TextFromFile(const char *path) {
	FILE *file = fopen(path, "r");
	if (!file) {
		Panic(ELOAD, "unable to load shader");
	}

	long size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	Text buffer = calloc(size+1, sizeof(char));
	if (!buffer) Panic(ELOAD, "unable to allocate shader src");

	if(fread(buffer, 1, size, file) != size) Panic(ELOAD, "unable to read file in to memory");

	fclose(file);
	return buffer;
}

Text TextFree(Text t) {
	if(t) free(t);
	return NULL;
}
