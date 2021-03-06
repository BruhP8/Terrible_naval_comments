#include "master_head.h"
/** Arguments : Rien \n
  * Initialise une partie en créant une grille de jeu et en allouant un tableau
  * de cellule et un camp. Un cheat_code est également initialisé à -1 (inactif) \n
  * Retour : Pointeur sur l'instance de jeu nouvellement créée
*/
game_state_t *newGame() {
	game_state_t *ret = calloc(sizeof(*ret), 1);
	ret->width = WIDTH * 3;
	ret->height = HEIGHT;
	ret->grid = calloc(sizeof(cell_t), (size_t)(ret->width * ret->height));
	ret->camps = darrayNew(sizeof(camp_t *));
	ret->cheat = -1;
	ret->camp_allocator = newSingleAllocator();
	return ret;
}
/**
	Args : game_state_t l'etat courant d'une partie , un point , et l'id de la cellule \n
	elle nous permet de savoir si oui ou non un bateau a coule \n

	retour un 0 si on a pas coule sinon 1
*/

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
/**
	Args : game_state_t l'etat courant d'une partie , un point , et l'id de la cellule \n
	elle nous permet de mettre a jour pour un point donneé si oui ou non le bateau est coule
	elle complete la fonction isSunk \n
	retour  rien
*/
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
/**
	args: fonction qui prend une game_state_t et point  \n
	ils nous indique de quel camps somme nous \n
	elle renvoie un pointeur sur joueur
	elle nous sert a pouvoir attaquer ou poser les bateaux
*/
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

/** Arguments : L'état de la partie actuelle, le joueur actuel et les coordonnées selectionnées
  *             par celui ci \n
  * Apres avoir vérifié que les coordonnées entrées n'ont pas déja été entrées et qu'elles
  * sont jouables (sinon, envoie REDO pour relancer le prompt), détruit le point touché (
  * retour HIT) et fait couler le bateau si besoin (retour SUNK). Enfin si le tir ne
  * touche pas, retourne MISS \n
  * Retour : renvoie HIT, SUNK MISS ou REDO celon la situation ce qui determinera ce que
  *          le programme doit faire ensuite.
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
/** Arguments : Une zone de jeu \n
  * Parcours la zone de jeu et compte le nombre de bateaux encore en vie \n
  * Retourne 1 si ce nombre est supérieur à 1, 0 sinon
*/
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

/** Arguments : Le point concerné, un rectangle représentant la zone attaquable \n
  * Vérifie si le point visé est bien inclus dans le rectangle jouable. En d'autres
  * termes, si il est dans le camp ennemi \n
  *	Retour : 1 si jouable, 0 sinon
*/
int isPointInsideRect(point_t p, point_t rect[2]) {
	return p.x >= rect[0].x && p.x < rect[1].x &&
		p.y >= rect[0].y && p.y < rect[1].y;
}
/** Arguments : une zone de jeu et un point \n
  * Sélectionne la cellule passée en argument \n
  * Retour : cette cellule
*/
cell_t *getCell(game_state_t *game, point_t co) {
	return &game->grid[game->width * co.y + co.x];
}
/** Arguments : Une zone de jeu et un point \n
  * Récupère les coordonnées d'une cellule \n
  * Retour : les coordonnées de la cellue
*/
point_t getCoordinates(game_state_t *game, cell_t *c) {
	int dist = c - game->grid;
	return (point_t) {
		dist % game->width,
		dist / game->width
	};
}
