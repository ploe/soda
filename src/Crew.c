#include <stdlib.h>

#include "Crew.h"

/*	Linked list of all the Crew members. They represent different
	components within the engine. */
static Crew *top = NULL;

/*	Method for creating a new Crew member.	*/
Crew *CrewNew(CrewMethod type) {
	Crew *c = calloc(1, sizeof(Crew));
	if (c) {
		if(type) {
			c->status = type(c);
			c->type = type;
		}

		c->next = top;
		top = c;
	}

	return c;
}

/*	Method that runs the update method for each Crew member, but
	only if it's LIVE.

	If EXIT is returned the app is closed.	*/
bool CrewRoll() {
	Crew *c;
	for (c = top; c != NULL; c = c->next) {
		if (c->status == LIVE) {
			if (c->update) c->status = c->update(c);
		}

		if (c->status == EXIT) return false;
	}

	return true;
}
