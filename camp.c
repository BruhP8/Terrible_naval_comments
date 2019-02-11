#include "head.h"

camp_t *newCamp() {
	camp_t *ret = calloc(1, sizeof(*ret));
	ret->players = darrayNew(sizeof(player_t*));
	ret->is_alive = 1;
	return ret;
}
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
		it += sprintf(it, "%s", players[i].name);
		if (i < n - 1)
			it += sprintf(it, ", ");
	}
	return self->team_string;
}
void deleteCamp(camp_t *self) {
	for (unsigned j = 0; j < darraySize(self->players); ++j) {
		player_t *player = *(player_t**)darrayGet(self->players, j);
		
		free(player);
	}
	darrayDelete(self->players);
	free(self->team_string);
	free(self);
}
