#include <memory.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"

void dec_viterbi_F(dvector* Metr_mem, unsigned char* history_mem, bitvector* bit_stream,
                   const dvarray* Dist, const param_viterbi_t* param, size_t n) 
{
  size_t i_in = 0, i_punct, i, j, vote, bv = 0;
  double *Metr0, *Metr1, *Metr;
  char X0, X1, Y0, Y1;
  unsigned char history[MAX_Nways][MAX_history];
  unsigned char history_new[MAX_Nways][MAX_history+1];
#if 0
  /* FIXME */
  /* Time varies between runs giving false negatives about which run
     (JIT or LLC) failed */
  double startTime, now, estTotal;
#endif /* 0 */
  
  if (bit_stream->length) {
    bitvector_clear(bit_stream);
  }
  
  if (n==0) {
    n = Dist->data[0].length;
  }
  
  bitvector_init(bit_stream, n);

  Metr  = (double*)malloc(Metr_mem->length*sizeof(double));
  Metr0 = (double*)malloc(Metr_mem->length*sizeof(double));
  Metr1 = (double*)malloc(Metr_mem->length*sizeof(double));
  
  memcpy(Metr, Metr_mem->data, Metr_mem->length*sizeof(double));
  memcpy(history, history_mem, sizeof(history));
  
#if 0
  /* FIXME */
  /* Time varies between runs giving false negatives about which run
     (JIT or LLC) failed */
  startTime = sTime();
#endif /* 0 */
  i_punct = 0;
  while (i_in<bit_stream->length) {
#if 0
    /* FIXME */
    /* Time varies between runs giving false negatives about which run
       (JIT or LLC) failed */
    if (i_in && (i_in % 10000 == 0)) {
      now = sTime();
      estTotal = ((double)1.02*bit_stream->length/((double)i_in))*(now-startTime);
      printf("Viterbi: Estimate %1.1lf%% complete (%1.1lf seconds / %1.1lf seconds)...\n",
             (double)100.0*i_in/(1.02*bit_stream->length), now-startTime, estTotal);
      fflush(stdout);
    }
#endif /* 0 */
    memcpy(Metr0, Metr, Metr_mem->length*sizeof(double));
    memcpy(Metr1, Metr, Metr_mem->length*sizeof(double));
    
    if (param->punct1[i_punct] == 1) {
      for (i=0; i<param->Nways; ++i) {
        X0 = param->Tabl_X[i];
        X1 = 1 - param->Tabl_X[i];
        Metr0[i] = Metr0[i] + Dist->data[X0].data[i_in];
        Metr1[i] = Metr1[i] + Dist->data[X1].data[i_in];
      }
      ++i_in;
    }
    
    if (param->punct2[i_punct] == 1) {
      if (i_in >= bit_stream->length) {
        assert(0 && "Synchronization in Viterbi: i_in > size(Dist,2)");
      }
      for (i=0; i<param->Nways; ++i) {
        Y0 = param->Tabl_Y[i];        
        Y1 = 1 - param->Tabl_Y[i];    
        Metr0[i] = Metr0[i] + Dist->data[Y0].data[i_in];
        Metr1[i] = Metr1[i] + Dist->data[Y1].data[i_in];
      }
      ++i_in;
    }
    
    for (i=0; i<param->Nways/2; ++i) {
      if (Metr0[2*i] <= Metr0[2*i+1]) {
        Metr[i] = Metr0[2*i];
        history_new[i][0] = 0;
        for (j=0; j<MAX_history; ++j) {
          history_new[i][j+1] = history[2*i][j];
        }
      } else {
        Metr[i] = Metr0[2*i+1];
        history_new[i][0] = 1;
        for (j=0; j<MAX_history; ++j) {
          history_new[i][j+1] = history[2*i+1][j];
        }
      }
      if (Metr1[2*i] <= Metr1[2*i+1]) {
        Metr[i+param->Nways/2] = Metr1[2*i];
        history_new[i+param->Nways/2][0] = 0;
        for (j=0; j<MAX_history; ++j) {
          history_new[i+param->Nways/2][j+1] = history[2*i][j];
        }
      } else {
        Metr[i+param->Nways/2] = Metr1[2*i+1];
        history_new[i+param->Nways/2][0] = 1;
        for (j=0; j<MAX_history; ++j) {
          history_new[i+param->Nways/2][j+1] = history[2*i+1][j];
        }
      }
    }
    
    for (vote=i=0; i<param->Nways; ++i) {
      vote += (history_new[i][MAX_history] == 0);
    }
    
    if (vote >= param->Nways/2) {
      bit_stream->data[bv++] = 0;
    } else {
      bit_stream->data[bv++] = 1;
    }

    for (i=0; i<param->Nways; ++i) {
      for (j=0; j<MAX_history; ++j) {
        history[i][j] = history_new[i][j];
      }
    }

    if (++i_punct == param->n_in) {
      i_punct = 0;                 
    }
  }

  bit_stream->data = (unsigned char*)realloc(bit_stream->data, bv*sizeof(unsigned char));
  bit_stream->length = bv;
  
  memcpy(Metr_mem->data, Metr, Metr_mem->length*sizeof(double));
  memcpy(history_mem, history, sizeof(history));
  
  free(Metr);
  free(Metr0);
  free(Metr1);
}

