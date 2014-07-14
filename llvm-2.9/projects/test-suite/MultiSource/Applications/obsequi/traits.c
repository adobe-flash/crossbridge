
#include "globals.h"
#include "macros.h"

static FILE *trait_file = NULL;


static s32bit
tr_total_non_safe_moves(s32bit player)
{
  u32bit mask1, mask2;
  s32bit num_rows = g_board_size[player];
  s32bit count = 0, i;
  
  for(i = 0; i < num_rows; i++){
    mask1 = g_board[player][i] & g_board[player][i+2];
    mask2 = ~g_board[player][i+1];
    
    mask1 = (~((mask1 >> 1) & mask1)) & ((mask2 >> 1) & mask2);

    //    printf("%X\n", mask1);
    
    count += countbits32(mask1);
  }
  
  return count;
}

static s32bit
tr_non_safe_moves_a_little_touchy(s32bit player)
{
  u32bit mask1, mask2;
  s32bit num_rows = g_board_size[player];
  s32bit count = 0, i;
  
  for(i = 0; i < num_rows; i++){
    mask1 = g_board[player][i] | g_board[player][i+2];
    mask2 = g_board[player][i+1];
    
    mask1 = ( (mask1 << 1) | mask1
              | (mask2 >> 1) | mask2 | (mask2 << 1) | (mask2 << 2) );

    count += countbits32(~mask1);
  }

  if(count == -1){
    print_board(player);
    printf("%d %d\n", player, count);
    exit(1);
  }
  
  return count;
}

static s32bit
tr_non_safe_moves_no_touchy(s32bit player)
{
  return 1;
}

static s32bit
tr_total_empty_squares()
{
  u32bit mask;
  s32bit num_rows = g_board_size[0];
  s32bit count = 0, i;
  
  for(i = 0; i < num_rows; i++){
    mask = ~(g_board[0][i+1]);

    count += countbits32(mask);
  }
    
  return count;
}

static s32bit
tr_border_length_col(player)
{
  u32bit mask;
  s32bit num_rows = g_board_size[player];
  s32bit count = 0, i;
  
  for(i = 0; i <= num_rows; i++){
    mask = g_board[player][i] ^ g_board[player][i+1];

    count += countbits32(mask);
  }
  
  return count;
}

static s32bit
tr_border_length_row(player)
{
  u32bit mask;
  s32bit num_rows = g_board_size[player];
  s32bit count = 0, i;
  
  for(i = 0; i <= num_rows; i++){
    mask  = (g_board[player][i+1]>>1) ^ g_board[player][i+1];
    mask &= 0x7FFFFFFF;
    
    count += countbits32(mask);
  }
  
  return count;
}




extern void
write_node_info(u64bit num_nodes, s32bit winner)
{
  s32bit num;
  
  if(trait_file == NULL) {
    trait_file = fopen("trait_file", "w");
    if(trait_file == NULL)
      fprintf(stderr, "Couldn't open \"trait_file\".\n");
  }
  
  fprintf(trait_file, "%c %15s :", (winner == VERTICAL) ? 'V' : 'H',
          u64bit_to_string(num_nodes));
  
  //========================================================
  // number of non-safe moves.
  //========================================================
  
  // total number allowing overlapping of other non-safe moves
  //   and of safe moves.
  num = tr_total_non_safe_moves(winner);
  fprintf(trait_file, " %2d", num);

  num = tr_total_non_safe_moves(winner^PLAYER_MASK);
  fprintf(trait_file, " %2d :", num);
  
  
  // non safe moves that don't touch a border, except diagonally.
  num = tr_non_safe_moves_a_little_touchy(winner);
  fprintf(trait_file, " %2d", num);

  num = tr_non_safe_moves_a_little_touchy(winner^PLAYER_MASK);
  fprintf(trait_file, " %2d :", num);


  // non safe moves that don't touch a border, never.
  num = tr_non_safe_moves_no_touchy(winner);
  fprintf(trait_file, " %2d", num);

  num = tr_non_safe_moves_no_touchy(winner^PLAYER_MASK);
  fprintf(trait_file, " %2d :", num);


  //========================================================
  // Miscellaneous other info.
  //========================================================

  // empty squares.
  num = tr_total_empty_squares();
  fprintf(trait_file, " %2d :", num);

  // border length.
  num = tr_border_length_col(winner);
  fprintf(trait_file, " %2d", num);

  // border length.
  num = tr_border_length_row(winner);
  fprintf(trait_file, " %2d", num);

  fprintf(trait_file, "\n");
}

