
#include "globals.h"

#define PRINT_DOES_X_WIN_INFO

// If defined we just immediately return from each function with
//   a value of -1. (In other words no winner yet.)
//#define NOTHING

// If defined don't pack any protected moves onto the board.
//#define NO_PROT

// If defined pretend all (unpacked) squares are available to opponent.
//#define NO_UNUSED

// If defined pretend we didn't find any vulnerable moves with a prot square.
//#define NO_VULN_W_PROT

// If defined pretend we didn't find any vulnerable moves with a prot square.
//#define NO_VULN_TYPE_1

// If defined pretend there were no vulnerable moves (note this also
//    means NO_VULN_W_PROT).
//#define NO_VULN

// If defined don't look for safe options.
//#define NO_SAFE_OPT

//#################################################################
// This function tries to pack as many 'protective' areas onto
//  the board as it can. (no guarantee of optimality).
//#################################################################
static inline void
pack_prot(u32bit tmp_board[32], s32bit player, s32bit *prot)
{
  s32bit r = 0, p = 0; //row, and number of prot. areas we have placed.
  
  u32bit tmp, inter, walls;
  
  for(r = 0; r < g_board_size[player]; r++){
    // set bit at each position where there is interference.
    inter = (tmp_board[r] | tmp_board[r+1] | tmp_board[r+2] | tmp_board[r+3]
             | g_board[player][r+1] | g_board[player][r+2]);
    inter = (inter >> 1) | inter; 
    
    // set bit at each position which there is a wall.
    walls = ( ((g_board[player][r] >> 1) & g_board[player][r])
              | ((g_board[player][r+3] >> 1) & g_board[player][r+3]) );
    
    // combine (bit set at each position which we can place a prot. area).
    inter = walls & ~inter;
    
    // process the positions.
    while(inter){
      tmp = (inter & -inter);        // least sig bit of m
      inter &= ~(tmp | (tmp << 1));  // remove bit and next bit.

      tmp_board[r+1] |= tmp | (tmp << 1); //record where we put
      tmp_board[r+2] |= tmp | (tmp << 1); // the protective square.

      p++; // count it.
    }
  }
  
  *prot = p;
}


