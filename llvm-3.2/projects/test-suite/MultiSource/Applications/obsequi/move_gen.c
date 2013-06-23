
#include "globals.h"
#include "macros.h"

//#################################################################
// The following functions are used to generate all the possible
//   moves which could be made, for a certain player, given
//   the current board position.
//#################################################################

//=================================================================
// This function generates all the moves in one pass.
//=================================================================
extern s32bit
move_generator(Move movelist[MAXMOVES], s32bit player)
{
  s32bit i, count = 0, player_index;
  u32bit prot_rows, curr_row, m, tmp;

  // Will be 0 if horizontal, 1 if vertical.
  // Appears to be redundant but seems to help the compiler optimize.
  player_index = player&PLAYER_MASK;
  
  for(i = 0; i < g_board_size[player_index]; i++){
    prot_rows = g_board[player_index][i] & g_board[player_index][i+2];
    curr_row  = g_board[player_index][i+1];
    
#ifndef PLAY_SAFE_MOVES
    // m contains a 1 at each position where there is valid move.
    //    (except safe moves, there is no need to play them)
    m = ~((curr_row|(curr_row>>1)) | (prot_rows&(prot_rows>>1)));
#else
    // if you uncomment this it will also play safe moves.
    m = ~(curr_row|(curr_row>>1));
#endif    

    while(m){
      tmp = (m&-m); // least sig bit of m
      m  ^= tmp;    // remove least sig bit of m.
      movelist[count].mask_index  = lastbit32(tmp);
      movelist[count].array_index = i+1;
      movelist[count].info        = 0;
      count++;
    }
  }
  
  return count;
}

//=================================================================
// The following functions use a two pass system. This means possibly
//   less cost in evaluating (because if we find a cutoff quickly
//   we will not have to evaluate the second half) and in sorting the
//   moves.
//=================================================================
extern s32bit
move_generator_stage1(Move movelist[MAXMOVES], s32bit player)
{
  s32bit i, count = 0;
  s32bit player_index;
  
  u32bit prot_rows, curr_row, m, tmp;

  // will be 0 if horizontal, 1 if vertical.
  // This appears to be redundant but it seems to helps the compiler
  //  optimize.
  player_index = player&PLAYER_MASK;
  
  for(i = 0; i < g_board_size[player_index]; i++){
    prot_rows = g_board[player_index][i] & g_board[player_index][i+2];
    curr_row  = g_board[player_index][i+1];
    
    // m will contain a 1 at each position that there is a move
    //   which is a vulnerable move with no protected squares.
    m = ~((curr_row|(curr_row>>1)) | (prot_rows|(prot_rows>>1)));
    
    while(m){
      tmp = (m&-m); // least sig bit of m
      m ^= tmp;     // remove least sig bit of m.
      movelist[count].mask_index  = lastbit32(tmp);
      movelist[count].array_index = i+1;
      movelist[count].info        = 0;
      count++;
    }
  }

  return count;
}

extern s32bit
move_generator_stage2(Move movelist[MAXMOVES], s32bit count, s32bit player)
{
  s32bit i, player_index;
  u32bit prot_rows, curr_row, m, tmp;

  // will be 0 if horizontal, 1 if vertical.
  // This appears to be redundant but it seems to helps the compiler
  //  optimize.
  player_index = player&PLAYER_MASK;
  
  for(i = 0; i < g_board_size[player_index]; i++){
    prot_rows = g_board[player_index][i] & g_board[player_index][i+2];
    curr_row  = g_board[player_index][i+1];
    
#ifndef PLAY_SAFE_MOVES
    // m will contain a 1 at each position that there is a move
    //   which is a vulnerable move with a protected squares.
    m = ~((curr_row|(curr_row>>1)) | (~(prot_rows^(prot_rows>>1))) );
#else
    // m will contain a 1 at each position that there is a move
    //   which is a vulnerable move with a protected squares.
    m = ((~((curr_row|(curr_row>>1)) | (prot_rows|(prot_rows>>1))))
         ^ (~(curr_row|(curr_row>>1))));
#endif
    
    while(m){
      tmp = (m&-m); // least sig bit of m
      m ^= tmp;     // remove least sig bit of m.
      movelist[count].mask_index  = lastbit32(tmp);
      movelist[count].array_index = i+1;
      movelist[count].info        = 0;
      count++;
    }
  }

  return count;
}
