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
