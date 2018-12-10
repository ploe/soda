#include <stdlib.h>
#include <stdio.h>

void Panic(int i, const char *prompt) {
	fprintf (stderr, "%s\n", prompt);
	exit(i);
}

