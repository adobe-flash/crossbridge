#ifndef __COMMON_H__
#define __COMMON_H__

#include <stddef.h>
#include <time.h>
#include <stdlib.h>

#if !defined(_MSC_VER) && !defined(__MINGW32__)
#  include <sys/time.h>
#endif

/* Structures */
typedef struct {
  size_t length;
  double* data;
} dvector;

typedef struct {
  size_t length;
  unsigned char* data;
} bitvector;

typedef struct {
  size_t count;
  dvector* data;
} dvarray;

typedef enum
{
    CODERATE_UNDEF = 0,
    CODERATE_1D2   = 1,
    CODERATE_2D3   = 2,
    CODERATE_3D4   = 3,
    CODERATE_5D6   = 4,
    CODERATE_7D8   = 5
} coderate_tt;

#define MAX_Nways   128
#define MAX_punct   7
#define MAX_history 150-7

typedef struct 
{
  size_t Nbits;
  size_t Nways;
  size_t Delay;
  unsigned char punct1[MAX_punct];
  unsigned char punct2[MAX_punct];
  size_t n_in;
  unsigned char Tabl_X[MAX_Nways];
  unsigned char Tabl_Y[MAX_Nways];
  double Metr[MAX_Nways];
  unsigned char history[MAX_Nways][MAX_history];
} param_viterbi_t;

/* Forward declarations */

static inline void dvarray_init(dvarray* array, size_t count, size_t vlength);
static inline void dvarray_clear(dvarray* array);

/* Inline functions */

static inline void dvector_clear(dvector* vector) 
{
  free(vector->data);
  vector->length = 0;
  vector->data = NULL;
}

static inline void bitvector_clear(bitvector* vector) 
{
  free(vector->data);
  vector->length = 0;
  vector->data = NULL;
}

static inline void dvector_init(dvector* vector, size_t length) 
{
  vector->length = length;
  if (length) {
    vector->data = (double*)malloc(length*sizeof(double));
    memset(vector->data, 0, length*sizeof(double));
  } else {
    vector->data = NULL;
  }
}

static inline void bitvector_init(bitvector* vector, size_t length) 
{
  vector->length = length;
  if (length) {
    vector->data = (unsigned char*)malloc(length*sizeof(unsigned char));
    memset(vector->data, 0, length*sizeof(char));
  } else {
    vector->data = NULL;
  }
}

static inline void dvarray_init(dvarray* array, size_t count, size_t vlength) 
{
  size_t i;
  
  array->count = count;
  if (count) {
    array->data = (dvector*)malloc(count*sizeof(dvector));
  } else {
    array->data = NULL;
  }
  
  for (i=0; i<count; ++i) {
    dvector_init(&array->data[i], vlength);
  }
}

static inline void dvarray_clear(dvarray* array) 
{
  size_t i;
  
  for (i=0; i<array->count; ++i) {
    dvector_clear(&array->data[i]);
  }

  free(array->data);

  array->count = 0;
  array->data = NULL;
}


static inline double sTime()
#if !defined(_MSC_VER) && !defined(__MINGW32__)
{ static struct timeval  this_tv;
  static struct timezone dumbTZ;
  double t;
                                                                                                                                                         
  gettimeofday(&this_tv, &dumbTZ);
  t = this_tv.tv_sec + 0.000001*this_tv.tv_usec;
  return t;
}
#else
{
  return clock() / (double)CLOCKS_PER_SEC;
}
#endif

#endif /* __COMMON_H__ */
