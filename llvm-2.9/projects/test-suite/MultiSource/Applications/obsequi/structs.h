

#ifndef STRUCTS_H
#define STRUCTS_H

#include "utils.h"

//########################################################
// Basic board info which we keep track of.
//########################################################
typedef struct
{
  s32bit safe;
  s32bit real;
  s32bit mobility;
} Basic_Info;


//########################################################
// Info we need to describe a move.
//########################################################
typedef struct
{
  s32bit array_index;
  s32bit mask_index;
  s32bit info;
} Move;


//########################################################
// Info we need for each entry in the hashtable.
//########################################################
typedef struct
{
  // uniquely identifies a board position.
  u32bit key[4];

  // if real num of nodes exceeds ULONG_MAX set to ULONG_MAX.
  //   or maybe we could just shift the bits (larger granularity).
  u32bit nodes;

  // uniquely identifies the previous best move for this position.
  u8bit  best_move;

  // depth of the search when this value was determined.
  u8bit  depth : 7;

  // whos turn it is.
  u8bit  whos_turn : 1;

  // value of node determined with a search to `depth`.
  s16bit value : 14;

  // value of node determined with a search to `depth`.
  u16bit type : 2; //UPPER, LOWER, EXACT.
} Hash_Entry;


//########################################################
// structure used to store current key and it's hash code.
//########################################################
typedef struct
{
  u32bit key[4];
  s32bit code;
} Hash_Key;


//########################################################
// table_keyinfo
//########################################################
typedef struct
{
  s32bit bit1_index;
  s32bit bit2_index;
  u32bit hash_code;
} KeyInfo_s;

typedef struct
{
  KeyInfo_s norm;
  KeyInfo_s flipV;
  KeyInfo_s flipH;
  KeyInfo_s flipVH;
} KeyInfo;

#endif

#ifndef STRUCTS_H

extern void
hashstore(s32bit value, s32bit alpha, s23bit beta,
          u64bit nodes, int depth, struct move best, int color)
{
  /* write the record anyway where the index is*/
  int32 index, minindex;
  int mindepth=1000,iter=0;
  int from,to;
  
  if(depth<0) return;
  hashstores++;
  
  if(realdepth < DEEPLEVEL) {
    /* its in the "deep" hashtable: take care not to overwrite other entries */
    index=Gkey&HASHMASKDEEP;
    minindex=index;
    while(iter<HASHITER) {
      if(deep[index].lock==Glock || deep[index].lock==0) {
        /* found an index where we can write the entry */
        deep[index].lock=Glock;
        deep[index].info=(int16) (depth&DEPTH);
        if(color==BLACK) {
          deep[index].best=best.bm|best.bk;
          deep[index].info|=HASHBLACK;
        } else
          deep[index].best=best.wm|best.wk;
        
        deep[index].value=(sint16)value;
        /* determine valuetype */
        if(value>=beta) {deep[index].info|=LOWER;return;}
        if(value>alpha) {deep[index].info|=EXACT;return;}
        deep[index].info|=UPPER;
        return;
      } else {
        if( hashdepth(deep[index].info) < mindepth) {
          minindex=index;
          mindepth=hashdepth(deep[index].info);
        }
      }
      iter++;
      index++;
      index=index&HASHMASKDEEP;
    }
    /* if we arrive here it means we have gone through all hashiter
       entries and all were occupied. in this case, we write the entry
       to minindex */
    if(mindepth>(depth)) return;
    deep[minindex].lock=Glock;
    deep[minindex].info=(int16) (depth);
    if(color==BLACK)
      deep[minindex].best=best.bm|best.bk;
    else
      deep[minindex].best=best.wm|best.wk;
    deep[minindex].info|=(int16)((color>>1)<<13);
    deep[minindex].value=value;
      /* determine valuetype */
    if(value>=beta) {deep[minindex].info|=LOWER;return;}
    if(value>alpha) {deep[minindex].info|=EXACT;return;}
    deep[minindex].info|=UPPER;
    return;
    /* and return */
  } else {
      index=Gkey&HASHMASKSHALLOW;
      if( hashdepth(shallow[index].info) <= depth )
        /* replace the old entry if the new depth is larger */
        {
          shallow[index].lock=Glock;
          shallow[index].info=(int16)depth;
          shallow[index].value=(sint16)value;
          if(color==BLACK)
            {
              shallow[index].best=best.bm|best.bk;
              shallow[index].info|=HASHBLACK;
            }
          else
            shallow[index].best=best.wm|best.wk;
          
          /* determine valuetype */
          
          if(value>=beta) {shallow[index].info|=LOWER;return;}
          if(value>alpha) {shallow[index].info|=EXACT;return;}
          shallow[index].info|=UPPER;
        }
    }
  return;
}

int hashlookup(int *value, int *alpha, int *beta, int depth, int32 *forcefirst, int color)
{
  int32 index;
  int iter=0;
  
  hashsearch++;
  
  if(realdepth<DEEPLEVEL)
    /* a position in the "deep" hashtable - it's important to find it since */
    /* the effect is larger here! */
    {
      hashhit++;
      index=Gkey&HASHMASKDEEP;
      while(iter<HASHITER)
      	{
          if(deep[index].lock==Glock && (hashcolor(deep[index].info)>>13)==(color>>1))
            {
              /* we have found the position */
              hashhit++;
              /* move ordering */
              *forcefirst=deep[index].best;
              /* use value if depth in hashtable >= current depth)*/
              if(hashdepth(deep[index].info)>=depth)
                {
                  /* if it's an exact value we can use it */
                  if(hashvaluetype(deep[index].info) == EXACT)
                    {
                      *value=deep[index].value;
                      return 1;
                    }
                  /* lower bound */
                  if(hashvaluetype(deep[index].info) == LOWER)
                    {
                      if(deep[index].value>=(*beta)) {*value=deep[index].value;return 1;}
                      if(deep[index].value>(*alpha)) {*alpha=deep[index].value;}
                      return 0;
                    }
                  /* upper bound */
                  if(hashvaluetype(deep[index].info) == UPPER)
                    {
                      if(deep[index].value<=*alpha) {*value=deep[index].value;return 1;}
                      if(deep[index].value<*beta)   {*beta=deep[index].value;}
                      return 0;
                    }
                }
            }
          iter++;
          index++;
          index&=HASHMASKDEEP;
      	}
      return 0;
    }
  /* use shallow hashtable */
  else
    {
      index=Gkey&HASHMASKSHALLOW;
      if(shallow[index].lock==Glock && (hashcolor(shallow[index].info)>>13)==(color>>1))
        {
          hashhit++;
          /*found the right entry!*/
          *forcefirst=shallow[index].best;
          if(hashdepth(shallow[index].info)>=depth)
            {
              if(hashvaluetype(shallow[index].info) == EXACT)
            	{
                  *value=shallow[index].value;
                  return 1;
                }
              /* lower bound */
              if(hashvaluetype(shallow[index].info) == LOWER)
            	{
                  if(shallow[index].value>=*beta) {*value=shallow[index].value;return 1;}
                  if(shallow[index].value>*alpha) {*alpha=shallow[index].value;}
                  return 0;
                }
              /* upper bound */
              if(hashvaluetype(shallow[index].info) == UPPER)
            	{
                  if(shallow[index].value<=*alpha) {*value=shallow[index].value;return 1;}
                  if(shallow[index].value<*beta)   {*beta=shallow[index].value;}
                  return 0;
                }
            }
      	}
    }
  return 0;
}
*/
#endif
