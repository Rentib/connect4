#ifndef MOVEPICKER_H_
#define MOVEPICKER_H_

#include "connect4.h"
#include "position.h"
#include "search.h"

struct scored_move {
	enum move move;
	int score;
};

struct move_picker {
	struct scored_move moves[FILES];
	size_t count;
};

void mp_init(struct move_picker *move_picker, struct position *position);
enum move mp_next(struct move_picker *move_picker);

void mp_clear(void);
void mp_update(struct position *position, struct search_stack *search_stack,
	       enum move *moves, size_t count);

#endif /* MOVEPICKER_H_ */
