#ifndef PLAYER_H
#define PLAYER_H
#include "point.h"
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
