#ifndef CAMP_H
#define CAMP_H

#include "player.h"
#include "darray.h"

typedef struct camp_s {
	darray_t *players;
	int is_alive;
	char *team_string;
} camp_t;

/// Création d'un camp
camp_t *newCamp();

///
char *campTeamString(camp_t *);

/// Supprime le camp donné et libère la mémoire utilisée
void deleteCamp(camp_t *);
#endif
