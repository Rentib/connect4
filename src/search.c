#include <stdio.h>
#include <stdlib.h>

#include "connect4.h"
#include "position.h"
#include "search.h"
#include "transposition.h"
#include "util.h"

struct search_stack {
	int ply;
	enum move move;
	enum move *pv;
};

static u64 nodes;
static u64 time_start;

static void update_pv(struct search_stack *ss, enum move move)
{
	size_t i;
	ss->pv[0] = move;
	for (i = 0; (ss + 1)->pv[i] != MOVE_NONE; i++)
		ss->pv[i + 1] = (ss + 1)->pv[i];
	ss->pv[i + 1] = MOVE_NONE;
}

static int negamax(struct position *pos, struct search_stack *ss, int alpha,
		   int beta)
{
	enum move move_list[FILES], *last, *m;

	bool pvnode = beta - alpha != 1;
	int origalpha = alpha;
	enum move move, bestmove = MOVE_NONE;
	int value, bestvalue = -MATE;
	int tt_value;
	bool tt_hit;
	enum tt_bound tt_bound;
	size_t movecount = 0;

	nodes++;

	if (pos_moves_played(pos) >= MATE - 2)
		return 0;

	if (pos_is_loss(pos))
		return MATED_IN(ss->ply);

	alpha = MAX(MATED_IN(ss->ply), alpha);
	beta = MIN(MATE_IN(ss->ply + 1), beta);
	if (alpha >= beta)
		return alpha;

	tt_hit = tt_probe(pos->key, ss->ply, &tt_value, &tt_bound);
	if (!pvnode && tt_hit &&
	    ((tt_bound == TT_EXACT) ||
	     (tt_bound == TT_LOWER && tt_value >= beta) ||
	     (tt_bound == TT_UPPER && tt_value <= alpha)))
		return tt_value;

	ss->move = MOVE_NONE;

	ss->pv[0] = MOVE_NONE;
	(ss + 1)->pv[0] = MOVE_NONE;
	last = pos_moves(pos, move_list);

	/* mate in one */
	for (m = move_list; bestvalue < beta && m != last; m++) {
		move = *m;
		movecount++;

		pos_do_move(pos, move);
		if (pos_is_loss(pos)) {
			bestmove = move;
			bestvalue = MATE_IN(ss->ply + 1);
			if (bestvalue > alpha) {
				alpha = bestvalue;
				update_pv(ss, bestmove);
			}
		}
		pos_undo_move(pos, move);
	}

	for (m = move_list; bestvalue < beta && m != last; m++) {
		move = *m;

		ss->move = move;
		pos_do_move(pos, move);

		if (!pvnode || movecount > 1)
			value = -negamax(pos, ss + 1, -(alpha + 1), -alpha);
		if (pvnode && (movecount == 1 || value > alpha))
			value = -negamax(pos, ss + 1, -beta, -alpha);

		pos_undo_move(pos, move);

		if (value <= bestvalue)
			continue;

		bestvalue = value;
		bestmove = move;

		if (value >= beta)
			break;

		if (value > alpha) {
			alpha = value;
			update_pv(ss, bestmove);
		}
	}

	if (movecount == 0) /* no legal moves */
		bestvalue = 0;

	tt_bound = bestvalue <= origalpha ? TT_UPPER
		 : bestvalue >= beta      ? TT_LOWER
					  : TT_EXACT;
	tt_store(pos->key, ss->ply, bestvalue, tt_bound);

	return bestvalue;
}

void search(struct position *pos)
{
	struct search_stack search_stack[MAX_MOVES + 2];
	struct search_stack *ss = search_stack + 2;
	size_t ply;
	int value;
	enum move bestmove = MOVE_NONE;

	tt_clear();

	for (ply = 0; ply < MAX_MOVES; ply++) {
		(ss + ply)->ply = ply;
		(ss + ply)->move = MOVE_NONE;
		(ss + ply)->pv = ecalloc(MAX_MOVES - ply, sizeof(enum move));
	}

	nodes = 0;
	time_start = gettime();
	*ss->pv = MOVE_NONE;

	value = negamax(pos, ss, -MATE, +MATE);

	bestmove = *ss->pv;
	value = value > 0 ? +(MATE - value + 1) / 2 : -(MATE + value + 1) / 2;

	printf("info ");
	printf("score %d ", value);
	printf("nodes %zu ", nodes);
	printf("time %zu ", gettime() - time_start);
	printf("pv ");
	for (ply = 0; ss->pv[ply] != MOVE_NONE; ply++)
		printf("%s", MOVE_STR(ss->pv[ply]));
	printf("\n");

	for (ply = 0; ply < MAX_MOVES; ply++)
		free((ss + ply)->pv);

	printf("bestmove %s\n", MOVE_STR(bestmove));
}
