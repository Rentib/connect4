#include <stdio.h>
#include <string.h>

#include "connect4.h"
#include "movepicker.h"
#include "position.h"

static int history[COLOR_NB][SQUARE_NB];

constexpr int MAX_SCORE = 1 << 16;

void mp_init(struct move_picker *mp, struct position *pos, enum move killer)
{
	enum move moves[FILES], *last, *m;
	struct scored_move *move;
	bool is_mate;
	int bonus;

	last = pos_moves(pos, moves);

	mp->count = 0;
	for (m = moves; m != last; m++) {
		move = mp->moves + mp->count++;
		move->move = *m;

		pos_do_move(pos, *m);
		is_mate = pos_is_loss(pos);
		pos_undo_move(pos, *m);

		if (is_mate) {
			move->score = MAX_SCORE << 2;
		} else if (*m == killer) {
			move->score = MAX_SCORE << 1;
		} else {
			bonus = FILES - ABS((int)SQ_FILE(*m) - 3);
			bonus = MAX_SCORE / (FILES * FILES) * (bonus * bonus);
			move->score = bonus + history[pos->stm][*m];
		}
	}
}

enum move mp_next(struct move_picker *mp)
{
	size_t i, best = 0;
	enum move bestmove;

	if (mp->count == 0)
		return MOVE_NONE;

	for (i = 1; i < mp->count; i++)
		if (mp->moves[i].score > mp->moves[best].score)
			best = i;

	bestmove = mp->moves[best].move;

	mp->moves[best] = mp->moves[--mp->count];

	return bestmove;
}

void mp_clear(void) { memset(history, 0, sizeof(history)); }

static void update_history(int *score, int delta)
{
	*score += delta - *score * ABS(delta) / MAX_SCORE;
}

void mp_update(struct position *pos, struct search_stack *ss, enum move *moves,
	       size_t count)
{
	size_t i;
	int depth = MAX_MOVES - ss->ply;

	for (i = 0; i < count - 1; i++)
		update_history(&history[pos->stm][moves[i]], -depth);
	update_history(&history[pos->stm][moves[i]], depth * depth);
}
