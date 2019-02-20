#include "master_head.h"

typedef struct {
	camp_allocator_t base;
	int alloc_number;
} single_camp_allocator_t;

static void singleAllocateCamp(camp_allocator_t *base, game_state_t *game, player_t *p) {
	single_camp_allocator_t *self = (void*) base;
	camp_t *camp = newCamp();
	darrayPushBack(camp->players, &p);
	darrayPushBack(game->camps, &camp);
	int bidx = 17 * self->alloc_number;
	p->owned_rect[0] = (point_t) {
			bidx % game->width,
			bidx / game->width * 17
	};
	p->owned_rect[1] = (point_t) {
		p->owned_rect[0].x + 17,
		p->owned_rect[0].y + 17,
	};
	++self->alloc_number;
	p->setup_boats(p, game);
}
camp_allocator_t *newSingleAllocator() {
	single_camp_allocator_t *ret = calloc(1, sizeof(*ret));
	ret->base.put_in_camp = singleAllocateCamp;
	return &ret->base;
}