//#################################################################
// This function tries to pack as many 'vuln. type 1 and 2' areas
//  onto the board as it can. (no guarantee of optimality).
// It also counts the number of unused squares.
//#################################################################
static void
pack_vuln(u32bit tmp_board[32], s32bit player,
          s32bit *vuln2, s32bit *vuln2_w_prot,
          s32bit *vuln1, s32bit *vuln1_w_prot,
          s32bit *unused) //, s32bit print)
{
  u32bit curr_row, adj_rows, next_prev_row;
  u32bit tmp = 0, tmp_;

  s32bit v2 = 0, v2_p = 0, v1 = 0, v1_p = 0, u = 0;
  
  s32bit r, state;
  u32bit s_v = 0; //start of vulnerable
  
  for(r = 0; r < g_board_size[player]; r++){
    next_prev_row  = tmp_board[r];
    next_prev_row |= ~(g_board[player][r+2] | (g_board[player][r+2] << 1));
    next_prev_row |= ~(g_board[player][r+2] | (g_board[player][r+2] >> 1));

    curr_row = ~(g_board[player][r+1] | tmp_board[r+1]);
    adj_rows = g_board[player][r] & g_board[player][r+2];
    state = 0;
    
    //========================================================
    // Three types of squares, prot, empty, occ.
    //
    // state = 0
    // if(prot)  -> state = 1
    // if(empty) -> state = 2, c_t = c,
    // if(occ)   -> state = 0
    //
    // state = 1
    // if(prot)  -> state = 0, safe++
    // if(empty) -> state = 0, v_p++, mark(c-1)
    // if(occ)   -> state = 0, unu++
    //
    // state = 2
    // if(prot)  -> state = 3,
    // if(empty) -> state = 0, vuln++, mark(c_t)
    // if(occ)   -> state = 0,
    //
    // state = 3
    // if(prot)  -> state = 4, safe++
    // if(empty) -> state = 2, v_p++, mark(c_t), c_t = c
    // if(occ)   -> state = 0, v_p++, mark(c_t)
    //
    // state = 4
    // if(prot)  -> state = 3
    // if(empty) -> state = 2, c_t = c
    // if(occ)   -> state = 0
    //========================================================

    while(curr_row){
      tmp_ = (curr_row & -curr_row);  //get least sig bit of curr_row
      curr_row ^= tmp_;               //remove least sig bit of curr_row.

      // if true then this iteration and last iteration are not contiguous.
      // Which means there was an occupied square.
      if ( ((tmp_ >> 1) & tmp) == 0 ) {
        if(state == 1) {
          u++;
          tmp_board[r+1] |= tmp;
        } else if(state == 3) {
          tmp_board[r+1] |= (s_v | (s_v << 1));
          if((s_v & next_prev_row) || ((s_v << 1) & next_prev_row))
            v2_p++;
          else
            v1_p++;
        }
        state = 0;
      }
      tmp = tmp_;
      
      // empty and protected
      if( tmp & adj_rows ){
        if(state == 0) {
          state = 1;
        } else if(state == 1) {
          state = 0;
          //safe
        } else if(state == 2) {
          state = 3;
        } else if(state == 3) {
          state = 4;
          //safe
        } else if(state == 4) {
          state = 3;
        }
      }
      
      // unprotected
      else {
        if(state == 0) {
          state = 2;
          s_v = tmp;
        } else if(state == 1){
          state = 0;

          // Check tmp >> 1

          tmp_board[r+1] |= (tmp | (tmp >> 1));
          if((tmp & next_prev_row) || ((tmp >> 1) & next_prev_row))
            v2_p++;
          else
            v1_p++;
        } else if(state == 2){
          state = 0;
          tmp_board[r+1] |= (s_v | (s_v << 1));
          if((s_v & next_prev_row) || ((s_v << 1) & next_prev_row))
            v2++;
          else
            v1++;
        } else if(state == 3){
          state = 2;
          tmp_board[r+1] |= (s_v | (s_v << 1));
          if((s_v & next_prev_row) || ((s_v << 1) & next_prev_row))
            v2_p++;
          else
            v1_p++;
          s_v = tmp;
        } else if(state == 4){
          state = 2;
          s_v = tmp;
        }
      }
    }

    if(state == 1) {
      u++;
      tmp_board[r+1] |= tmp;
    } else if(state == 3) {
      tmp_board[r+1] |= (s_v | (s_v << 1));
      if((s_v & next_prev_row) || ((s_v << 1) & next_prev_row))
        v2_p++;
      else
        v1_p++;
    }

    //    if(print)
    //  printf("%d %d %d %d %d - %X\n", v2, v2_p, v1, v1_p, u,
    //        tmp_board[r+1]);
    
  }
  //  if(print) printf("\n");

  *vuln2 = v2 + v2_p;
  *vuln2_w_prot = v2_p;
  *vuln1 = v1 + v1_p;
  *vuln1_w_prot = v1_p;
  *unused = u;
}


//#################################################################
// This function tries to pack as many 'option of vulnerability' areas
//  onto the board as it can. (no guarantee of optimality).
//#################################################################
static inline void
pack_safe(u32bit tmp_board[32], s32bit player,
          s32bit *safe_op2, s32bit *safe_op1, s32bit *safe_op0)
{
  u32bit guard, mask, curr, tmp;

  s32bit r;                       //row
  s32bit s2 = 0, s1 = 0, s0 = 0;
  
  for(r = 0; r < g_board_size[player]; r++){
    guard = g_board[player][r] & g_board[player][r+2];
    curr  = g_board[player][r+1] | tmp_board[r+1];
    
    // mask contains a bit for each safe move.
    mask  = ( (~(curr | (curr >> 1))) & (guard & (guard >> 1)) );

    while(mask){
      tmp   =  (mask & -mask);          // least sig bit of m
      mask &= ~(tmp | (tmp << 1));      // remove bit and next bit.

      // add these bits to current.
      curr |= (tmp | (tmp << 1));

      if( ! ( (curr | tmp_board[r] | tmp_board[r+2]) & (tmp >> 1) ) ){
        // we have an option to move vulnerably. (which option is it).
        curr           |= (tmp >> 1);
        tmp_board[r+1] |= (tmp >> 1);
        
        // check up.
        if(  (!(g_board[player][r] & (tmp >> 1)))
             && (g_board[player][r-1] & (tmp >> 1)) ){
          // up is good now check down.
          if(  (!(g_board[player][r+2] & (tmp >> 1)))
               && (g_board[player][r+3] & (tmp >> 1)) ){
            s2++;
          } else {
            s1++;
          }
        } else if(  (!(g_board[player][r+2] & (tmp >> 1)))
                    && (g_board[player][r+3] & (tmp >> 1)) ){
          s1++;
        } else {
          s0++;
        }
      } else 
      if( ! ( (mask | curr | tmp_board[r] | tmp_board[r+2]) & (tmp << 2) ) ){
        // we have an option to move vulnerably. (which option is it).
        curr           |= (tmp << 2);
        tmp_board[r+1] |= (tmp << 2);
        
        // check up.
        if(  (!(g_board[player][r] & (tmp << 2)))
             && (g_board[player][r-1] & (tmp << 2)) ){
          // up is good now check down.
          if(  (!(g_board[player][r+2] & (tmp << 2)))
               && (g_board[player][r+3] & (tmp << 2)) ){
            s2++;
          } else {
            s1++;
          }
        } else if(  (!(g_board[player][r+2] & (tmp << 2)))
                    && (g_board[player][r+3] & (tmp << 2)) ){
          s1++;
        } else {
          s0++;
        }
      }
    }
    //    if(debug) printf("(%d,%d) ", s0, s1);
  }
  //  if(debug) printf("\n");

  *safe_op2 = s2;
  *safe_op1 = s1;
  *safe_op0 = s0;
}

