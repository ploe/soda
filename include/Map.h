#ifndef TR_MAP
#define TR_MAP

#include <stdbool.h>

#include "Text.h"

typedef struct Pair {
	Text key;
	void *value;
} Pair;

typedef struct Map {
	Pair *pairs;
	double load;
	unsigned long mask;
} Map;

typedef bool (*MapMethod)(Map *m, char *key, void *value, void *probe);

#define MAP_DEFAULT_MASK 0x0F

Map *MapNewWithMask(unsigned long mask);
#define MapNew() MapNewWithMask(MAP_DEFAULT_MASK)

bool MapSet(Map *m, char *key, void *value);
void *MapGet(Map *m, char *key);
void MapRemove(Map *m, char *key);

bool MapProbeEach(Map *m, MapMethod method, void *probe);
#define MapForEach(m, method) MapProbeEach(m, method, NULL)

Map *MapGrow(Map *m);
unsigned long MapSize(Map *m);

#endif
