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
game_state_t *newGame();
result_t doAction(game_state_t *, player_t *, point_t);
int turnEndUpdate(game_state_t *);
int isPointInsideRect(point_t p, point_t rect[2]);
void interruptHandler(int s);
cell_t *getCell(game_state_t *game, point_t co);
point_t getCoordinates(game_state_t *game, cell_t *c);
#endif 
