

#include "globals.h"

#ifdef COUNTBITS16
s32bit countbits16[65536];

static void
init_countbits()
{
  s32bit i;
  
  countbits16[0] = 0;
  
  for(i = 1; i < 65536; i++){
    countbits16[i] = (i & 1) + countbits16[i >> 1];
  }
}
#endif

#ifdef COUNTBITS8
s32bit countbits8[256];

static void
init_countbits()
{
  s32bit i;
  
  countbits8[0] = 0;
  
  for(i = 1; i < 256; i++){
    countbits8[i] = (i & 1) + countbits8[i >> 1];
  }
}
#endif

#ifdef LASTBIT16
s32bit lastbit16[65536];

static void
init_lastbit()
{
  s32bit i;
  
  for(i = 1; i < 65536; i++){
    if(i&NTH_BIT(0)) {lastbit16[i] = 0; continue;}
    if(i&NTH_BIT(1)) {lastbit16[i] = 1; continue;}
    if(i&NTH_BIT(2)) {lastbit16[i] = 2; continue;}
    if(i&NTH_BIT(3)) {lastbit16[i] = 3; continue;}
    if(i&NTH_BIT(4)) {lastbit16[i] = 4; continue;}
    if(i&NTH_BIT(5)) {lastbit16[i] = 5; continue;}
    if(i&NTH_BIT(6)) {lastbit16[i] = 6; continue;}
    if(i&NTH_BIT(7)) {lastbit16[i] = 7; continue;}
    if(i&NTH_BIT(8)) {lastbit16[i] = 8; continue;}
    if(i&NTH_BIT(9)) {lastbit16[i] = 9; continue;}
    if(i&NTH_BIT(10)) {lastbit16[i] = 10; continue;}
    if(i&NTH_BIT(11)) {lastbit16[i] = 11; continue;}
    if(i&NTH_BIT(12)) {lastbit16[i] = 12; continue;}
    if(i&NTH_BIT(13)) {lastbit16[i] = 13; continue;}
    if(i&NTH_BIT(14)) {lastbit16[i] = 14; continue;}
    if(i&NTH_BIT(15)) {lastbit16[i] = 15; continue;}
  }
  
  lastbit16[0] = 100;
}
#endif

#ifdef LASTBIT8
s32bit lastbit8[256];

static void
init_lastbit()
{
  s32bit i;
  
  for(i = 1; i < 256; i++){
    if(i&NTH_BIT(0)) {lastbit8[i] = 0; continue;}
    if(i&NTH_BIT(1)) {lastbit8[i] = 1; continue;}
    if(i&NTH_BIT(2)) {lastbit8[i] = 2; continue;}
    if(i&NTH_BIT(3)) {lastbit8[i] = 3; continue;}
    if(i&NTH_BIT(4)) {lastbit8[i] = 4; continue;}
    if(i&NTH_BIT(5)) {lastbit8[i] = 5; continue;}
    if(i&NTH_BIT(6)) {lastbit8[i] = 6; continue;}
    if(i&NTH_BIT(7)) {lastbit8[i] = 7; continue;}
  }
  
  lastbit8[0] = 100;
}
#endif

#ifdef COUNTMOVES_TABLE
s32bit move_table16[65536];

static void
init_movetable()
{
  u32bit i = 0, mask, tmp = 0, count;
  
  while(i < 65536){
    mask = i;
    count = 0;
    
    while(mask){
      tmp = (mask&-mask);           // least sig bit of m
      mask &= ~(tmp | (tmp << 1));  // remove bit and next bit.
      count++;
    }

#ifndef BOARD_SIZE_LT_16
    if(tmp & 0x00008000) count |= 0xF0000000;
#endif    

    move_table16[i] = count;
    i++;
  }
}
#else
#define init_movetable() ;
#endif

extern void
init_static_tables()
{
  init_countbits();
  init_lastbit();
  init_movetable();
}


