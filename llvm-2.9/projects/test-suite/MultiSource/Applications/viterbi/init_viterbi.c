#include <assert.h>
#include <memory.h>

#include "common.h"

void init_viterbi(param_viterbi_t* param, coderate_tt in_rate) 
{
  unsigned char Tabl[7][MAX_Nways];
  size_t i, j;
  
  memset(param, 0, sizeof(param_viterbi_t));
  memset(&Tabl, 0, sizeof(Tabl));
  
  switch (in_rate) {
   case CODERATE_1D2:
    param->punct1[0] = 1; param->punct2[0] = 1;
    param->n_in = 1;
    break;
   case CODERATE_2D3:
    param->punct1[0] = 1;  param->punct1[1] = 0; param->punct2[0] = 1;
    param->punct2[1] = 1;
    param->n_in = 2;
    break;
   case CODERATE_3D4:
    param->punct1[0] = 1; param->punct1[1] = 0; param->punct1[2] = 1;
    param->punct2[0] = 1; param->punct2[1] = 1; param->punct2[2] = 0;
    param->n_in = 3;
    break;
   case CODERATE_5D6:
    param->punct1[0] = 1;  param->punct1[1] = 0; param->punct1[2] = 1;
    param->punct1[3] = 0;  param->punct1[4] = 1;
    param->punct2[0] = 1;  param->punct2[1] = 1; param->punct2[2] = 0;
    param->punct2[3] = 1;  param->punct2[4] = 0;
    param->n_in = 5;
    break;
   case CODERATE_7D8:
    param->punct1[0] = 1;  param->punct1[1] = 0; param->punct1[2] = 0;
    param->punct1[3] = 0;  param->punct1[4] = 1; param->punct1[5] = 0;
    param->punct1[6] = 1;
    param->punct2[0] = 1;  param->punct2[1] = 1; param->punct2[2] = 1;
    param->punct2[3] = 1;  param->punct2[4] = 0; param->punct2[5] = 1;
    param->punct2[6] = 0;
    param->n_in = 7;
    break;
   default:
    assert(0 && "Unsupported coderate");
  }

  param->Nbits = 7;
  param->Nways = 1 << (param->Nbits);
  param->Delay = 150;

  for (i=0; i<param->Nways; ++i) {
    for (j=0; j<7; ++j) {
        Tabl[6-j][i] = ((i & (1 << j)) != 0);
    }
  }
   
  for (i=0; i<param->Nways; ++i) {
    param->Tabl_X[i] = Tabl[0][i] ^ Tabl[1][i] ^ Tabl[2][i] ^ Tabl[5][i];
    param->Tabl_Y[i] = Tabl[1][i] ^ Tabl[2][i] ^ Tabl[4][i] ^ Tabl[5][i];
  }

  for (i=0; i<param->Nways; ++i) {
    param->Metr[i] = 1e6;
  }
  param->Metr[0] = 0;
  
}

