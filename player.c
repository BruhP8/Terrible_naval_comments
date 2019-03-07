#include "master_head.h"

//Un type inutile......

/**
* Une structure ayant pour seul et unique membre base, un player_t.
*/
typedef struct {
	player_t base;
} local_player_t;
extern char Pieces[][PIECE_SIZE][PIECE_SIZE]; //Variable global..

/** Arguments : Un environnement de jeu, le tableau de enum definissant les couleurs. \n
  * Parcours le tableau représentant la zone de jeu et le remplis de couleurs spécifiées dans les sprintf.\n
  * Retour : rien
*/
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
/** Arguments : Une pièce à afficher, la position relative de la pièce, un environnement
  * de jeu, un identifiant pour savoir quel genre de pièce afficher \n
  * Affichage de la pièce spécifiée sur la zone de jeu, et déclaration de la présence d'
  * un bateau sur cette cellule.\n
  * Retour : rien
*/
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
/** Arguments : Une pièce à afficher, le enum de couleurs, la postion de la pièce \n
  * un environnement de jeu, et un int dont l'utilité est floue
  * Affiche les bateaux placés et en cours de placement sur la zone de jeu \n
  * Retour : un int.
*/
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
			*c = RED;
		}
	return acc;
}
/** Arguments : un point (la position actuelle du curseur), un environnement de jeu \n
  * Déplace le curseur sur la zone de jeu, en fonction des interactions du joueur
  * avec les flèches directionnelles. \n
  * Retour : Rien
*/
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
/** Arguments : un joueur, un envrionnement de jeu \n
  * Détermine le type d'action qu'a fait le joueur lors d'une boucle infinie. \n
  * Exemple : si on recoit ' ', soit un espace, on sort de la boucle car le joueur
  * a fini de faire ce qu'il avait a faire (tirer, placer un bateau...) Appelle
  * le interrupHandler en cas de fin de trasnmission (4 en ASCII = EOF, end of transmission) \n
  * Retour : le point de la grille sélectionné par le joueur quand il appuie sur espace.
*/
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
/** Arguments : Le joueur devant jouer, l'environnement de jeu. \n
  * Gère la phase de placement des bateaux : initialise le curseur au milieu de la
  * zone appartenant au joueur, puis lance le placement des bateaux celon le cheat
  * code utilisé. Espace valide le placement du bateau, r demande la rotation de ce
  * bateau... \n
  * Retour : rien.
*/
static void playerLocalSetBoats(player_t *self, game_state_t *game) {
	point_t prev = {0, 0};
	point_t r = {
		(self->owned_rect[0].x + self->owned_rect[1].x) / 2,
		(self->owned_rect[0].y + self->owned_rect[1].y) / 2
	};

	time_t t;

	srand((unsigned) time(&t));

	for(int i = 0; i < 41; i++){
		point_t p1 = {
			(rand() % (self->owned_rect[1].x - self->owned_rect[0].x) + self->owned_rect[0].x),
			(rand() % (self->owned_rect[1].y - self->owned_rect[0].y) + self->owned_rect[0].y)
		};
		cell_t *c = &game->grid[game->width * p1.y + p1.x];
		c->obstacle = 1;
	}

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

/**
* Fonction qui créée un nouveau joueur en initialisant sa mémoire et get_action et setup_boats \n
* Retour : Pointeur sur player_t
*/
player_t *newLocalPlayer() {
	local_player_t *ret = calloc(1, sizeof(*ret));
	ret->base.get_action = playerLocalAction;
	ret->base.setup_boats = playerLocalSetBoats;
	return &ret->base;
}
/** Arguments : un environnement de jeu, un joueur \n
  * Détermine la couleur que doit prendre la zone de jeu celon le joueu actuel,
  * notement pour faire la différence entre les zones alliées et ennemies \n
  * Retour : un tableau de couleurs
*/
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

			if(game->grid[game->width * i + j].obstacle && j < filter->owned_rect[1].x && j >= filter->owned_rect[0].x){
				if(!game->grid[game->width * i + j].has_exploded)
					arr[game->width * i + j] = BLACK;
			}

		}
	return arr;
}
/** Arguments : 4 pointeurs représentant les 4 coins d'une pièce \n
  * Inverse les positions de manière a effectuer une rotation a 90° vers la droite \n
  * Retour : Rien
*/
void cyclicRoll(char *a, char *b, char *c, char *d) {
	char temp = *a;
	*a = *b;
	*b = *c;
	*c = *d;
	*d = temp;
}
/** Arguments : Une pièce, un numéro de ligne  \n
  * Vérifie si une ligne de la pièce passée en arguments est vide ou non. \n
  * Retour : 1 si c'est le cas, 0 sinon.
*/
int emptyLine(char piece[PIECE_SIZE][PIECE_SIZE], int line) {
	int loop;
	for (loop = 0; loop < PIECE_SIZE; ++loop)
		if (piece[line][loop] != 0)
			return 0;
	return 1;
}
/** Arguments : une pièce, un numéro de colonne \n
  * Vérifie si une colonne de la pièce passée en arguments est vide ou non. \n
  * Retour : 1 si c'est le cas, 0 sinon.
*/
int emptyColumn(char piece[PIECE_SIZE][PIECE_SIZE], int column) {
	int loop;
	for (loop = 0; loop < PIECE_SIZE; ++loop)
		if (piece[loop][column] != 0)
			return 0;
	return 1;
}
/** Arguments : une pièce \n
  * Déplace le contenu de la pièce passée en arguments vers la gauche. \n
  * Retour : rien
*/
void shiftColumnLeft(char piece[PIECE_SIZE][PIECE_SIZE]) {
	int column;
	for (column = 0; column < PIECE_SIZE; ++column) {
		memmove(&piece[column][0], &piece[column][1], PIECE_SIZE - 1);
		piece[column][PIECE_SIZE - 1] = 0;
	}
}
/** Arguments : une pièce \n
  * Déplace le contenu de la pièce passée en arguments vers le haut. \n
  * Retour : rien
*/
void shiftLineUp(char piece[PIECE_SIZE][PIECE_SIZE]) {
	memmove(&piece[0][0], &piece[1][0], sizeof(piece[0]) * (PIECE_SIZE - 1));
	memset(&piece[PIECE_SIZE-1], 0, PIECE_SIZE);
}
/** Arguments : une pièce \n
  * Recentre la pièce le plus "en haut à gauche" possible de sa mémire allouée \n
  * Retour : Rien
*/
void realignPiece(char piece[PIECE_SIZE][PIECE_SIZE]) {
	while (emptyColumn(piece, 0))
		shiftColumnLeft(piece);
	while (emptyLine(piece, 0))
		shiftLineUp(piece);
}
/** Argument : une piece (sous la forme d'un tableau de 5*5) \n
* Fonction pour effectuer une rotation de la piece.
* Le second argument sert a definir le nombre de rotation de 90 degres à effectuer. \n
* Retour : rien
*/
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
/** Arguments : une pièce \n
  * Fonction de débug pas utilisée dans le programme final. Servait a afficher la
  * pièce sous la forme de 0 et de 1 indépendament de l'affichage graphique \n
  * Retour : rien
*/
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
/** Tableau représentant toutes les pièces possibles \n
*/
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
