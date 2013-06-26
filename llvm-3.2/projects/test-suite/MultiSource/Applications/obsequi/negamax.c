
#include "globals.h"
#include "macros.h"

#include <time.h>
#include <ctype.h>


//#################################################################
// Statistics gathering variables and functions.
//#################################################################

//Determines if we should collect this data.
#define COLLECT_STATS

//=================================================================
// Variables used for statistics gathering.
//=================================================================
#ifdef COLLECT_STATS
static s32bit cut1 = 0, cut2 = 0, cut3 = 0, cut4 = 0;
static s32bit stat_cutoffs[40];
static s32bit stat_nodes[40];
static s32bit stat_nth_try[40][10];
#endif

//=================================================================
// Print the statistics which we have gathered.
//=================================================================
static void
print_stats()
{
#ifdef COLLECT_STATS
  s32bit i, j;
  
  printf("%d %d %d %d.\n\n", cut1, cut2, cut3, cut4);

  for(i = 0; i < 40; i++){
    if(stat_cutoffs[i] != 0 || stat_nodes[i] != 0){
      printf("cutoffs depth %d: (%d) %d -",
             i, stat_nodes[i], stat_cutoffs[i]);
      for(j = 0; j < 5; j++)
        printf(" %d", stat_nth_try[i][j]);
      printf(" >%d.\n", stat_nth_try[i][5]);
    }
  }
#else
  printf("\n");
#endif
}

//=================================================================
// Initialize the statistical variables.
//=================================================================
static void
init_stats()
{
#ifdef COLLECT_STATS
  s32bit i, j;

  // zero all data.
  for(i = 0; i < 40; i++){
    for(j = 0; j < 6; j++)
      stat_nth_try[i][j] = 0;
    stat_cutoffs[i] = 0;
    stat_nodes[i] = 0;
  }
#endif
}


//#################################################################
// Other variables.
//#################################################################

//#define RECORD_MOVES
//#define DEBUG_NEGAMAX


extern s32bit  debug_score_move;

Hash_Key       g_norm_hashkey;
Hash_Key       g_flipV_hashkey;
Hash_Key       g_flipH_hashkey;
Hash_Key       g_flipVH_hashkey;

u64bit         g_num_nodes;

s32bit         g_first_move[2][32][32];
s32bit         g_empty_squares = 0;

s32bit         g_move_number[128];

#ifdef RECORD_MOVES
static s32bit  g_move_player[128];
static Move    g_move_position[128];
#endif

s32bit         g_print = 0;


static s32bit starting_depth;


//#################################################################
// Negamax and driver functions. (and function prototype).
//#################################################################
static s32bit
negamax(s32bit depth_remaining, s32bit whos_turn_t, s32bit alpha, s32bit beta);


