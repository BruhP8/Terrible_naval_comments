#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "cell.h"
#include "result.h"
#include "camp.h"
#include "darray.h"
#include "camp_allocator.h"

typedef struct game_state_s {
	cell_t *grid;
	int width;
	int height;
	int cheat;
	int alloc_id;
	camp_allocator_t *camp_allocator;
	darray_t *camps;
	camp_t *winning;
} game_state_t;

/// Défini l'espace de jeu de la nouvelle partie
game_state_t *newGame();

/// Retourne les informations sur le tir effectué
result_t doAction(game_state_t *, player_t *, point_t);

/// Vérifie s'il reste toujours deux joueur en jeu
int turnEndUpdate(game_state_t *);

/// Vérifie si le point donné appartient au rectangle donné
int isPointInsideRect(point_t p, point_t rect[2]);

void interruptHandler(int s);

/// Retourne la case correspondant au point donné
cell_t *getCell(game_state_t *game, point_t co);

/// Retourne les coordonnées d'une case donné
point_t getCoordinates(game_state_t *game, cell_t *c);
#endif
