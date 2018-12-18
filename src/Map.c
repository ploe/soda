#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "Map.h"
#include "Text.h"

static unsigned long djb2(const char *str) {
	unsigned long hash = 5381;

	int c;
	while (c = *str++) {
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

#define MAP_DEFAULT_MASK 0x0F

Map *MapNewWithMask(unsigned long mask) {
	Map *m = calloc(1, sizeof(Map));
	if (m) {
		m->mask = mask;
		m->buckets = calloc(mask + 1, sizeof(Bucket));
		if (!m->buckets) {
			free(m);
			return NULL;
		}
	}

	return m;
}

static unsigned long BucketIndex(unsigned long hash, unsigned long mask) {
	return hash & mask;
}

static Pair **BucketTopAddress(Map *m, char *key) {
	unsigned long hash, index;
	hash = djb2(key);

	index = BucketIndex(hash, m->mask);
	return &(m->buckets[index].top);
}

static Pair *BucketTop(Map *m, char *key) {
	return *BucketTopAddress(m, key);
}

static Pair *PairFind(Map *m, char *key) {
	Pair *p;
	for (p = BucketTop(m, key); p != NULL; p = p->next) {
		if ((strcmp(p->key, key) == 0)) break;
	}

	return p;
}

static void PairFree(Pair *p) {
	TextFree(p->key);
	free(p);
}

static Pair *PairNew(Map *m, char *key) {
	Pair *p = calloc(1, sizeof(Pair));
	if (p) {
		p->key = TextNew(key);
		if (!p->key) {
			free(p);
			return NULL;
		}

		Pair **top = BucketTopAddress(m, key);
		p->next = *top;
		*top = p;
	}

	return p;
}

bool MapSet(Map *m, char *key, void *value) {
	Pair *p = PairFind(m, key);
	if (!p) p = PairNew(m, key);
	if (p) {
		p->value = value;	
		return true;
	}

	return false;
}

void *MapGet(Map *m, char *key) {
	Pair *p = PairFind(m, key);

	if (p) return p->value;

	return NULL;
}

void MapRemove(Map *m, char *key) {
	Pair *p = PairFind(m, key);
	if (p) {
		Pair **current = BucketTopAddress(m, key);

		while ((*current != p) && (*current != NULL)) {
			current = &(*current)->next;
		}

		if (*current) {
			*current = p->next;
			PairFree(p);
		}
	}
}
