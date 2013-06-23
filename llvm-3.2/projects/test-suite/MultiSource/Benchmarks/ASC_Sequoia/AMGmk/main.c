/*BHEADER****************************************************************
 * (c) 2007   The Regents of the University of California               *
 *                                                                      *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright       *
 * notice and disclaimer.                                               *
 *                                                                      *
 *EHEADER****************************************************************/

//--------------
//  A micro kernel 
//--------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "headers.h"

//
#ifdef SMALL_PROBLEM_SIZE
const int testIter   = 100;
#else
const int testIter   = 2000;
#endif
double totalWallTime = 0.0;
double totalCPUTime  = 0.0;

// 
void test_Matvec();
void test_Relax();
void test_Axpy();

//
int main(int argc, char *argv[])
{
  struct timeval  t0, t1;
  clock_t t0_cpu = 0,
          t1_cpu = 0;

  double del_wtime = 0.0;


  printf("\n");
  printf("//------------ \n");
  printf("// \n");
  printf("//  Sequoia Benchmark Version 1.0 \n");
  printf("// \n");
  printf("//------------ \n");

  gettimeofday(&t0, ((void *)0));
  t0_cpu = clock();

  // Matvec
  totalWallTime = 0.0;
  totalCPUTime  = 0.0;

  test_Matvec();

  printf("\n");
  printf("//------------ \n");
  printf("// \n");
  printf("//   MATVEC\n");
  printf("// \n");
  printf("//------------ \n");

//  printf("\nTotal Wall time = %f seconds. \n", totalWallTime);

//  printf("\nTotal CPU  time = %f seconds. \n\n", totalCPUTime);

  // Relax
  totalWallTime = 0.0;
  totalCPUTime  = 0.0;

  test_Relax();

  printf("\n");
  printf("//------------ \n");
  printf("// \n");
  printf("//   Relax\n");
  printf("// \n");
  printf("//------------ \n");

//  printf("\nTotal Wall time = %f seconds. \n", totalWallTime);

//  printf("\nTotal CPU  time = %f seconds. \n\n", totalCPUTime);

  // Axpy
  totalWallTime = 0.0;
  totalCPUTime  = 0.0;

  test_Axpy();

  printf("\n");
  printf("//------------ \n");
  printf("// \n");
  printf("//   Axpy\n");
  printf("// \n");
  printf("//------------ \n");

//  printf("\nTotal Wall time = %f seconds. \n", totalWallTime);

//  printf("\nTotal CPU  time = %f seconds. \n\n", totalCPUTime);


  gettimeofday(&t1, ((void *)0));
  t1_cpu = clock();

  del_wtime = (double)(t1.tv_sec - t0.tv_sec) +
              (double)(t1.tv_usec - t0.tv_usec)/1000000.0;

//  printf("\nTotal Wall time = %f seconds. \n", del_wtime);
//  printf("\nTotal CPU  time = %f seconds. \n", ((double) (t1_cpu - t0_cpu))/CLOCKS_PER_SEC);

  return  0;

}

void test_Matvec()
{
  struct timeval  t0, t1;
  clock_t t0_cpu = 0,
          t1_cpu = 0;

  hypre_CSRMatrix *A;
  hypre_Vector *x, *y, *sol;
  int nx, ny, nz, i;
  double *values;
  double *y_data, *sol_data;
  double error, diff;

  nx = 50;  /* size per proc nx*ny*nz */
  ny = 50;
  nz = 50;

  values = hypre_CTAlloc(double, 4);
  values[0] = 6; 
  values[1] = -1;
  values[2] = -1;
  values[3] = -1;

  A = GenerateSeqLaplacian(nx, ny, nz, values, &y, &x, &sol);

  hypre_SeqVectorSetConstantValues(x,1);
  hypre_SeqVectorSetConstantValues(y,0);

  gettimeofday(&t0, ((void *)0));
  t0_cpu = clock();

  for (i=0; i<testIter; ++i)
      hypre_CSRMatrixMatvec(1,A,x,0,y);

  gettimeofday(&t1, ((void *)0));
  t1_cpu = clock();

  totalWallTime += (double)(t1.tv_sec - t0.tv_sec) +
                   (double)(t1.tv_usec - t0.tv_usec)/1000000.0;

  totalCPUTime  += ((double) (t1_cpu - t0_cpu))/CLOCKS_PER_SEC;

  y_data = hypre_VectorData(y);
  sol_data = hypre_VectorData(sol);

  error = 0;
  for (i=0; i < nx*ny*nz; i++)
  {
      diff = fabs(y_data[i]-sol_data[i]);
      if (diff > error) error = diff;
  }
     
  if (error > 0) printf(" \n Matvec: error: %e\n", error);

  hypre_TFree(values);
  hypre_CSRMatrixDestroy(A);
  hypre_SeqVectorDestroy(x);
  hypre_SeqVectorDestroy(y);
  hypre_SeqVectorDestroy(sol);

}

