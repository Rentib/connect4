#ifndef CONNECT4_H_
#define CONNECT4_H_

#include <stddef.h>
#include <stdint.h>

enum color { WHITE, BLACK, COLOR_NB };

constexpr size_t RANKS = 6;
constexpr size_t FILES = 7;

/* clang-format off */
enum square {
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6,
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5,
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4,
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3,
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2,
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1,
	SQUARE_NB, SQ_NONE,
};
/* clang-format on */

#define SQ_FILE(square) ((square) % FILES)
#define SQ_RANK(square) ((square) / FILES)
#define SQ_MIRROR(square)                                                      \
	((SQ_RANK(square) * FILES) + (FILES - 1 - SQ_FILE(square)))
#define SQ_STR(square)                                                         \
	((square) == SQ_NONE ? "NO"                                            \
			     : (char[]){'a' + SQ_FILE(square),                 \
					'0' + RANKS - SQ_RANK(square), '\0'})

enum direction {
	NORTH = -(int)FILES,
	SOUTH = +(int)FILES,
	EAST = +1,
	WEST = -1,
	NORTH_EAST = NORTH + EAST,
	NORTH_WEST = NORTH + WEST,
	SOUTH_EAST = SOUTH + EAST,
	SOUTH_WEST = SOUTH + WEST,
	NORTH_NORTH = NORTH + NORTH,
	SOUTH_SOUTH = SOUTH + SOUTH,
};

typedef uint64_t u64;

enum move : uint16_t {
	MOVE_NONE = SQ_NONE,
};

#define MOVE_STR(move)                                                         \
	((move) == MOVE_NONE                                                   \
	     ? "none"                                                          \
	     : (char[]){'1' + SQ_FILE((enum square)move), '\0'})

#define ABS(X)    ((X) < 0 ? -(X) : (X))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

constexpr int MAX_MOVES = FILES * RANKS;
constexpr int MATE = MAX_MOVES;

#define MATE_IN(ply)   (+MATE - (ply))
#define MATED_IN(ply)  (-MATE + (ply))
#define IS_MATE(value) (ABS(value) >= MATE - MAX_MOVES)

#define INLINE static inline __attribute__((always_inline))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#endif /* CONNECT4_H_ */
