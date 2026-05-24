#ifndef BITBOARDS_H_
#define BITBOARDS_H_

#include "connect4.h"

#define BB_TEST(bitboard, square)  (((bitboard) >> (square)) & 1)
#define BB_SET(bitboard, square)   ((bitboard) |= (1ULL << (square)))
#define BB_RESET(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define BB_XOR(bitboard, square)   ((bitboard) ^= (1ULL << (square)))
#define BB_SWAP(bitboard)          ((bitboard) = __builtin_bswap64(bitboard))
#define BB_POPCOUNT(bitboard)      (__builtin_popcountll(bitboard))
#define BB_LSB(bitboard)           (__builtin_ctzll(bitboard))
#define BB_MSB(bitboard)           (__builtin_clzll(bitboard) ^ 63)
#define BB_SINGLE(bitboard)        (!((bitboard) & ((bitboard) - 1)))
#define BB_SEVERAL(bitboard)       (!BB_SINGLE(bitboard))

#define BB_FROM_SQUARE(square) (1ULL << (square))
#define BB_TO_SQUARE(bitboard) (__builtin_ctzll(bitboard))

constexpr u64 BB_RANK_6 = 0x7F;
constexpr u64 BB_RANK_5 = BB_RANK_6 << FILES;
constexpr u64 BB_RANK_4 = BB_RANK_5 << FILES;
constexpr u64 BB_RANK_3 = BB_RANK_4 << FILES;
constexpr u64 BB_RANK_2 = BB_RANK_3 << FILES;
constexpr u64 BB_RANK_1 = BB_RANK_2 << FILES;
constexpr u64 BB_RANK_0 = BB_RANK_1 << FILES;

constexpr u64 BB_FILE_A = 0x810204081;
constexpr u64 BB_FILE_B = BB_FILE_A << 1;
constexpr u64 BB_FILE_C = BB_FILE_B << 1;
constexpr u64 BB_FILE_D = BB_FILE_C << 1;
constexpr u64 BB_FILE_E = BB_FILE_D << 1;
constexpr u64 BB_FILE_F = BB_FILE_E << 1;
constexpr u64 BB_FILE_G = BB_FILE_F << 1;

void bb_print(u64 bitboard);

INLINE u64 bb_shift(u64 bitboard, enum direction direction)
{
#define gen_shift(bb, dir) ((dir) < 0 ? ((bb) >> -(dir)) : (bb) << (dir))
	switch (direction) {
	case NORTH:       return gen_shift(bitboard, direction);
	case SOUTH:       return gen_shift(bitboard, direction);
	case EAST:        return gen_shift(bitboard & ~BB_FILE_G, direction);
	case WEST:        return gen_shift(bitboard & ~BB_FILE_A, direction);
	case NORTH_EAST:  return gen_shift(bitboard & ~BB_FILE_G, direction);
	case NORTH_WEST:  return gen_shift(bitboard & ~BB_FILE_A, direction);
	case SOUTH_EAST:  return gen_shift(bitboard & ~BB_FILE_G, direction);
	case SOUTH_WEST:  return gen_shift(bitboard & ~BB_FILE_A, direction);
	case NORTH_NORTH: return gen_shift(bitboard, direction);
	case SOUTH_SOUTH: return gen_shift(bitboard, direction);
	default:          __builtin_unreachable();
	}
}

INLINE enum square bb_poplsb(u64 *bitboard)
{
	enum square square = BB_LSB(*bitboard);
	BB_XOR(*bitboard, square);
	return square;
}

INLINE enum square bb_popmsb(u64 *bitboard)
{
	enum square square = BB_MSB(*bitboard);
	BB_XOR(*bitboard, square);
	return square;
}

#endif /* BITBOARDS_H_ */
