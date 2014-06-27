
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void *allocvector(size_t);

void  *allocvector(size_t size) 
{
  void *V;

  /*  if (size > sysconf(_SC_PAGESIZE)/2 ) { 
    if ( (V = (void *) valloc(size) ) == NULL ) {
      fprintf(stderr, "Error: couldn't allocate V in allocvector.\n");
      exit(-1);
    }
  }
  else {     */
    if ( (V = (void *) malloc((size_t) size)) == NULL ) {
      fprintf(stderr, "Error: couldn't allocate V in allocvector.\n");
      exit(-1);
    }
    /*  } */
  memset(V,0,size);
  return V;
}


void dallocvector(int n, double **V)
{
  *V = (double *) allocvector((size_t) n*sizeof(double));
}


void sallocvector(int n, float **V)
{
  *V = (float *) allocvector((size_t) n*sizeof(float));
}


void iallocvector(int n, int **V)
{
  *V = (int *)  allocvector((size_t) n*sizeof(int));
}