//#################################################################
// 
//#################################################################
extern s32bit
#ifdef PRINT_DOES_X_WIN_INFO
does_next_player_win(s32bit next_player, s32bit print)
#else
does_next_player_win(s32bit next_player)
#endif
{
  // info we directly get from the board.
  s32bit prot;   // This is the number of protective regions.

  s32bit vuln2;   // Total number of vulnerable moves (includes vuln_w_prot).
  // Num of vuln moves which contain a square unavailable to the opponent.
  s32bit vuln2_w_prot;
  s32bit vuln1;   // Total number of vulnerable moves (includes vuln_w_prot).
  // Num of vuln moves which contain a square unavailable to the opponent.
  s32bit vuln1_w_prot;

  s32bit safe;   // Number of safe moves horizontal has.
  // Number of squares unused in our packing and unavailable for opponent.
  s32bit unused;
  s32bit empty;  // Number of empty squares on board.
  s32bit safe_op2, safe_op1, safe_op0; //safe moves with options.

  // value we return and other variables.
  s32bit r_value = 0, i;

  // temporary board to store which positions have already been packed.
  u32bit tmp_board[32];

#ifdef NOTHING
  return -1;
#endif
  
  //========================================================  
  // Initialize all of the values.
  //========================================================  
  for(i = 0; i < 32; i++) tmp_board[i] = 0;
  safe  = g_info_totals[next_player].safe;
  empty = g_empty_squares;
  
  // Determine the number of protective regions that we have.
#ifdef NO_PROT
  prot = 0;
#else
  pack_prot(tmp_board, next_player, &prot);
#endif

  // Determine the number of vuln, vuln_w_prot, and unused.
  pack_vuln(tmp_board, next_player, &vuln2, &vuln2_w_prot,
            &vuln1, &vuln1_w_prot, &unused); //, print);

#ifdef NO_VULN_W_PROT
  vuln1_w_prot = vuln2_w_prot = 0;
#endif

#ifdef NO_VULN_TYPE_1
  vuln2 += vuln1;
  vuln2_w_prot += vuln1_w_prot;
  vuln1 = vuln1_w_prot = 0;
#endif

#ifdef NO_UNUSED
  unused = 0;
#endif

#ifdef NO_VULN
  vuln2 = vuln1 = vuln2_w_prot = vuln1_w_prot = 0;
#endif

  // Determine the number of safe moves with options we have.
#ifdef NO_SAFE_OPT
  safe_op2 = safe_op1 = safe_op0 = 0;
#else
  pack_safe(tmp_board, next_player, &safe_op2, &safe_op1, &safe_op0);
#endif

#ifdef PRINT_DOES_X_WIN_INFO
  if(print){
    fprintf(stderr, "%d moves next, do they win?\n", next_player);
    fprintf(stderr, "prot %d, vuln2 %d(%d), vuln1 %d(%d), "
            "safe %d, unused %d, empty %d.\n",
            prot, vuln2, vuln2_w_prot, vuln1, vuln1_w_prot,
            safe, unused, empty);
    fprintf(stderr, "safe_op2 %d, safe_op1 %d, safe_op0 %d.\n",
            safe_op2, safe_op1, safe_op0);
  }
#endif
  
  {
    s32bit moves, opp_moves, x = 0;

    if(prot % 2 == 1){
      prot--;
      safe += 2;
    } else if(vuln2 % 3 != 0){
      vuln2--;
      safe++;
      if(vuln2_w_prot > vuln2) vuln2_w_prot--;
    } else if(vuln1 % 2 != 0){
      vuln1--;
      safe++;
      if(vuln1_w_prot > vuln1) vuln1_w_prot--;
    } else if(safe_op2 % 2 != 0){
      safe_op2--;
      unused+=3;
    } else if(safe_op1 % 2 != 0){
      safe_op1--;
      unused+=2;
    } else if(safe_op0 % 2 != 0){
      safe_op0--;
      unused+=1;
    } else if(vuln2 > 0){
      vuln2--;
      safe++;
      if(vuln2_w_prot > vuln2) vuln2_w_prot--;
    } else if(vuln1 > 0){
      vuln1--;
      safe++;
      if(vuln1_w_prot > vuln1) vuln1_w_prot--;
    } else if(prot > 0){
      prot--;
      safe += 2;
    } else {
      return -1;
    }

    if(prot % 2 == 1){
      prot--;
      vuln2 += 2;
    }

    moves = (prot) + (vuln2/3) + (vuln1/2) + safe;
    
    if(vuln2 % 3 != 0 && vuln1 % 2 != 0){
      moves++, unused--, x=1;
      //      if(vuln2 > 0 || vuln1 > 0) unused--;
    } else if(vuln2 % 3 == 0 && vuln1 % 2 == 0) x=1;

    if(x == 1){
      if(safe_op2%2 == 1) safe_op2--, safe_op1++;
      if(safe_op1%2 == 1) safe_op1--, safe_op0++;
    } else {
      if(safe_op2%2 == 1){
        unused += 3;
        if(safe_op1%2==1) safe_op1--, safe_op0++;
      } else if(safe_op1%2 == 1) { unused += 2; }
      else if(safe_op0%2 == 1)   { unused += 1; }
    }
    
    unused += vuln2_w_prot - ( (vuln2)/3 - (vuln2-vuln2_w_prot)/3 );
    unused += vuln1_w_prot - ( (vuln1)/2 - (vuln1-vuln1_w_prot)/2 );

    unused += (safe_op2/2) * 3;
    unused += (safe_op1/2) * 2;
    unused += (safe_op0/2) * 1;

    opp_moves = (empty - (moves*2) - unused)/2;

    //========================================================
    // If r_value > 0 then next_player wins.
    //========================================================
    r_value = moves - opp_moves;

#ifdef PRINT_DOES_X_WIN_INFO
    if(print){
      printf("moves:%d, opp:%d.\n", moves, opp_moves);
      if(moves - opp_moves >= 0) printf("H WINS\n");
    }
#endif
  }

  return r_value;
}