//=================================================================
// Search for move function. (Negamax Driver)
//=================================================================
extern s32bit
search_for_move(char dir, s32bit *row, s32bit *col, u64bit *nodes)
{
  s32bit  d, i, value = 0, num_moves;
  Move    movelist[MAXMOVES];
  s32bit  whos_turn;
  Move    forcefirst;
    
  // Set who's turn it is.
  if(toupper(dir) == 'V')      whos_turn = VERTICAL;
  else if(toupper(dir) == 'H') whos_turn = HORIZONTAL;
  else { fatal_error(1, "Invalid player.\n"); exit(1); }
  
  // initialize the number of empty squares.
  g_empty_squares = 0;
  for(i = 0; i < g_board_size[0]; i++)
    g_empty_squares += countbits32( (~g_board[0][i+1]) );
  
  // zero out all the statistics variables.
  init_stats();
  
  // Can we already determine a winner?
  {
    s32bit opponent = whos_turn ^ PLAYER_MASK;
    
    // stop search if game over.
    if(g_info_totals[whos_turn].safe > g_info_totals[opponent].real){
      // current player wins.
      *col = *row = -1;
      *nodes = 0;
      return 5000;
    }
    
    if(g_info_totals[opponent].safe >= g_info_totals[whos_turn].real){
      // opponent wins.
      *col = *row = -1;
      *nodes = 0;
      return -5000;
    }
  }
  
  // generate all possible moves for current player given current position.
  num_moves = move_generator(movelist, whos_turn);

  // This should never happen.
  if(num_moves == 0) fatal_error(1, "No moves");

  // should possibly sort the whole list instead of just get first.
  forcefirst.array_index = -1;
  score_and_get_first(movelist, num_moves, whos_turn, forcefirst);
  sort_moves(movelist, 1, num_moves);

  // Really this is for iterative deepening.
  for(d = 1; d < 50; d += 44){

    // Initialize alpha and beta.
    s32bit alpha = -5000, beta = 5000;

    // Re-initialize the statistics for each iteration.
    g_num_nodes = 0;
    init_stats();

    // set what the starting max depth is.
    starting_depth = d;
        
    // iterate through all the possible moves.
    for(i = 0; i < num_moves; i++){

#ifdef DYNAMIC_POSITION_VALUES
      init_move_value();
      set_move_value(movelist[i], whos_turn);
#else
      set_position_values();
#endif
      
      g_move_number[0] = i;
#ifdef RECORD_MOVES
      g_move_player[0] = whos_turn;
      g_move_position[0] = movelist[i];
#endif
      
      g_empty_squares -= 2;
      toggle_move(movelist[i], whos_turn);
      toggle_hash_code
    (g_keyinfo[whos_turn][movelist[i].array_index][movelist[i].mask_index]);
      check_hash_code_sanity();
      
      value = -negamax(d-1, whos_turn^PLAYER_MASK, -beta, -alpha);
      
#ifdef DYNAMIC_POSITION_VALUES
      unset_move_value(movelist[i], whos_turn);
#endif

      g_empty_squares += 2;
      toggle_move(movelist[i], whos_turn);
      toggle_hash_code
    (g_keyinfo[whos_turn][movelist[i].array_index][movelist[i].mask_index]);
      check_hash_code_sanity();

      printf("Move (%d,%d), value %d: %s.\n",
             movelist[i].array_index, movelist[i].mask_index,
             value, u64bit_to_string(g_num_nodes));
      printf("alpha %d, beta %d.\n", alpha, beta);

      movelist[i].info = value;

      if(value >= beta){
        alpha = value;
        break;
      }
      if(value > alpha) {
        alpha = value;
      }
    }

    if(value >= 5000){
      printf("Winner found: %d.\n", value);
      if(whos_turn == HORIZONTAL){
        *row = movelist[i].array_index;
        *col = movelist[i].mask_index;
      } else if(whos_turn == VERTICAL){
        *col = movelist[i].array_index;
        *row = movelist[i].mask_index;
      } else {
        fatal_error(1, "oops.");
      }
      
      *nodes = g_num_nodes;

      print_stats();
      
      return value;
    }

    // remove lossing moves from movelist.
    {
      s32bit rem = 0;
      for(i = 0; i < num_moves; i++){
        if(movelist[i].info <= -5000) rem++;
        else if(rem > 0) movelist[i-rem] = movelist[i];
      }
      num_moves -= rem;
      /*
      for(i = 0; i < num_moves; i++){
        printf("(%d,%d): %d.\n",
               movelist[i].array_index, movelist[i].mask_index,
               movelist[i].info);
      }
      */
    }

    print_stats();
    
    if(num_moves == 0){
      break;
    }
    
    // use a stable sort algorithm
    {
      Move swp;
      s32bit max, index, j;
      
      for(i=0; i<num_moves; i++) {
        max = movelist[i].info;
        index = i;
        
        for(j=i+1; j < num_moves; j++)
          if(movelist[j].info > max){
            max = movelist[j].info;
            index = j;
          }
    
        if(index != i){
          swp = movelist[index];
          //          printf("%d %d\n", index, i);
          for(j = index; j != i; j--){
            movelist[j] = movelist[j-1];
          }
          movelist[i] = swp;
        }
      }
    }
    
    printf("The value is %d at a depth of %d.\n", value, d);
    printf("Nodes: %u.\n", (u32bit)g_num_nodes);
  }

  *col = *row = -1;
  *nodes = g_num_nodes;

  return value;
}


