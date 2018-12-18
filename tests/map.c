#include <stdio.h>

#include "Map.h"

bool PrintPairs(Map *m, char *key, void *value, void *probe) {
	printf("'%s' => '%s'\n", key, (char *) value);
}

int main(int argc, char *argv[]) {
	Map *m = MapNew();
	MapSet(m, "hello", "world");
	MapSet(m, "name", "myke");
	MapSet(m, "age", "2... oh wait");
	MapSet(m, "age", "hi helo");
	MapSet(m, "hobbies", "game boy and ladies");
	MapSet(m, "a", "hi helo");
	MapSet(m, "b", "hi helo");
	MapSet(m, "c", "hi helo");
	MapSet(m, "d", "hi helo");
	MapSet(m, "e", "hi helo");
	MapSet(m, "f", "hi helo");
	MapSet(m, "g", "hi helo");
	MapSet(m, "h", "hi helo");
	MapSet(m, "i", "hi helo");
	MapSet(m, "j", "hi helo");
	MapSet(m, "k", "hi helo");
//	MapSet(m, "l", "hi helo");
//	MapSet(m, "m", "hi helo");
//	MapSet(m, "noon", "hi helo");
//	MapSet(m, "o", "hi helo");
//	MapSet(m, "p", "hi helo");
	MapRemove(m, "hobbies");
//	MapSet(m, "hobbies", "being dull");

	MapForEach(m, PrintPairs);
	printf("%s, my name is %s and I'm %s years old. My hobbies include %s. (size %lu)\n",
		(char *) MapGet(m, "hello"),
		(char *) MapGet(m, "name"),
		(char *) MapGet(m, "age"),
		(char *) MapGet(m, "hobbies"),
		MapSize(m)
	);


	return 0;
}