KeyInfo      g_keyinfo[2][32][32];

static void
negate_keyinfo(KeyInfo_s *keyinfo)
{
  keyinfo->bit1_index = keyinfo->bit2_index = -1;
  keyinfo->hash_code = 0;
}

      
static void
fill_in_hash_code(KeyInfo_s *info, s32bit num_cols)
{
  s32bit r, c, hash = 0;
    
  r = info->bit1_index/num_cols;
  c = info->bit1_index%num_cols;
  
  hash = g_zobrist[r+1][c+1];
  
  r = info->bit2_index/num_cols;
  c = info->bit2_index%num_cols;
  
  hash ^= g_zobrist[r+1][c+1];

  info->hash_code = hash;
}


static void
fill_in_key_entry(KeyInfo *keyinfo, s32bit num_rows, s32bit num_cols)
{
  if(keyinfo->norm.bit1_index == -1){
    negate_keyinfo( & keyinfo->norm);
    negate_keyinfo( & keyinfo->flipV);
    negate_keyinfo( & keyinfo->flipH);
    negate_keyinfo( & keyinfo->flipVH);
  } else {
    s32bit r1, c1, r2, c2;
    
    r1 = keyinfo->norm.bit1_index/num_cols;
    c1 = keyinfo->norm.bit1_index%num_cols;
    r2 = keyinfo->norm.bit2_index/num_cols;
    c2 = keyinfo->norm.bit2_index%num_cols;
  
    keyinfo->flipV.bit1_index  = (r1*num_cols)+(num_cols - c1 - 1);
    keyinfo->flipV.bit2_index  = (r2*num_cols)+(num_cols - c2 - 1);

    keyinfo->flipH.bit1_index  = ((num_rows - r1 - 1) * num_cols) + c1;
    keyinfo->flipH.bit2_index  = ((num_rows - r2 - 1) * num_cols) + c2;

    keyinfo->flipVH.bit1_index = ( ((num_rows - r1 - 1) * num_cols)
                                   + (num_cols - c1 - 1) );
    keyinfo->flipVH.bit2_index = ( ((num_rows - r2 - 1) * num_cols)
                                   + (num_cols - c2 - 1) );

    fill_in_hash_code( & keyinfo->norm, num_cols);
    fill_in_hash_code( & keyinfo->flipV, num_cols);
    fill_in_hash_code( & keyinfo->flipH, num_cols);
    fill_in_hash_code( & keyinfo->flipVH, num_cols);
  }
}


extern void
init_less_static_tables()
{
  s32bit n_rows, n_cols, i, j, k;
  
  n_rows = g_board_size[HORIZONTAL], n_cols = g_board_size[VERTICAL];
  
  for(i = 0; i < 32; i++)
    for(j = 0; j < 32; j++)
      for(k = 0; k < 2; k++)
        negate_keyinfo( & g_keyinfo[k][i][j].norm);
  
  for(i = 0; i < n_rows; i++){
    for(j = 0; j < n_cols; j++){
      //Horizontal Entry
      if(j + 1 < n_cols){
        g_keyinfo[HORIZONTAL][i+1][j+1].norm.bit1_index = (i*n_cols)+j;
        g_keyinfo[HORIZONTAL][i+1][j+1].norm.bit2_index = (i*n_cols)+(j+1);
      }
      
      //Vertical Entry
      if(i + 1 < n_rows){
        g_keyinfo[VERTICAL][j+1][i+1].norm.bit1_index   = (i*n_cols)+j;
        g_keyinfo[VERTICAL][j+1][i+1].norm.bit2_index   = ((i+1)*n_cols)+j;
      }
    }
  }
  
  for(i = 0; i < 32; i++)
    for(j = 0; j < 32; j++)
      for(k = 0; k < 2; k++)
        fill_in_key_entry(&g_keyinfo[k][i][j], n_rows, n_cols);
}

