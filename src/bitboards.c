#include <inttypes.h>
#include <stdio.h>

#include "bitboards.h"
#include "connect4.h"

void bb_print(u64 bb)
{
	size_t rank, file;
	enum square sq = SQ_A6;
	char c;
	const char *sep = "+---+---+---+---+---+---+---+";
	printf("  %s\n", sep);
	for (rank = RANKS; rank >= 1; rank--) {
		printf("%zu |", rank);
		for (file = 0; file < FILES; file++) {
			c = BB_TEST(bb, sq++) ? 'X' : ' ';
			printf(" \033[31;1m%c\033[00;0m |", c);
		}
		printf("\n  %s\n", sep);
	}
	printf("    a   b   c   d   e   f   g\n");
	printf("  Bitboard: %016" PRIx64 "\n", bb);
}
