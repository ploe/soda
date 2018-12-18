#ifndef TR_MAP
#define TR_MAP

#include <stdbool.h>

#include "Text.h"

typedef struct Pair {
	struct Pair *next;
	Text key;
	void *value;
} Pair;

typedef struct Bucket {
	Pair *top;
} Bucket;

typedef struct Map {
	Bucket *buckets;
	double load;
	unsigned long mask;
} Map;

#define MAP_DEFAULT_MASK 0x0F

Map *MapNewWithMask(unsigned long mask);
#define MapNew() MapNewWithMask(MAP_DEFAULT_MASK)

bool MapSet(Map *m, char *key, void *value);
void *MapGet(Map *m, char *key);
void MapRemove(Map *m, char *key);

#endif
