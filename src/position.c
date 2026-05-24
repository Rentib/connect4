#include <stdio.h>
#include <stdlib.h>

#include "bitboards.h"
#include "connect4.h"
#include "position.h"
#include "util.h"

static struct {
	u64 empty;
	u64 side;
	u64 color_square[COLOR_NB][SQUARE_NB];
} zobrist;

void pos_init(void)
{
	enum color c;
	enum square sq;

	zobrist.empty = rand_u64();
	zobrist.side = rand_u64();
	for (c = 0; c < COLOR_NB; c++) {
		for (sq = 0; sq < SQUARE_NB; sq++)
			zobrist.color_square[c][sq] = rand_u64();
	}
}

void pos_reset(struct position *pos)
{
	pos->stm = WHITE;
	pos->color[WHITE] = 0ULL;
	pos->color[BLACK] = 0ULL;
	pos->key = zobrist.empty;
	pos->key_normal = zobrist.empty;
	pos->key_mirror = zobrist.empty;
}

void pos_print(const struct position *pos)
{
	enum square sq;
	size_t rank, file;
	const char *sep = "+---+---+---+---+---+---+---+";
	char c;
	printf("  %s\n", sep);
	for (sq = 0, rank = RANKS; rank >= 1; rank--) {
		printf("%zu |", rank);
		for (file = 0; file < FILES; file++, sq++) {
			c = BB_TEST(pos->color[WHITE], sq) ? 'O'
			  : BB_TEST(pos->color[BLACK], sq) ? 'X'
							   : ' ';
			printf(" %c |", c);
		}
		printf("\n  %s\n", sep);
	}
	printf("    a   b   c   d   e   f   g\n");
	printf("    Turn: %s\n", pos->stm == WHITE ? "WHITE" : "BLACK");
	printf("    Key:  %016lX\n", pos->key);
}

void pos_do_move(struct position *pos, enum move move)
{
	const enum color us = pos->stm;

	BB_XOR(pos->color[us], move);
	pos->stm ^= 1;

	pos->key_normal ^= zobrist.side;
	pos->key_normal ^= zobrist.color_square[us][move];
	pos->key_mirror ^= zobrist.side;
	pos->key_mirror ^= zobrist.color_square[us][SQ_MIRROR(move)];
	pos->key = MIN(pos->key_normal, pos->key_mirror);
}

void pos_undo_move(struct position *pos, enum move move)
{
	const enum color us = !pos->stm;

	BB_XOR(pos->color[us], move);
	pos->stm ^= 1;

	pos->key_normal ^= zobrist.side;
	pos->key_normal ^= zobrist.color_square[us][move];
	pos->key_mirror ^= zobrist.side;
	pos->key_mirror ^= zobrist.color_square[us][SQ_MIRROR(move)];
	pos->key = MIN(pos->key_normal, pos->key_mirror);
}

INLINE bool check_loss(const u64 bb, enum direction dir)
{
	u64 b = bb;
	b = bb & bb_shift(b, dir);
	b = bb & bb_shift(b, dir);
	b = bb & bb_shift(b, dir);
	return !!b;
}

bool pos_is_loss(struct position *pos)
{
	u64 bb = pos->color[!pos->stm];
	return check_loss(bb, SOUTH) || check_loss(bb, EAST) ||
	       check_loss(bb, SOUTH_EAST) || check_loss(bb, SOUTH_WEST);
}

int pos_moves_played(struct position *pos)
{
	return BB_POPCOUNT(pos->color[WHITE] | pos->color[BLACK]);
}

enum move *pos_moves(struct position *pos, enum move *move_list)
{
	static const int STRATEGY_FILES[7] = {3, 4, 2, 5, 1, 6, 0};

	u64 bb = pos->color[WHITE] | pos->color[BLACK] | BB_RANK_0;
	bb = bb_shift(bb, NORTH) & ~bb;

	for (int i = 0; i < 7; i++) {
		int file = STRATEGY_FILES[i];
		u64 file_mask = BB_FILE_A << file;
		u64 move_bit = bb & file_mask;

		if (move_bit)
			*move_list++ = bb_poplsb(&move_bit);
	}

	return move_list;
}