void test_Relax()
{
  struct timeval  t0, t1;
  clock_t t0_cpu = 0,
          t1_cpu = 0;

  hypre_CSRMatrix *A;
  hypre_Vector *x, *y, *sol;
  int nx, ny, nz, i;
  double *values;
  double *x_data;
  double diff, error;

  nx = 50;  /* size per proc nx*ny*nz */
  ny = 50;
  nz = 50;

  values = hypre_CTAlloc(double, 4);
  values[0] = 6; 
  values[1] = -1;
  values[2] = -1;
  values[3] = -1;

  A = GenerateSeqLaplacian(nx, ny, nz, values, &y, &x, &sol);

  hypre_SeqVectorSetConstantValues(x,1);

  gettimeofday(&t0, ((void *)0));
  t0_cpu = clock();

  for (i=0; i<testIter; ++i)
      hypre_BoomerAMGSeqRelax(A, sol, x);

  gettimeofday(&t1, ((void *)0));
  t1_cpu = clock();

  totalWallTime += (double)(t1.tv_sec - t0.tv_sec) +
                   (double)(t1.tv_usec - t0.tv_usec)/1000000.0;

  totalCPUTime  += ((double) (t1_cpu - t0_cpu))/CLOCKS_PER_SEC;

  x_data = hypre_VectorData(x);
  error = 0;
  for (i=0; i < nx*ny*nz; i++)
  {
      diff = fabs(x_data[i]-1);
      if (diff > error) error = diff;
  }
     
  if (error > 0) printf(" \n Relax: error: %e\n", error);

  hypre_TFree(values);
  hypre_CSRMatrixDestroy(A);
  hypre_SeqVectorDestroy(x);
  hypre_SeqVectorDestroy(y);
  hypre_SeqVectorDestroy(sol);

}

void test_Axpy()
{
  struct timeval  t0, t1;
  clock_t t0_cpu = 0,
          t1_cpu = 0;

  hypre_Vector *x, *y;
  int nx, i;
  double alpha=0.5;
  double diff, error;
  double *y_data;

  nx = 125000;  /* size per proc  */

  x = hypre_SeqVectorCreate(nx);
  y = hypre_SeqVectorCreate(nx);

  hypre_SeqVectorInitialize(x);
  hypre_SeqVectorInitialize(y);

  hypre_SeqVectorSetConstantValues(x,1);
  hypre_SeqVectorSetConstantValues(y,1);

  gettimeofday(&t0, ((void *)0));
  t0_cpu = clock();

  for (i=0; i<testIter; ++i)
      hypre_SeqVectorAxpy(alpha,x,y);

  gettimeofday(&t1, ((void *)0));
  t1_cpu = clock();

  y_data = hypre_VectorData(y);
  error = 0;
  for (i=0; i < nx; i++)
  {
    diff = fabs(y_data[i]-1-0.5*(double)testIter);
      if (diff > error) error = diff;
  }
     
  if (error > 0) printf(" \n Axpy: error: %e\n", error);

  totalWallTime += (double)(t1.tv_sec - t0.tv_sec) +
                   (double)(t1.tv_usec - t0.tv_usec)/1000000.0;

  totalCPUTime  += ((double) (t1_cpu - t0_cpu))/CLOCKS_PER_SEC;

  hypre_SeqVectorDestroy(x);
  hypre_SeqVectorDestroy(y);

}

