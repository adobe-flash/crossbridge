
#include "globals.h"


u32bit     g_board[2][32];
s32bit     g_board_size[2] = {-1,-1};

Basic_Info g_info[2][32];
Basic_Info g_info_totals[2];

// zobrist value for each position on the board.
s32bit       g_zobrist[32][32];

// Transposition table.
Hash_Entry  *g_trans_table = 0;

// Current boards hash key and code (flipped in various ways).
Hash_Key     norm_hashkey;
Hash_Key     flipV_hashkey;
Hash_Key     flipH_hashkey;
Hash_Key     flipVH_hashkey;


extern void
init__safe_count(s32bit player)
{
  s32bit i;
    
  g_info_totals[player].safe = 0;

  for(i = 0; i < g_board_size[player]; i++){
    g_info[player][i+1].safe = 0;
    
    update_safe(player, i+1);
  }
}

extern void
init__real_count(s32bit player)
{
  s32bit i;
    
  g_info_totals[player].real = 0;

  for(i = 0; i < g_board_size[player]; i++){
    g_info[player][i+1].real = 0;
    
    update_real(player, i+1);
  }
}

extern void
initialize_solver()
{
  s32bit i, j;

  if(g_trans_table == NULL){

    // first time initialization stuff.
    g_trans_table = calloc(HASHSIZE,sizeof(Hash_Entry));
  
    // initialize zobrist values
    srandom(1);
    for(i=1; i<31; i++) {
      for(j=1; j<31; j++) {
        g_zobrist[i][j] = random() & HASHMASK;
      }
    }

    init_static_tables();
  }

  init_less_static_tables();
}



static void
init_hashkey_code(Hash_Key* key)
{
  s32bit i, j, index, n_rows, n_cols;
  
  n_rows = g_board_size[HORIZONTAL], n_cols = g_board_size[VERTICAL];

  key->code = 0;
  
  for(i = 0; i < n_rows; i++)
    for(j = 0; j < n_cols; j++){
      index = (i*n_cols) + j;
      if(key->key[index/32] & NTH_BIT(index%32))
        key->code ^= g_zobrist[i+1][j+1];
    }
}

extern void
initialize_board(s32bit num_rows, s32bit num_cols, s32bit board[30][30])
{
  s32bit i, j;
  s32bit init = 0;
  
  if(num_rows > 30 || num_rows < 1 || num_cols > 30 || num_cols < 1)
    fatal_error(1, "Invalid board size %dX%d.\n", num_rows, num_cols);

  if(num_rows * num_cols >= 128)
    fatal_error(1, "Invalid board size %dX%d.\n", num_rows, num_cols);
  
  // Check if we need to re-initialize the solver.
  if(g_trans_table == NULL || g_board_size[HORIZONTAL] != num_rows
     ||  g_board_size[VERTICAL] != num_cols) init = 1;
  
  g_board_size[HORIZONTAL] = num_rows;
  g_board_size[VERTICAL]   = num_cols;

  if(init) initialize_solver();

  // Fill all positions on the board.
  for(i = 0; i < 32; i++){
    g_board[0][i] = ALL_BITS;
    g_board[1][i] = ALL_BITS;
  }
  
  // Clear positions where there isn't a piece.
  for(i = 0; i < num_rows; i++)
    for(j = 0; j < num_cols; j++)
      if(board[i][j] == 0){
        g_board[HORIZONTAL][i+1] &= ~(NTH_BIT(j+1));
        g_board[VERTICAL][j+1]   &= ~(NTH_BIT(i+1));
      }

  init__real_count(VERTICAL);
  init__real_count(HORIZONTAL);

  init__safe_count(VERTICAL);
  init__safe_count(HORIZONTAL);

  for(i = 0; i < 4; i++){
    g_norm_hashkey.key[i]   = 0;
    g_flipV_hashkey.key[i]  = 0;
    g_flipH_hashkey.key[i]  = 0;
    g_flipVH_hashkey.key[i] = 0;
  }

  // Modify hashkeys to deal with positions which are already occupied.
  for(i = 0; i < num_rows; i++)
    for(j = 0; j < num_cols; j++)
      if(board[i][j] != 0){
        s32bit index;

        index = (i*num_cols)+j;
        g_norm_hashkey.key[index/32] |= NTH_BIT(index%32);
        
        index = (i*num_cols)                  + (num_cols - j - 1);
        g_flipV_hashkey.key[index/32] |= NTH_BIT(index%32);
        
        index = ( (num_rows - i - 1) *num_cols) + j;
        g_flipH_hashkey.key[index/32] |= NTH_BIT(index%32);
        
        index = ( (num_rows - i - 1) *num_cols) + (num_cols - j - 1);
        g_flipVH_hashkey.key[index/32] |= NTH_BIT(index%32);
      }
  
  init_hashkey_code(&g_norm_hashkey);
  init_hashkey_code(&g_flipV_hashkey);
  init_hashkey_code(&g_flipH_hashkey);
  init_hashkey_code(&g_flipVH_hashkey);
  
  print_board(HORIZONTAL);
  printf("\n");
  print_board_info(HORIZONTAL);

  check_hash_code_sanity();
}
