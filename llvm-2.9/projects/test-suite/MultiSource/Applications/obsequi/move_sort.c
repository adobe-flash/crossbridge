
#include "globals.h"

//********************************************************
// Move sorting function.
//
// This is where the program spends some of the larger
//   portions of its time. Sometime it should be sped up.
//********************************************************

#define NUM_BUCKETS 128

//=================================================================
// This is a stable sort algorithm (uses a bucket sort algorithm).
//=================================================================
extern void
sort_moves(Move movelist[MAXMOVES], s32bit start, s32bit num_moves)
{
  Move    bucket[NUM_BUCKETS][MAXMOVES];
  s32bit  buck_val[NUM_BUCKETS];
  s32bit  buck_size[NUM_BUCKETS];

  s32bit num_buckets = 0, i, j;
  
  // place each move in it's proper bucket.
  for(i = start; i < num_moves; i++){
    for(j = 0; j < num_buckets; j++){
      if(movelist[i].info == buck_val[j]){
        bucket[j][buck_size[j]++] = movelist[i];
        break;
      }
    }
    if(j == num_buckets){
      if(j == NUM_BUCKETS) fatal_error(1, "Not enough buckets.\n");
      bucket[j][0] = movelist[i];
      buck_val[j] = movelist[i].info;
      buck_size[j] = 1;
      num_buckets++;
    }
  }
  
  // remove the moves from their buckets in the proper order.
  {
    s32bit best, index, count = start;
    
    while(count != num_moves){
      
      best = buck_val[0];
      index = 0;
      
      for(i = 1; i < num_buckets; i++)
        if(buck_val[i] > best) index = i, best = buck_val[i];
      
      // every bucket must have at least one move.
      i = 0;
      do {
        movelist[count++] = bucket[index][i++];
      } while(i < buck_size[index]);
      
      buck_val[index] = -5000;
    }
  }
}
