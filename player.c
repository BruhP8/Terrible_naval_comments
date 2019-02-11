#include "head.h"

typedef struct {
	player_t base;
} local_player_t;
extern char Pieces[][PIECE_SIZE][PIECE_SIZE];
void printColorArray(game_state_t *game, color_t *arr) {
	
	
	
	
	char *buf = malloc(game->height * game->width * sizeof(char) * 7 + (game->height * 7) + 9);
	buf[0] = 0;
	char *it = buf;
	it += sprintf(it, "\033[2J\033[0;0f");
	color_t current = BLACK;
	for (int i = 0; i < game->height; ++i) {
		for (int j = 0; j < game->width; ++j) {
			color_t c = arr[game->width * i + j];
			if (c != current) {
				it += sprintf(it, "\033[%dm", c);
				current = c;
			}
			it += sprintf(it, "  ");
		}
		current = BLACK;
		it += sprintf(it, "\033[0m\n\r");
	}
	it += sprintf(it, "\033[0m");
	puts(buf);
	free(buf);
}
void blitToGrid(char (*piece)[5][5], point_t pos, game_state_t *game, int id) {
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j) {
			char v = (*piece)[i][j];
			if (!v)
				continue;
			if (j + pos.x >= game->width) {
				continue;
			}
			if (i + pos.y >= game->height) {
				continue;
			}
			cell_t *c = &game->grid[game->width * (i + pos.y) + (j + pos.x)];
			c->has_boat |= 1;
			c->boat_id = id;
		}
}
int blitBoat(char (*piece)[5][5], color_t *arr, point_t pos, game_state_t *game, int *add) {
	int acc = 0;
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j) {
			char v = (*piece)[i][j];
			if (!v)
				continue;
			if (j + pos.x >= game->width) {
				acc += 1 + game->width - (j + pos.x);
				continue;
			}
			if (i + pos.y >= game->height) {
				acc += 1 + game->height - (i + pos.y);
				continue;
			}
			color_t *c = &arr[game->width * (i + pos.y) + (j + pos.x)];
			acc += (*c != CYAN);
			add[0] += 1;
			*c = WHITE;
		}
	return acc;
}
static void cursorMovement(point_t *r, game_state_t *game) {
	switch(getch()) {
	case '[':
		switch(getch()) {
		case 'A':
			if (r->y > 0)
				--r->y;
			break;
		case 'B':
			if (r->y < game->height - 1)
				++r->y;
			break;
		case 'C':
			if (r->x < game->width - 1)
				++r->x;
			break;
		case 'D':
			if (r->x > 0)
				--r->x;
			break;
		default:
			break;
		}
		break;
	}
}
static point_t playerLocalAction(player_t *self, game_state_t *game) {
	point_t r = {game->width / 2, game->height / 2};
	while (1) {
		color_t *arr = stateToView(game, self);
		arr[game->width * r.y + r.x] = BLACK;
		printColorArray(game, arr);
		printf("Tour du joueur %s\n\r", self->name);
		refresh();
		free(arr);
		while (1) {
			int c = getch();
			if (c == ' ')
				return r;
			else if (c == '\033') {
				cursorMovement(&r, game);
				break;
			}
			else if (c == 4) {
				interruptHandler(0);
				break;
			}
		}
	}
	
	return r;
}
static void playerLocalSetBoats(player_t *self, game_state_t *game) {
	point_t prev = {0, 0};
	point_t r = {
		(self->owned_rect[0].x + self->owned_rect[1].x) / 2,
		(self->owned_rect[0].y + self->owned_rect[1].y) / 2
	};
	for(int i = 0; i < 7;) {
		if (game->cheat > -1)
			i = game->cheat;
		char boat_mem[5][5];
		char (*boat)[5][5] = &boat_mem;
		memcpy(boat, Pieces[i], sizeof(boat_mem));
		while (1) {
			color_t *arr = stateToView(game, self);
			int k = 0;
			int coll = blitBoat(boat, arr, r, game, &k);
			printColorArray(game, arr);
			printf("[%s] Flèches pour déplacer, r pour tourner, espace pour valider\n\r", self->name);
			
			int c = getch();
			if (c == ' ') {
				if (!coll) {
					prev = r;
					++i;
					blitToGrid(boat, r, game, ++game->alloc_id);
					r = (point_t) {
						(self->owned_rect[0].x + self->owned_rect[1].x) / 2,
						(self->owned_rect[0].y + self->owned_rect[1].y) / 2
					};
					if (game->cheat > -1)
						i = 10;
				}
				else
					r = prev;
				free(arr);
				break;
			}
			else if (c == 'r') 
				rotate(*boat, 1);
			else if (c == '\033') 
				cursorMovement(&r, game);
			else if (c == 4) 
				interruptHandler(0);
			free(arr);
		}
	}
	self->n_boats = 7;
	if (game->cheat > -1)
		self->n_boats = 1;
}
player_t *newLocalPlayer() {
	local_player_t *ret = calloc(1, sizeof(*ret));
	ret->base.get_action = playerLocalAction;
	ret->base.setup_boats = playerLocalSetBoats;
	return &ret->base;
}
color_t *stateToView(game_state_t *game, player_t *filter) {
	
	color_t *arr = calloc(game->width * game->height, sizeof(color_t));
	static const color_t player_colors[] = {CYAN, WHITE, YELLOW, MAGENTA, 0, 0, 0, RED};
	static const color_t foe_colors[] = {BLUE, BLUE, CYAN, MAGENTA, 0, 0, 0, RED};
	for (int i = 0; i < game->height; ++i)
		for (int j = 0; j < game->width; ++j) {
			int state = game->grid[game->width * i + j].state & 0x7;
			const color_t *values = isPointInsideRect((point_t){j, i}, filter->owned_rect) ?
				player_colors : foe_colors;
			arr[game->width * i + j] = values[state];
		}
	return arr;
}
void cyclicRoll(char *a, char *b, char *c, char *d) {
	char temp = *a;
	*a = *b;
	*b = *c;
	*c = *d;
	*d = temp;
}
int emptyLine(char piece[PIECE_SIZE][PIECE_SIZE], int line) {
	int loop;
	for (loop = 0; loop < PIECE_SIZE; ++loop)
		if (piece[line][loop] != 0)
			return 0;
	return 1;
}
int emptyColumn(char piece[PIECE_SIZE][PIECE_SIZE], int column) {
	int loop;
	for (loop = 0; loop < PIECE_SIZE; ++loop)
		if (piece[loop][column] != 0)
			return 0;
	return 1;
}
void shiftColumnLeft(char piece[PIECE_SIZE][PIECE_SIZE]) {
	int column;
	for (column = 0; column < PIECE_SIZE; ++column) {
		memmove(&piece[column][0], &piece[column][1], PIECE_SIZE - 1);
		piece[column][PIECE_SIZE - 1] = 0;
	}
}
void shiftLineUp(char piece[PIECE_SIZE][PIECE_SIZE]) {
	memmove(&piece[0][0], &piece[1][0], sizeof(piece[0]) * (PIECE_SIZE - 1));
	memset(&piece[PIECE_SIZE-1], 0, PIECE_SIZE);
}
void realignPiece(char piece[PIECE_SIZE][PIECE_SIZE]) {
	while (emptyColumn(piece, 0))
		shiftColumnLeft(piece);
	while (emptyLine(piece, 0))
		shiftLineUp(piece);
}
void rotate(char piece[5][5], int rotation_nb) {
	int outer_loop, inner_loop, rotations;
	for (rotations = 0; rotations < rotation_nb; ++rotations) {
		for (outer_loop = 0; outer_loop < PIECE_SIZE / 2; ++outer_loop)
			for (inner_loop = 0; inner_loop < (PIECE_SIZE + 1) / 2; ++inner_loop)
				cyclicRoll(
						&piece[outer_loop][inner_loop],
						&piece[PIECE_SIZE - 1 - inner_loop][outer_loop],
						&piece[PIECE_SIZE - 1 - outer_loop][PIECE_SIZE - 1 - inner_loop],
						&piece[inner_loop][PIECE_SIZE - 1 - outer_loop]);
	}
	realignPiece(piece);
}
void printPiece(char piece[PIECE_SIZE][PIECE_SIZE]) {
	(void)piece;
#ifdef Debug
	int inner_loop, outer_loop;
	for (outer_loop = 0; outer_loop < PIECE_SIZE; ++outer_loop) {
		for (inner_loop = 0; inner_loop < PIECE_SIZE; ++inner_loop)
			printf("%d ", piece[outer_loop][inner_loop]);
		printf("\n");
	}
#endif
}
char Pieces[][PIECE_SIZE][PIECE_SIZE] = {{{1, 1, 0, 0, 0},
										  {1, 1, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0}},
										 {{1, 1, 1, 1, 0},
										  {1, 1, 1, 1, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0}},
										 {{1, 1, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0}},
										 {{1, 1, 1, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0}},
										 {{1, 1, 1, 1, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0}},
										 {{1, 1, 1, 1, 1},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0}},
										 {{1, 1, 1, 1, 0},
										  {0, 0, 0, 1, 0},
										  {0, 0, 0, 1, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0}}};
