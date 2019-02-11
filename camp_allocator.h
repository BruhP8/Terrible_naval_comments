#ifndef CAMP_ALLOCATOR_H
#define CAMP_ALLOCATOR_H
typedef struct game_state_s game_state_t;
typedef struct player_s player_t;
typedef struct camp_allocator_s {
	void (*put_in_camp)(struct camp_allocator_s *, game_state_t *, player_t *);
} camp_allocator_t;
camp_allocator_t *newSingleAllocator();
#endif 
