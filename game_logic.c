#include "master_head.h"
/**
 newGame: ne prend rien en args , renvoie un ptr sur game_stat
 utilité : elle initialise une partie  en initialisant la taille
 de la grille et on allouant un tableau de cellule et un camp avec
 un cheat code a -1 ; et alloue un camp
 utilité : c'est une fonction necessaire
*/
game_state_t *newGame() {
	game_state_t *ret = calloc(sizeof(*ret), 1);
	ret->width = 17 * 2;
	ret->height = 17;
	ret->grid = calloc(sizeof(cell_t), (size_t)(ret->width * ret->height));
	ret->camps = darrayNew(sizeof(camp_t *));
	ret->cheat = -1;
	
	ret->camp_allocator = newSingleAllocator();
	return ret;
}
int isSunk(game_state_t *game, point_t cp, int id) {
	cell_t *neighbours[4] = {0};
	cell_t *c = &game->grid[game->width * cp.y + cp.x];
	if (!c->has_exploded)
		return 0;
	if (cp.y != 0)
		neighbours[0] = c - game->width;
	if (cp.y != game->height - 1)
		neighbours[1] = c + game->width;
	if (cp.x != 0)
		neighbours[2] = c - 1;
	if (cp.x != game->width - 1)
		neighbours[3] = c + 1;
	
	
	for (int i = 0; i < 4; ++i) {
		if (!neighbours[i])
			continue;
		if (neighbours[i]->boat_id != id)
			continue;
		if (!neighbours[i]->has_exploded)
			return 0;
		if (!neighbours[i]->marked) {
			c->marked |= 1;
			if (!isSunk(game, getCoordinates(game, neighbours[i]), id)) {
				c->marked = 0;
				return 0;
			}
			c->marked = 0;
		}
	}
	return 1;
}
void setSunk(game_state_t *game, point_t cp, int id) {
	cell_t *neighbours[4] = {0};
	cell_t *c = &game->grid[game->width * cp.y + cp.x];
	if (!c->has_exploded)
		return;
	if (cp.y != 0)
		neighbours[0] = c - game->width;
	if (cp.y != game->height - 1)
		neighbours[1] = c + game->width;
	if (cp.x != 0)
		neighbours[2] = c - 1;
	if (cp.x != game->width - 1)
		neighbours[3] = c + 1;
	for (int i = 0; i < 4; ++i) {
		if (!neighbours[i])
			continue;
		if (neighbours[i]->boat_id != id)
			continue;
		if (!neighbours[i]->has_sunk) {
			c->has_sunk |= 1;
			setSunk(game, getCoordinates(game, neighbours[i]), id);
		}
	}
	c->has_sunk |= 1;
}
player_t *findOwner(game_state_t *game, point_t p) {
	for (unsigned i = 0; i < darraySize(game->camps); ++i) {
		camp_t *c = *(camp_t**)darrayGet(game->camps, i);
		for (unsigned j = 0; j < darraySize(c->players); ++j) {
			player_t *player = *(player_t**)darrayGet(c->players, j);
			if (isPointInsideRect(p, player->owned_rect))
				return player;
		}
	}
	return 0;
}

/**
fonction qui prend en args un game_state qui contient toutes les infos
d'une partie  un joueur(celui qui attaque) et le point qu'il attaque
 cette fonction nous permet d'attauquer un point de l'equipe adeverse 
 apres avoir verifier que celui est bien un point de l'equipe adverse 
 qu'il n'a pas deja ete attaque return un result REDO qui veut dire
 que le joueur attaque son camp
 SUNK qu'il l'a coulé un bateau
 Hit qu'on attauque un point d'un bateau pas tous le bateau
 et MISS qu'on a rien eu
*/
result_t doAction(game_state_t *game, player_t *player, point_t coordinates) {
	cell_t *c;
	c = &game->grid[game->width * coordinates.y + coordinates.x];
	if (isPointInsideRect(coordinates, player->owned_rect) || c->has_exploded)
		return REDO;
	c->has_exploded |= 1;
	if (c->has_boat) {
		if (isSunk(game, coordinates, c->boat_id)) {
			setSunk(game, coordinates, c->boat_id);
			player_t *target = findOwner(game, coordinates);
			--target->n_boats;
			return SUNK;
		}
		return HIT;
	}
	return MISS;
}
int turnEndUpdate(game_state_t *game) {
	int n_alive = 0;
	
	for (unsigned i = 0; i < darraySize(game->camps); ++i) {
		camp_t *c = *(camp_t**)darrayGet(game->camps, i);
		c->is_alive = 0;
		for (unsigned j = 0; j < darraySize(c->players); ++j) {
			player_t *player = *(player_t**)darrayGet(c->players, j);
			c->is_alive = c->is_alive || player->n_boats;
		}
		n_alive += c->is_alive;
		if (c->is_alive)
			game->winning = c;
	}
	return n_alive > 1;
}

/**
	prend en args un point et un tableau de deux points 
	verifie que le point est inclut dans le rectangle

*/
int isPointInsideRect(point_t p, point_t rect[2]) {
	return p.x >= rect[0].x && p.x < rect[1].x &&
		p.y >= rect[0].y && p.y < rect[1].y;
}
/**
prend en args 
*/
cell_t *getCell(game_state_t *game, point_t co) {
	return &game->grid[game->width * co.y + co.x];
}
point_t getCoordinates(game_state_t *game, cell_t *c) {
	int dist = c - game->grid;
	return (point_t) {
		dist % game->width,
		dist / game->width
	};
}
