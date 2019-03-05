#include "master_head.h"

typedef struct {
	camp_allocator_t base;
	int alloc_number;
} single_camp_allocator_t;
/** Arguments : un camp, un environnement de jeu et le joueur concerné
  * Contribue à la création de la zone de jeu du joueur p en attribuant ce joueur 
  * au nouveau camp et en ajoutant ce camp a la liste des camps de l'environnement de jeu
  * Dessine aussi la zone de jeu appartenant a ce joueur et lance la routine de 
  * placement des bateaux du joueur.
  * Retour : rien
*/
static void singleAllocateCamp(camp_allocator_t *base, game_state_t *game, player_t *p) {
	single_camp_allocator_t *self = (void*) base;
	camp_t *camp = newCamp();
	darrayPushBack(camp->players, &p);
	darrayPushBack(game->camps, &camp);
	int bidx = 25 * self->alloc_number;
	p->owned_rect[0] = (point_t) {
			bidx % game->width,
			bidx / game->width * 25
	};
	p->owned_rect[1] = (point_t) {
		p->owned_rect[0].x + 25,
		p->owned_rect[0].y + 25,
	};
	++self->alloc_number;
	p->setup_boats(p, game);
}
/** Arguments : Rien
  * Créé une nouvelle zone de jeu pour un joueur.
  * Retour : un camp_allocator_t représentant la zone de jeu d'un nouveau joueur
 */
camp_allocator_t *newSingleAllocator() {
	single_camp_allocator_t *ret = calloc(1, sizeof(*ret));
	ret->base.put_in_camp = singleAllocateCamp;
	return &ret->base;
}
