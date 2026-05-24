#ifndef POSITION_H_
#define POSITION_H_

#include "connect4.h"

struct position {
	enum color stm;
	u64 color[COLOR_NB];
	int game_ply;
	u64 key, key_normal, key_mirror;
};

void pos_init(void);

void pos_reset(struct position *pos);
void pos_print(const struct position *position);

void pos_do_move(struct position *position, enum move move);
void pos_undo_move(struct position *position, enum move move);

bool pos_is_loss(struct position *position);

int pos_moves_played(struct position *position);

enum move *pos_moves(struct position *position, enum move *move_list);

#endif /* POSITION_H_ */
