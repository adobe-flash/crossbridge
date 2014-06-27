

//########################################################
// Constants which we use.
// Any constants which we commonly change or try to tweak
//   should be in cppflags.raw.
//########################################################

// max number of empty squares which should ever exist is 256
//  therefore we could possible get close to this number of moves.
#define MAXMOVES 256

// Player stuff. (Used for example with g_board to get the current player).
#define HORIZONTAL    0
#define VERTICAL      1
#define PLAYER_MASK   1

// Used with g_board_size (to get num of rows and num of cols).
#define ROW_INDEX  HORIZONTAL
#define COL_INDEX  VERTICAL


#define LOWER   0
#define EXACT   1
#define UPPER   2


// blah
#define ERASE_NEXT_BIT  0x80000000;
#define NEXT_BIT        0x00010000;

//########################################################
// Constants used in conjuction with the transposition table
//   and the zobrist values.
//########################################################

// HASHCODEBITS should already be defined.
#ifndef HASHCODEBITS
#error "HASHCODEBITS must be defined.\n"
#endif

// It should have a value of between 0 and 31.
#if ! (HASHCODEBITS > 0 && HASHCODEBITS < 31)
#error "Invalid value for HASHCODEBITS, it should be a value from 1 - 30."
#endif

#define HASHSIZE (1 << HASHCODEBITS)
#define HASHMASK (HASHSIZE - 1)
