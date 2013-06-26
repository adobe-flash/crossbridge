
#ifndef OBSEQUI_INTERFACE_H
#define OBSEQUI_INTERFACE_H

//########################################################
// 
//########################################################
void   print_external();

//########################################################
// This function initializes the solver.
//
// This includes things like allocating and zeroing data
//   for the hashtable.
//
// If it is called a second time it will re-zero all the
//   data, (restart from scratch).
//########################################################
void   initialize_solver ();


//########################################################
// This function gives the solver the current board that
//   we want to work with.
//########################################################
void   initialize_board  (s32bit row, s32bit col,
                          s32bit board[30][30]);


//########################################################
// This function tries to find the best move for 'player'.
//
//  - player is either 'V' or 'H'.
//########################################################
s32bit search_for_move   (char player,
                          s32bit *row, s32bit *col, u64bit *nodes);


//########################################################
// This function returns a string about the current
//   state of the search.
//########################################################
const char*  current_search_state();

#endif
