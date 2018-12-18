#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "Map.h"
#include "Text.h"

typedef unsigned long Hash;

static Hash djb2WithHash(Hash hash, const char *str) {
	int c;
	while (c = *str++) {
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

#define DJB2_DEFAULT_HASH 5381

#define djb2(str) djb2WithHash(DJB2_DEFAULT_HASH, str)

#define MAP_DEFAULT_MASK 0x0F

static Hash PairIndex(Hash hash, Hash mask) {
	return hash & mask;
}

static Pair *PairFindAddress(Map *m, char *key) {
	Hash hash = DJB2_DEFAULT_HASH;
	Pair *p;

	do {
		hash = djb2WithHash(hash, key);
		Hash index = PairIndex(hash, m->mask);
		p = &(m->pairs[index]);
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


static bool MapCopy(Map *m, char *key, void *value, void *new) {
	return MapSet((Map *) new, key, value);
}

static Map *MapRehash(Map *old, Hash mask) {
	Map *new = MapNewWithMask(mask);
	if (!new) return old;

	free(old);
	if (!MapProbeEach(old, MapCopy, new)) return NULL;
	
	return new;
}

Map *MapGrow(Map *m) {
	Hash mask = m->mask;
	mask <<= 1;
	mask += 1;
	return MapRehash(m, mask);
}

bool MapSet(Map *m, char *key, void *value) {
	Pair *p = PairFindAddress(m, key);
	if (!p->key) p->key = TextNew(key);

	p->value = value;

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
