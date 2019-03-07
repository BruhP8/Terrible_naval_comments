#include "master_head.h"
/** Argument : un signal (Sous la forme d'un int) \n
  * Fonction pour arreter le programme proprement.. \n
  * Retour : rien
*/
void interruptHandler(int s) {
	(void)s;
	resetty();
	reset_shell_mode();
	clear();
	echo();
	timeout(0);
	curs_set(1);
	endwin();
	exit(127);
}
/** Arguments : les paramétres passés a l'executable et leur nombre. \n
  * Apres de nombreux appels visant a initialiser ncurses, l'appel a newGame créé les 
  * camps de chaque joueur. L'activation des éventuels cheat codes se fait en testant
  * l'existance d'un argument au programme, puis on créé les 3 joueurs. \n
  * On entre alors dans une boucle infinie qui invite chaque joueur a effectuer une action
  * puis on l'informe de son résultat (touché, coulé, raté). La boucle est aussi responsable 
  * du changement de tour (passage de joueur X à joueur Y....) \n
  * Enfin, si goto(end) est appélée, on affiche le joueur qui a gagné la partie, et on
  * met fin au programme avec un appel à interrupthandler() \n
  * retour : 0.
*/
int main(int argc, char *argv[argc])
{
	struct sigaction s;
	memset(&s, 0, sizeof(s)); //Alloue la mémoire
	s.sa_handler = interruptHandler; //Affecte la fonction interrupHandler a l'evenement s
	sigemptyset(&s.sa_mask);
	sigaction(SIGINT, &s, 0); //Remplace l'action de SIGINT par s
	savetty();
	initscr(); //Initialise l'ecran Ncurse
	clear(); //Nettoie l'ecran
	noecho();
	timeout(-1); //Bloque le rafraichissement en attente d'input
	curs_set(0); //Rend le curseur invisible
	refresh(); //Affiche les input

	game_state_t *game = newGame();
	if (argc >= 2) {
		game->cheat = atoi(argv[1]);
		if (game->cheat < 0 || game->cheat >= 7)
		game->cheat = -1;
	}

	{
		player_t *p = newLocalPlayer(); //Creation du joueur 1
		p->name = "Joueur 1";
		game->camp_allocator->put_in_camp(game->camp_allocator, game, p);
		p = newLocalPlayer(); //Creation du joueur 2
		p->name = "Joueur 2";
		game->camp_allocator->put_in_camp(game->camp_allocator, game, p);
		// ici pour 3e joueur
		p = newLocalPlayer();
		p->name = "Joueur 3";
		game->camp_allocator->put_in_camp(game->camp_allocator, game, p);
	}
	while (1) {
		for(unsigned i = 0; i < darraySize(game->camps); ++i) {
			camp_t *camp = *(camp_t **)darrayGet(game->camps, i);
			player_t *players = *(player_t**)darrayGet(camp->players, 0);
			unsigned n = darraySize(camp->players);
			while (n--) {
				if (players->n_boats == 0) {
					++players;
					continue;
				}
				result_t r = REDO;
				point_t coordinates;
				do {
					coordinates = players->get_action(players, game);
					r = doAction(game, players, coordinates);
					clear();
					refresh();
					switch (r) {
						case REDO:
							break;
						case HIT:
							puts("Touché!");
							break;
						case MISS:
							puts("Raté!");
							break;
						case SUNK:
							puts("Coulé!");
							break;
					}
					printf("\r");
					fflush(stdout);
				} while(r == REDO);
				if (!turnEndUpdate(game))
				goto end;
				printf("C'est le tour du joueur suivant, appuyez sur espace\n\r");
				while (getch() != 32);
				++players;
			}
		}
	}
	end:;
	player_t admin = {.owned_rect = {{0, 0}, {game->width, game->height}}};
	void *bitmap = stateToView(game, &admin);
	printColorArray(game, bitmap);
	free(bitmap);
	printf("L'équipe gagnante est { %s }\n\r", campTeamString(game->winning));


	for (unsigned i = 0; i < darraySize(game->camps); ++i) {
		camp_t *c = *(camp_t**)darrayGet(game->camps, i);
		deleteCamp(c);
	}
	darrayDelete(game->camps);
	free(game->grid);
	free(game->camp_allocator);
	free(game);
	printf("Appuyez sur une touche pour terminer\n\r");
	refresh();

	getch();
	interruptHandler(0);
	return 0;
}
