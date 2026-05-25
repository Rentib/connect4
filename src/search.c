#include <stdio.h>
#include <stdlib.h>

#include "connect4.h"
#include "movepicker.h"
#include "position.h"
#include "search.h"
#include "transposition.h"
#include "util.h"

static u64 time_start;

static void print_info(int value, struct search_stack *ss, enum move move)
{
	size_t ply;
	u64 time = gettime() - time_start;

	value = value > 0 ? +(MATE - value + 1) / 2 : -(MATE + value + 1) / 2;

	printf("info ");
	if (move != MOVE_NONE)
		printf("rootmove %s ", MOVE_STR(move));
	printf("score %d ", value);
	printf("nodes %zu ", ss->nodes);
	printf("time %zu ", time);
	printf("pv ");
	for (ply = 0; ss->pv[ply] != MOVE_NONE; ply++)
		printf("%s", MOVE_STR(ss->pv[ply]));
	printf("\n");
}

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
	bool pvnode = beta - alpha != 1;
	bool isroot = ss->ply == 0;
	int origalpha = alpha;
	enum move move, bestmove = MOVE_NONE;
	int value, bestvalue = -MATE;
	int tt_value;
	bool tt_hit;
	enum tt_bound tt_bound;
	struct move_picker mp;
	enum move moves[FILES];
	size_t movecount = 0;

	ss->nodes = 1;

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

	ss->pv[0] = MOVE_NONE;
	(ss + 1)->pv[0] = MOVE_NONE;
	ss->move = MOVE_NONE;
	(ss + 1)->killer = MOVE_NONE;

	mp_init(&mp, pos, ss->killer);
	while ((move = mp_next(&mp)) != MOVE_NONE) {
		moves[movecount++] = move;
		ss->move = move;
		pos_do_move(pos, move);

		if (!pvnode || movecount > 1)
			value = -negamax(pos, ss + 1, -(alpha + 1), -alpha);
		if (pvnode && (movecount == 1 || value > alpha))
			value = -negamax(pos, ss + 1, -beta, -alpha);

		pos_undo_move(pos, move);

		if (isroot && pvnode)
			print_info(value, ss + 1, move);
		ss->nodes += (ss + 1)->nodes;

		if (value <= bestvalue)
			continue;

		bestvalue = value;
		bestmove = move;

		if (value >= beta) {
			ss->killer = move;
			break;
		}

		if (value > alpha) {
			alpha = value;
			update_pv(ss, bestmove);
		}
	}

	if (bestvalue >= beta)
		mp_update(pos, ss, moves, movecount);

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
	int low = -MATE, high = +MATE, bound, value = low;
	enum move bestmove = MOVE_NONE;

	mp_clear();
	tt_clear();

	for (ply = 0; ply < MAX_MOVES; ply++) {
		(ss + ply)->ply = ply;
		(ss + ply)->nodes = 0;
		(ss + ply)->pv = ecalloc(MAX_MOVES - ply, sizeof(enum move));
		(ss + ply)->move = MOVE_NONE;
		(ss + ply)->killer = MOVE_NONE;
	}

	time_start = gettime();
	*ss->pv = MOVE_NONE;

	/* NegaC* */
	while (low < high) {
		bound = (low + high) / 2;
		value = negamax(pos, ss, bound, bound + 1);
		if (value > bound)
			low = value;
		else
			high = value;
		print_info(value, ss, MOVE_NONE);
	}

	/* PVS */
	value = negamax(pos, ss, value - 1, value + 1);

	bestmove = *ss->pv;
	print_info(value, ss, MOVE_NONE);

	for (ply = 0; ply < MAX_MOVES; ply++)
		free((ss + ply)->pv);

	printf("bestmove %s\n", MOVE_STR(bestmove));
}
