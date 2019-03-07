#include "master_head.h"
/**
inialise un camp avec aux moins un joueur 
dans la strucute camp_t avec un calloc en les mettant a 1
le champ player est inialiser avec un pointeur
*/
camp_t *newCamp() {
	camp_t *ret = calloc(1, sizeof(*ret));
	ret->players = darrayNew(sizeof(player_t*));
	ret->is_alive = 1;
	return ret;
}/**
	pour un camp donner renvoie une chaine de caracteres 
	contenant le nom de chaque joueur 
*/
char *campTeamString(camp_t *self) {
	unsigned n = darraySize(self->players);
	player_t *players = *(player_t **)darrayGet(self->players, 0);
	int len = 0;
	for (unsigned i = 0; i < n; ++i) {
		len += players[i].name ? strlen(players[i].name) : 0;
	}
	self->team_string = malloc(len + (2 * (len - 1)) + 1);
	char *it = self->team_string;
	for (unsigned i = 0; i < n; ++i) {
		 it+=sprintf(it, "%s", players[i].name);
		if (i < n - 1)
			it += sprintf(it, ", ");
	}
	return self->team_string;
}
/**
	Argument : un camp  \n
	elle supprime tous ces champs en fin de partie \n
	utilité : libere la memoire en fin de partie pour eviter les pertes 
*/
void deleteCamp(camp_t *self) {
	for (unsigned j = 0; j < darraySize(self->players); ++j) {
		player_t *player = *(player_t**)darrayGet(self->players, j);
		free(player);
	}
	darrayDelete(self->players);
	free(self->team_string);
	free(self);
}
