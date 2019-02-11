#ifndef CAMP_H
#define CAMP_H
#include "player.h"
#include "darray.h"
typedef struct camp_s {
	darray_t *players;
	int is_alive;
	char *team_string;
} camp_t;
camp_t *newCamp();
char *campTeamString(camp_t *);
void deleteCamp(camp_t *);
#endif 
