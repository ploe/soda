#ifndef SODA_CREW_H
#define SODA_CREW_H

#include <stdbool.h>
#include <stdlib.h>

typedef int CrewStatus;
enum {
	LIVE = 0,
	PAUSE,
	CUT,
	EXIT,
};

// forward declaration of Crew
typedef struct Crew Crew;

/*	CrewMethod is the type used to describe methods for the Crew	*/
typedef CrewStatus (*CrewMethod)(Crew *);

/* 	Type for Crew which is a state in the State machine. 
	The Crew are a linked list of actions we need to execute each and
	every frame.	*/ 
typedef struct Crew {
	char *tag;
	CrewMethod type, update, destroy;
	CrewStatus status;	
	struct Crew *next;
} Crew;

/*	Methods for the Crew module	*/
Crew *CrewNew(CrewMethod type);
bool CrewRoll();

#endif
