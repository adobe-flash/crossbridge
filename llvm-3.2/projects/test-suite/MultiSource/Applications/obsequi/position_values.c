

#include "globals.h"

//#################################################################
// Give all the positions for this PLAYER which are symmetrical
//  to the position (ROW, COL) a value of VALUE.
//#################################################################
static void
set_position_value(s32bit player, s32bit row, s32bit col, s32bit value)
{
  s32bit num_rows, num_cols;
  
  num_rows = g_board_size[player] + 1;
  num_cols = g_board_size[player^PLAYER_MASK];

  //printf("1 - %d %d %d\n", player, row, col);
  g_first_move[player][row][col]                   = value;
  //printf("2 - %d %d %d\n", player, num_rows-row, col);
  g_first_move[player][num_rows-row][col]          = value;
  //printf("3 - %d %d %d\n", player, num_rows-row, num_cols-col);
  g_first_move[player][num_rows-row][num_cols-col] = value;
  //  printf("4 - %d %d %d\n", player, row, num_cols-col);
  g_first_move[player][row][num_cols-col]          = value;
}


#ifdef DYNAMIC_POSITION_VALUES
//#################################################################
// If we are using dynamic position values then these are the
//  functions (and variables) which we use to set these values.
//#################################################################

//=================================================================
// Variable to keep track of value we will give to the next position.
//=================================================================
static s32bit set_move_value_current = 127;

//=================================================================
// Initialize all positions with a value of 0
//=================================================================
extern void
init_move_value     ()
{
  s32bit i, j, k;
  set_move_value_current = 127;
  
  for(i = 0; i < 2; i++)
    for(j = 0; j < 32; j++)
      for(k = 0; k < 32; k++)
        g_first_move[i][j][k] = 0;
}

//=================================================================
// Set the value of the positions symetrical to MOVE.
//  return 0 if these positions already have a value, otherwise 1.
//=================================================================
extern s32bit
set_move_value      (Move move, s32bit player)
{
  if(g_first_move[player][move.array_index][move.mask_index] != 0)
    return 0;
  
  set_position_value(player, move.array_index, move.mask_index,
                     set_move_value_current--);
  return 1;
}

//=================================================================
// UnSet the value of the positions symetrical to MOVE.
//  Shouldn't be called if when 'set' was called it returned a value of 0.
//=================================================================
extern void
unset_move_value    (Move move, s32bit player)
{
  set_move_value_current
    = g_first_move[player][move.array_index][move.mask_index];
  
  set_position_value(player, move.array_index, move.mask_index, 0);
}

#else
//#################################################################
// If we are using dynamic position values then these are the
//  functions which we use to set these values.
//#################################################################

//=================================================================
// Set the value of all the positions on the board.
//  We only do this once at the start of the search.
//=================================================================
extern void
set_position_values()
{
  s32bit i, j, k, count;
  
  // set them all to zero.
  for(i = 0; i < 2; i++)
    for(j = 0; j < 32; j++)
      for(k = 0; k < 32; k++)
        g_first_move[i][j][k] = 0;

#if 0
  // Set values using very simple scheme.
  for(i = 0; i < 2; i++){
    count = 64;
    for(j = 1; j < (g_board_size[i]+3)/2; j++){
      for(k = 1; k < (g_board_size[i^PLAYER_MASK]/2)+1; k++){
        if(g_first_move[i][j][k] == 0)
          set_position_value(i, j, k, --count);
      }
    }
  }
#elif 1
  // Set values using a more complex (seems to be better) scheme.
  for(i = 0; i < 2; i++){
    count = 127;
    for(j = 2; j < (g_board_size[i]+3)/2; j+=2){
      for(k = 1; k < (g_board_size[i^PLAYER_MASK]/2)+1; k+=2){
        set_position_value(i, j, k, --count);
      }
    }
  }
  for(i = 0; i < 2; i++){
    count = 90;
    for(j = 2; j < (g_board_size[i]+3)/2; j+=2){
      for(k = 1; k < (g_board_size[i^PLAYER_MASK]/2)+1; k++){
        if(g_first_move[i][j][k] == 0)
          set_position_value(i, j, k, --count);
      }
    }
  }
  for(i = 0; i < 2; i++){
    count = 70;
    for(j = 3; j < (g_board_size[i]+3)/2; j++){
      for(k = 1; k < (g_board_size[i^PLAYER_MASK]/2)+1; k+=2){
        if(g_first_move[i][j][k] == 0)
          set_position_value(i, j, k, --count);
      }
    }
  }
  for(i = 0; i < 2; i++){
    count = 50;
    for(j = 3; j < (g_board_size[i]+3)/2; j++){
      for(k = 1; k < (g_board_size[i^PLAYER_MASK]/2)+1; k++){
        if(g_first_move[i][j][k] == 0)
          set_position_value(i, j, k, --count);
      }
    }
  }
  for(i = 0; i < 2; i++){
    count = 30;
    for(j = 1; j < (g_board_size[i]+3)/2; j++){
      for(k = 1; k < (g_board_size[i^PLAYER_MASK]/2)+1; k++){
        if(g_first_move[i][j][k] == 0)
          set_position_value(i, j, k, --count);
      }
    }
  }
#endif

  // print the values. (mostly for debugging)
#if 0
  for(i = 0; i < 2; i++){
    for(j = 0; j < 32; j++){
      for(k = 0; k < 32; k++){
        printf("%d ", g_first_move[i][j][k]);
      }
      printf("\n");
    }
    printf("\n");
  }
  exit(0);
#endif
  
}

#endif
