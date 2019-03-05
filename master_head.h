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

#define HEIGHT 25
#define WIDTH 25

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

/* ------ darray.h ------ */

#ifndef DARRAY_H
#define DARRAY_H
typedef struct {
	void *begin;
	void *end;
	size_t element_size;
	size_t capacity;
} darray_t;
/// Créé une nouvelle structure darray_t vide et pouvant avoir des éléments de element_size
darray_t *darrayNew(size_t element_size);
/// Insère un élément à la position pos du darray
void  darrayInsert(darray_t *self, void *pos, void *elem);
/// Supprime l'élément situé au pos - ième élément
void  darrayErase(darray_t *self, void *pos);
/// Retourne un pointeur sur l'élément à la position pos
void *darrayGet(darray_t *self, size_t idx);
/// Renvoie la taille du darray_t passé en argument
size_t darraySize(darray_t *self);
// size_t darrayElemSize(darray_t *self);
/// Fake macro pour appeller insert à la fin.
void  darrayPushBack(darray_t *self, void *elem);
/// Fake macro pour appeller Erase sur le dernier élément
void  darrayPopBack(darray_t *self);
/// Supprime le darray_t passé en argument
void  darrayDelete(darray_t *self);
#endif

/* ------ camp.h ----- */

#ifndef CAMP_H
#define CAMP_H
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
/// Création du joueur, attribution de sa zone de jeu, appel au placement de ses bateaux
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

/* ------ game_state.h ------ */

#ifndef GAME_STATE_H
#define GAME_STATE_H

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
/// Initialisation d'une nouvelle zone de jeu
game_state_t *newGame();
/// Détermine celon la position jouée la suite du programme : rejouer, touché, raté, coulé...
result_t doAction(game_state_t *, player_t *, point_t);
/// Regarde si la fin du tour entraine la victoire d'un des deux camps
int turnEndUpdate(game_state_t *);
/// Vérifie si le point entré en argument est jouable dans le rectangle
int isPointInsideRect(point_t p, point_t rect[2]);
/// Met fin à la partie, annonce le gagnant et stoppe le programme ?
void interruptHandler(int s);
/// Selectionne une cellule
cell_t *getCell(game_state_t *game, point_t co);
/// Récupère les coordonnées d'une cellule
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

///Fonction qui créée un nouveau joueur.
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
/// Coloration de la zone de jeu
void printColorArray(game_state_t *game, color_t *arr);
/// Fonction qui gere les rotations
void rotate(char piece[5][5], int rotation_nb);
void printPiece(char piece[PIECE_SIZE][PIECE_SIZE]);
#endif