//=================================================================
// Negamax Function.
//=================================================================
static s32bit
negamax(s32bit depth_remaining, s32bit whos_turn_t, s32bit alpha, s32bit beta)
{
  Move   movelist[MAXMOVES], best;
  s32bit whos_turn = whos_turn_t & PLAYER_MASK;
  s32bit opponent  = whos_turn_t ^ PLAYER_MASK;
  s32bit value;
  s32bit init_alpha = alpha, init_beta = beta;
  u32bit start_nodes = g_num_nodes;
  Move   forcefirst;
  s32bit who_wins_value;

  s32bit stage = 0, state = 0, true_count, i = 0, num_moves = 1;

#ifdef DYNAMIC_POSITION_VALUES
  s32bit dyn_set;
#endif

  // increment a couple of stats
  g_num_nodes++;
#ifdef COLLECT_STATS
  stat_nodes[starting_depth - depth_remaining]++;
#endif

  // if no depth remaining stop search.
  if( depth_remaining <= 0 ){
    s32bit a = 0, b = 0;

    if( (a = does_next_player_win(whos_turn, 0)) > 0 ) {
      // current player wins.
      return 5000;
    }

    if( (b = does_who_just_moved_win(opponent, 0)) >= 0 ) {
      // opponent wins.
      return -5000;
    }
  
    return a - b;
  }
  

  //------------------------------------------
  // Can we determine a winner yet (simple check).
  //------------------------------------------

  // does current player win
  if(g_info_totals[whos_turn].safe > g_info_totals[opponent].real){
#ifdef COLLECT_STATS
    cut1++;
#endif
    return 5000;
  }
  
  // does opponent win
  if(g_info_totals[opponent].safe >= g_info_totals[whos_turn].real){
#ifdef COLLECT_STATS
    cut2++;
#endif
    return -5000;
  }


  //------------------------------------------
  // check transposition table
  //------------------------------------------

  forcefirst.array_index = -1;
  if(hashlookup(&value, &alpha, &beta, depth_remaining,
                &forcefirst, whos_turn))
    return value;
  // since we aren't using iter deep not interested in forcefirst.
  forcefirst.array_index = -1;
  

  //------------------------------------------
  // Can we determine a winner yet (look harder).
  //------------------------------------------

  // does current player win
  if( (who_wins_value = does_next_player_win(whos_turn, 0)) > 0 ) {

#ifdef DEBUG_NEGAMAX
    if(random() % 1000000 == -1){
      does_next_player_win(whos_turn, 1);
      print_board(whos_turn);
    }
#endif

#ifdef COLLECT_STATS
    cut3++;
#endif
    return 5000;
  }
    
  // does opponent win
  if( (who_wins_value = does_who_just_moved_win(opponent, 0)) >= 0 ) {

#ifdef DEBUG_NEGAMAX
    if(who_wins_value < 3){ // && random() % 500 == -1){
      does_who_just_moved_win(opponent, 1);
      //  print_board(opponent);
    }
#endif  

#ifdef COLLECT_STATS
    cut4++;
#endif
    return -5000;
  }

#if 0
  {
    s32bit num;
    num = move_generator_stage1(movelist, whos_turn);
    num = move_generator_stage2(movelist, num, whos_turn);
    if(move_generator(movelist, whos_turn) != num)
      fatal_error(1, "NOPE\n");
  }
#endif


  //------------------------------------------
  // Generate child nodes and examine them.
  //------------------------------------------

  // initialize a few variables. (some of them don't really need to be.)
  stage = state = true_count = i = 0;
  num_moves = 1;

#ifdef TWO_STAGE_GENERATION
  true_count = move_generator_stage1(movelist, whos_turn);
  if(true_count == 0){
    true_count = move_generator_stage2(movelist, 0, whos_turn);
    stage = 1;
    if(true_count == 0) fatal_error(1, "Should always have a move.\n");
  }
#else
  true_count = move_generator(movelist, whos_turn);
  stage = 1;
  if(true_count == 0)   fatal_error(1, "Should always have a move.\n");
#endif
  
  // score all the moves and move the best to the front.
  score_and_get_first(movelist, true_count, whos_turn, forcefirst);
  
  best = movelist[0];
  
  // need to sort moves and generate more moves in certain situations.
  while(state < 3){
    if(state == 0) {
      state = 1;
    } else if(state == 1){
      sort_moves(movelist, 1, true_count);
      num_moves = true_count;
      if(stage == 0) state = 2;
      else           state = 3;
    } else {
      num_moves = move_generator_stage2(movelist, num_moves, whos_turn);
      state = 3;
    }
    
    // Iterate through all the moves.
    for(; i < num_moves; i++){

      // A few statistics
      g_move_number[starting_depth - depth_remaining] = i;
#ifdef RECORD_MOVES
      g_move_player[starting_depth - depth_remaining] = whos_turn;
      g_move_position[starting_depth - depth_remaining] = movelist[i];
#endif

      // make move.
      g_empty_squares -= 2;
      toggle_move(movelist[i], whos_turn);
      toggle_hash_code
    (g_keyinfo[whos_turn][movelist[i].array_index][movelist[i].mask_index]);
#ifdef DYNAMIC_POSITION_VALUES
      dyn_set = set_move_value(movelist[i], whos_turn);
#endif
      
      // recurse.
      value = -negamax(depth_remaining-1,whos_turn^PLAYER_MASK,
                       -beta, -alpha);
      
      // undo move.
      g_empty_squares += 2;
      toggle_move(movelist[i], whos_turn);
      toggle_hash_code
    (g_keyinfo[whos_turn][movelist[i].array_index][movelist[i].mask_index]);
#ifdef DYNAMIC_POSITION_VALUES
      if(dyn_set != 0) unset_move_value(movelist[i], whos_turn);
#endif

#if 0
      if(starting_depth - depth_remaining == 8) {
        s32bit g;
        
        printf("goof:");
        
        for(g = 0; g < 8; g++){
          printf(" :%c:%d(%d,%d)", 
                 (g_move_player[g] == VERTICAL) ? 'V' : 'H',
                 g_move_number[g],
                 g_move_position[g].array_index - 1,
                 g_move_position[g].mask_index - 1);
        }
        printf("\n");
      }
#endif

      // If this is a cutoff, break.
      if(value >= beta){
        alpha = value;
        best  = movelist[i];
        
#ifdef COLLECT_STATS
        stat_cutoffs[starting_depth - depth_remaining]++;
        if(i < 5) stat_nth_try[starting_depth - depth_remaining][i]++;
        else      stat_nth_try[starting_depth - depth_remaining][5]++;
#endif
        break;
      }
      
      // If the current value is greater than alpha, increase alpha.
      if(value > alpha) {
        alpha = value;
        best  = movelist[i];
      }
    }

    // If we have broken out of previous FOR loop make sure we break out
    //   of this loop as well.
    if(value >= beta) break;
  }
  
  // save the position in the hashtable
  hashstore(alpha, init_alpha, init_beta, (g_num_nodes - start_nodes) >> 5,
            depth_remaining, best, whos_turn);
  
  return alpha;
}
