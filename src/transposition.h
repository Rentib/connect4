#ifndef TRANSPOSITION_H_
#define TRANSPOSITION_H_

#include "connect4.h"

enum tt_bound {
	TT_NONE,
	TT_UPPER,
	TT_LOWER,
	TT_EXACT,
};

void tt_init(size_t mb);
void tt_free(void);
void tt_clear(void);

bool tt_probe(u64 key, int ply, int *value, enum tt_bound *bound);
void tt_store(u64 key, int ply, int  value, enum tt_bound  bound);

#endif /* TRANSPOSITION_H_ */
