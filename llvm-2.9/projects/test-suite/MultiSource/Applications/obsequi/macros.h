

#ifndef MACROS_H
#define MACROS_H

#include "globals.h"


//########################################################
// We use tables to count how many bits are set in a bitmask.
//########################################################
#ifdef COUNTBITS8
#define countbits32(x) (countbits8[x&0x000000FF] \
                        +countbits8[(x>>8)&0x000000FF]\
                        +countbits8[(x>>16)&0x000000FF]\
                        +countbits8[(x>>24)&0x000000FF])
#endif

#ifdef COUNTBITS16
#define countbits32(x) (countbits16[x&0x0000FFFF] \
                        +countbits16[(x>>16)&0x0000FFFF])
#endif


//########################################################
// Assume we have a mask with only one bit set -
//   we use tables to determine the index of this bit.
//########################################################
static inline u32bit
lastbit32(u32bit x)
{
#ifdef LASTBIT8
  /* returns the position of the last bit in x */
  if(x&0x000000FF)
    return(lastbit8[x&0x000000FF]);
  if(x&0x0000FF00)
    return(lastbit8[(x>>8)&0x000000FF]+8);
  if(x&0x00FF0000)
    return(lastbit8[(x>>16)&0x000000FF]+16);
  if(x&0xFF000000)
    return(lastbit8[(x>>24)&0x000000FF]+24);
  return 100;
#endif

#ifdef LASTBIT16
  /* returns the position of the last bit in x */
  if(x&0x0000FFFF)
    return(lastbit16[x&0x0000FFFF]);
  if(x&0xFFFF0000)
    return(lastbit16[(x>>16)&0x0000FFFF]+16);
  return 100;
#endif
}


//########################################################
// Define the function COUNTMOVES.
//  This is either a macro or an inline function.
//########################################################
static inline s32bit
COUNTMOVES_DYN(u32bit mask)
{
  s32bit count = 0;
  u32bit tmp;
  
  while(mask){
    tmp = (mask&-mask);           // least sig bit of m
    mask &= ~(tmp | (tmp << 1));  // remove bit and next bit.
    count++;
  }
  return count;
}

#ifdef COUNTMOVES_TABLE
#ifdef BOARD_SIZE_LT_16
//========================================================
// count moves (assuming small board).
//========================================================
#define COUNTMOVES(mask) move_table16[(mask)]

#else
//========================================================
// count moves (assuming large board).
//========================================================
static inline s32bit
COUNTMOVES(u32bit mask)
{
  s32bit count = 0;
  count = move_table16[mask & 0x0000FFFF];

  if(count&0xF0000000)
    count = ((count & 0x0000FFFF) + move_table16[mask >> 16]) & 0x0000FFFE;
  else
    count = (count + move_table16[mask >> 16]) & 0x0000FFFF;
  
  return count;
  
  /*
  if(count&ERASE_NEXT_BIT) mask &= ~(NEXT_BIT);
  return (count + move_table16[mask >> 16]) & 0x0000FFFF;
  */
}

#endif
#else
//========================================================
// count moves (move_table doesn't exist).
//========================================================
#define COUNTMOVES(mask) COUNTMOVES_DYN(mask)

#endif




#define HASHKEY_UPDATE(key,index) key[index/32] ^= NTH_BIT(index%32)


#define toggle_hash_code(info)  \
                                                              \
HASHKEY_UPDATE(g_norm_hashkey.key, info.norm.bit1_index);     \
HASHKEY_UPDATE(g_norm_hashkey.key, info.norm.bit2_index);     \
g_norm_hashkey.code ^= info.norm.hash_code;                   \
                                                              \
HASHKEY_UPDATE(g_flipV_hashkey.key, info.flipV.bit1_index);   \
HASHKEY_UPDATE(g_flipV_hashkey.key, info.flipV.bit2_index);   \
g_flipV_hashkey.code ^= info.flipV.hash_code;                 \
                                                              \
HASHKEY_UPDATE(g_flipH_hashkey.key, info.flipH.bit1_index);   \
HASHKEY_UPDATE(g_flipH_hashkey.key, info.flipH.bit2_index);   \
g_flipH_hashkey.code ^= info.flipH.hash_code;                 \
                                                              \
HASHKEY_UPDATE(g_flipVH_hashkey.key, info.flipVH.bit1_index); \
HASHKEY_UPDATE(g_flipVH_hashkey.key, info.flipVH.bit2_index); \
g_flipVH_hashkey.code ^= info.flipVH.hash_code;


#endif //MACROS_H