//#################################################################
// 
//#################################################################
extern s32bit
#ifdef PRINT_DOES_X_WIN_INFO
does_who_just_moved_win(s32bit who_just_moved, s32bit print)
#else
does_who_just_moved_win(s32bit who_just_moved)
#endif
{
  // info we directly get from the board.
  s32bit prot;   // This is the number of protective regions.

  s32bit vuln2;   // Total number of vulnerable moves (includes vuln_w_prot).
  // Num of vuln moves which contain a square unavailable to the opponent.
  s32bit vuln2_w_prot;
  s32bit vuln1;   // Total number of vulnerable moves (includes vuln_w_prot).
  // Num of vuln moves which contain a square unavailable to the opponent.
  s32bit vuln1_w_prot;

  s32bit safe;   // Number of safe moves horizontal has.
  // Number of squares unused in our packing and unavailable for opponent.
  s32bit unused;
  s32bit empty;  // Number of empty squares on board.
  s32bit safe_op2, safe_op1, safe_op0; //safe moves with options.

  // value we return and other variables.
  s32bit r_value = 0, i;

  // temporary board to store which positions have already been packed.
  u32bit tmp_board[32];
  
#ifdef NOTHING
  return -1;
#endif
  
  //========================================================  
  // Initialize all of the values.
  //========================================================  
  for(i = 0; i < 32; i++) tmp_board[i] = 0;
  safe  = g_info_totals[who_just_moved].safe;
  empty = g_empty_squares;
  
  // Determine the number of protective regions that we have.
#ifdef NO_PROT
  prot = 0;
#else
  pack_prot(tmp_board, who_just_moved, &prot);
#endif

  // Determine the number of vuln, vuln_w_prot, and unused.
  pack_vuln(tmp_board, who_just_moved, &vuln2, &vuln2_w_prot,
            &vuln1, &vuln1_w_prot, &unused);

#ifdef NO_VULN_W_PROT
  vuln1_w_prot = vuln2_w_prot = 0;
#endif

#ifdef NO_VULN_TYPE_1
  vuln2 += vuln1;
  vuln2_w_prot += vuln1_w_prot;
  vuln1 = vuln1_w_prot = 0;
#endif

#ifdef NO_UNUSED
  unused = 0;
#endif

#ifdef NO_VULN
  vuln2 = vuln1 = vuln2_w_prot = vuln1_w_prot = 0;
#endif

  // Determine the number of safe moves with options we have.
#ifdef NO_SAFE_OPT
  safe_op2 = safe_op1 = safe_op0 = 0;
#else
  pack_safe(tmp_board, who_just_moved, &safe_op2, &safe_op1, &safe_op0);
#endif
  
  /*
  if(prot % 2 == 1){
    prot--;
    vuln2 += 2;
  }

  if(print == 1 && unused > 0
     && prot >= 2 // && (vuln2%3 != 0 || vuln1%2 != 0)
     && vuln2 >= 3 && vuln1 >= 2
     && safe_op1 + safe_op0 >= 2){
    print_board(who_just_moved);
  } else {
    print = 0;
  }
  */
#ifdef PRINT_DOES_X_WIN_INFO
  if(print){
    fprintf(stderr, "%d just moved, do they win?\n", who_just_moved);
    fprintf(stderr, "prot %d, vuln2 %d(%d), vuln1 %d(%d), "
            "safe %d, unused %d, empty %d.\n",
            prot, vuln2, vuln2_w_prot, vuln1, vuln1_w_prot,
            safe, unused, empty);
    fprintf(stderr, "safe_op2 %d, safe_op1 %d, safe_op0 %d.\n",
            safe_op2, safe_op1, safe_op0);
  }
#endif
  
  {
    s32bit moves, opp_moves, x = 0;

    if(prot % 2 == 1){
      prot--;
      vuln2 += 2;
    }
    
    moves = (prot) + (vuln2/3) + (vuln1/2) + safe;
    
    if(vuln2 % 3 != 0 && vuln1 % 2 != 0){
      moves++, unused--, x=1;
      //      if(vuln2 > 0 || vuln1 > 0) unused--;
    } else if(vuln2 % 3 == 0 && vuln1 % 2 == 0) x=1;

    if(x == 1){
      if(safe_op2%2 == 1) safe_op2--, safe_op1++;
      if(safe_op1%2 == 1) safe_op1--, safe_op0++;
    } else {
      if(safe_op2%2 == 1){
        unused += 3;
        if(safe_op1%2==1) safe_op1--, safe_op0++;
      } else if(safe_op1%2 == 1) { unused += 2; }
      else if(safe_op0%2 == 1)   { unused += 1; }
    }
    
    unused += vuln2_w_prot - ( (vuln2)/3 - (vuln2-vuln2_w_prot)/3 );
    unused += vuln1_w_prot - ( (vuln1)/2 - (vuln1-vuln1_w_prot)/2 );

    unused += (safe_op2/2) * 3;
    unused += (safe_op1/2) * 2;
    unused += (safe_op0/2) * 1;

    opp_moves = (empty - (moves*2) - unused)/2;

    //========================================================
    // If r_value >= 0 then who_just_moved wins.
    //========================================================
    r_value = moves - opp_moves;

#ifdef PRINT_DOES_X_WIN_INFO
    if(print){
      printf("moves:%d, opp:%d.\n", moves, opp_moves);
      if(moves - opp_moves >= 0) printf("H WINS\n");
    }
#endif
  }

  return r_value;
}
