#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Map.h"
#include "Text.h"

typedef unsigned long Hash;

static Hash djb2WithHash(Hash hash, const char *str) {
	int c;
	while ( (c = *str++) ) {
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

#define DJB2_DEFAULT_HASH 5381

#define djb2(str) djb2WithHash(DJB2_DEFAULT_HASH, str)

#define MAP_DEFAULT_MASK 0x0F

double MapGetCurrentLoad(Map *m) {
	return MapSize(m) / m->mask;
}

bool MapIsOverLoaded(Map *m) {
	return (MapGetCurrentLoad(m) >= m->load);	
}

static Hash PairIndex(Hash hash, Hash mask) {
	return hash & mask;
}

static Pair *PairFindAddress(Map *m, char *key) {
	Hash hash = DJB2_DEFAULT_HASH, collisions = 0;

	Pair *p;
	do {
		// if the key can't be found within the load limit,
		// we break out, this we can MapGrow
		double load = (double) collisions / (double) m->mask;
		if (load >= m->load) return NULL;

		hash = djb2WithHash(hash, key);
		Hash index = PairIndex(hash, m->mask);
		p = &(m->pairs[index]);
		collisions++;
	} while ((p->key) && (strcmp(p->key, key) != 0));

	return p;
}

Map *MapNewWithMask(Hash mask) {
	Map *m = calloc(1, sizeof(Map));
	if (m) {
		m->load = 0.75;
		m->mask = mask;
		m->pairs = calloc(mask + 1, sizeof(Pair));
		if (!m->pairs) {
			free(m);
			return NULL;
		}
	}

	return m;
}

Map *MapFree(Map *m) {
	free(m->pairs);
	free(m);
	return NULL;
}

static bool MapCopy(Map *m, char *key, void *value, void *new) {
	return MapSet((Map *) new, key, value);
}

#define GROW(mask) ((mask << 1) + 1)

static Map *MapRehash(Map *old, Hash mask) {
	Map *new = MapNewWithMask(mask);

	// just grow the hash till all the elems fit in
	if (new && MapProbeEach(old, MapCopy, new) == false) {
		MapFree(old);
		return new;
	}

	if (new) MapFree(new);
	return old;
	
}

Map *MapGrow(Map *m) {
	Hash mask = m->mask;
	mask <<= 1;
	mask += 1;
	return MapRehash(m, mask);
}

bool MapSet(Map *m, char *key, void *value) {
	Pair *p = PairFindAddress(m, key);
	if (p) {
		if (!p->key) p->key = TextNew(key);
		p->value = value;
		
		return true;
	}

	return false;
}

void *MapGet(Map *m, char *key) {
	Pair *p = PairFindAddress(m, key);
	return p->value;
}

void MapRemove(Map *m, char *key) {
	Pair *p = PairFindAddress(m, key);
	
	if (p->key) p->key = TextFree(p->key);
	p->value = NULL;
}

bool MapProbeEach(Map *m, MapMethod method, void *probe) {
	Hash i;
	bool again;
	for (i = 0; i <= m->mask; i++) {
		Pair *p = &(m->pairs[i]);
		again = method(m, p->key, p->value, probe);

		if (!again) break;
	}

	return again;
}

static bool MapCountElems(Map *m, char *key, void *value, void *probe) {
	int *elems = (int *) probe;
	if (key) (*elems)++;

	return true;
}

unsigned long MapSize(Map *m) {
	unsigned long elems = 0;
	MapProbeEach(m, MapCountElems, &elems);

	return elems;
}
