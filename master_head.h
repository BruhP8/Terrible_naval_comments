/* ------ head.h ----- */

#include <signal.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>



/* ------ camp.h ----- */

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

/* ------  camp_allocator.h ------ */

#ifndef CAMP_ALLOCATOR_H
#define CAMP_ALLOCATOR_H
typedef struct game_state_s game_state_t;
typedef struct player_s player_t;
typedef struct camp_allocator_s {
	void (*put_in_camp)(struct camp_allocator_s *, game_state_t *, player_t *);
} camp_allocator_t;
camp_allocator_t *newSingleAllocator();
#endif 

/* ------ cell.h ----- */

#ifndef CELL_H
#define CELL_H
typedef struct {
	union {
		struct {
			int has_boat : 1;
			int has_exploded : 1;
			int has_sunk : 1;
			int marked : 1;
		};
		unsigned char state;
	};
	unsigned char boat_id;
} cell_t;
#endif

/* ------ darray.h ------ */

#ifndef DARRAY_H
#define DARRAY_H
typedef struct {
	void *begin;
	void *end;
	size_t element_size;
	size_t capacity;
} darray_t;
darray_t *darrayNew(size_t element_size);
void  darrayInsert(darray_t *self, void *pos, void *elem);
void  darrayErase(darray_t *self, void *pos);
void *darrayGet(darray_t *self, size_t idx);
size_t darraySize(darray_t *self);
size_t darrayElemSize(darray_t *self);
void  darrayPushBack(darray_t *self, void *elem);
void  darrayPopBack(darray_t *self);
void  darrayDelete(darray_t *self);
#endif 

/* ------ game_state.h ------ */

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


/* ------ player.h ------ */

#ifndef PLAYER_H
#define PLAYER_H
#define PIECE_SIZE 5
struct game_state_s;
typedef struct game_state_s game_state_t;
typedef struct player_s {
	point_t (*get_action)(struct player_s *, game_state_t *);
	void (*setup_boats)(struct player_s *, game_state_t *);
	char *name;
	point_t owned_rect[2];
	int n_boats;
} player_t;
player_t *newLocalPlayer();
typedef enum {
	BLACK,
	RED = 41,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
} color_t;
color_t *stateToView(game_state_t *game, player_t *filter);
void printColorArray(game_state_t *game, color_t *arr);
void rotate(char piece[5][5], int rotation_nb);
void printPiece(char piece[PIECE_SIZE][PIECE_SIZE]);
#endif 

/* ------- point.h ------ */
#ifndef POINT_H
#define POINT_H
typedef struct {
	int x;
	int y;
} point_t;
#endif 

/* ------ result.h ------ */

#ifndef RESULT_H
#define RESULT_H
typedef enum {
	REDO = -1,
	MISS,
	HIT,
	SUNK
} result_t;
#endif 
