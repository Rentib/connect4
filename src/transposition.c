#include <stdlib.h>
#include <string.h>

#include "connect4.h"
#include "transposition.h"
#include "util.h"

struct __attribute__((packed)) tt_entry {
	u64 key;
	int8_t value;
	enum tt_bound bound : 2;
};

constexpr size_t MEBIBYTE = 1ULL << 20;

static struct {
	struct tt_entry *entries;
	size_t mask;
} tt = {nullptr, 0};

INLINE int value_from(int value, int ply)
{
	return IS_MATE(value) ? value > 0 ? value - ply : value + ply : value;
}

INLINE int value_to(int value, int ply)
{
	return IS_MATE(value) ? value > 0 ? value + ply : value - ply : value;
}

void tt_init(size_t mb)
{
	size_t keysize = 16;

	if (tt.mask)
		free(tt.entries);

	while ((1ull << keysize) * sizeof(struct tt_entry) <= mb * MEBIBYTE / 2)
		keysize++;

	tt.entries = ecalloc(1ull << keysize, sizeof(struct tt_entry));
	tt.mask = (1ull << keysize) - 1u;

	tt_clear();
}

void tt_free(void)
{
	if (tt.mask)
		free(tt.entries);
	tt.mask = 0;
}

void tt_clear(void)
{
	memset(tt.entries, 0, (tt.mask + 1) * sizeof(struct tt_entry));
}

bool tt_probe(u64 key, int ply, int *value, enum tt_bound *bound)
{
	struct tt_entry *et = &tt.entries[key & tt.mask];

	if (et->key == key) {
		*value = value_from(et->value, ply);
		*bound = et->bound;

		return true;
	}

	return false;
}

void tt_store(u64 key, int ply, int value, enum tt_bound bound)
{
	struct tt_entry *et = &tt.entries[key & tt.mask];

	et->key = key;
	et->value = value_to(value, ply);
	et->bound = bound;
}

size_t tt_hashfull(void)
{
	size_t used = 0, i;
	for (i = 0; i < 1000; i++)
		used += tt.entries[i].bound != TT_NONE;
	return used;
}
