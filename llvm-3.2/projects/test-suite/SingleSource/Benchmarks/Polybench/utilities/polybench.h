/**
 * polybench.h: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
/*
 * Polybench header for instrumentation.
 *
 * Programs must be compiled with `-I utilities utilities/polybench.c'
 *
 * Optionally, one can define:
 *
 * -DPOLYBENCH_TIME, to report the execution time,
 *   OR (exclusive):
 * -DPOLYBENCH_PAPI, to use PAPI H/W counters (defined in polybench.c)
 *
 *
 * See README or utilities/polybench.c for additional options.
 *
 */
#ifndef POLYBENCH_H
# define POLYBENCH_H

# include <stdlib.h>

/* Array padding. By default, none is used. */
# ifndef POLYBENCH_PADDING_FACTOR
/* default: */
#  define POLYBENCH_PADDING_FACTOR 0
# endif


/* C99 arrays in function prototype. By default, do not use. */
# ifdef POLYBENCH_USE_C99_PROTO
#  define POLYBENCH_C99_SELECT(x,y) y
# else
/* default: */
#  define POLYBENCH_C99_SELECT(x,y) x
# endif


/* Scalar loop bounds in SCoPs. By default, use parametric loop bounds. */
# ifdef POLYBENCH_USE_SCALAR_LB
#  define POLYBENCH_LOOP_BOUND(x,y) x
# else
/* default: */
#  define POLYBENCH_LOOP_BOUND(x,y) y
# endif


/* Macros to reference an array. Generic for heap and stack arrays
   (C99).  Each array dimensionality has his own macro, to be used at
   declaration or as a function argument.
   Example:
   int b[x] => POLYBENCH_1D_ARRAY(b, x)
   int A[N][N] => POLYBENCH_2D_ARRAY(A, N, N)
*/
# ifndef POLYBENCH_STACK_ARRAYS
#  define POLYBENCH_ARRAY(x) *x
#  define POLYBENCH_FREE_ARRAY(x) free((void*)x);
#  define POLYBENCH_DECL_VAR(x) (*x)
# else
#  define POLYBENCH_ARRAY(x) x
#  define POLYBENCH_FREE_ARRAY(x)
#  define POLYBENCH_DECL_VAR(x) x
# endif
/* Macros for using arrays in the function prototypes. */
# define POLYBENCH_1D(var, dim1,ddim1) var[POLYBENCH_C99_SELECT(dim1,ddim1) + POLYBENCH_PADDING_FACTOR]
# define POLYBENCH_2D(var, dim1, dim2, ddim1, ddim2) var[POLYBENCH_C99_SELECT(dim1,ddim1) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim2,ddim2) + POLYBENCH_PADDING_FACTOR]
# define POLYBENCH_3D(var, dim1, dim2, dim3, ddim1, ddim2, ddim3) var[POLYBENCH_C99_SELECT(dim1,ddim1) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim2,ddim2) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim3,ddim3) + POLYBENCH_PADDING_FACTOR]
# define POLYBENCH_4D(var, dim1, dim2, dim3, dim4, ddim1, ddim2, ddim3, ddim4) var[POLYBENCH_C99_SELECT(dim1,ddim1) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim2,ddim2) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim3,ddim3) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim4,ddim4) + POLYBENCH_PADDING_FACTOR]
# define POLYBENCH_5D(var, dim1, dim2, dim3, dim4, dim5, ddim1, ddim2, ddim3, ddim4, ddim5) var[POLYBENCH_C99_SELECT(dim1,ddim1) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim2,ddim2) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim3,ddim3) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim4,ddim4) + POLYBENCH_PADDING_FACTOR][POLYBENCH_C99_SELECT(dim5,ddim5) + POLYBENCH_PADDING_FACTOR]


/* Macros to allocate heap arrays.
   Example:
   polybench_alloc_2d_array(N, M, double) => allocates N x M x sizeof(double)
					  and returns a pointer to the 2d array
 */
# define POLYBENCH_ALLOC_1D_ARRAY(n1, type)	\
  (type(*)[n1 + POLYBENCH_PADDING_FACTOR])polybench_alloc_data (n1 + POLYBENCH_PADDING_FACTOR, sizeof(type))
# define POLYBENCH_ALLOC_2D_ARRAY(n1, n2, type)		\
  (type(*)[n1 + POLYBENCH_PADDING_FACTOR][n2 + POLYBENCH_PADDING_FACTOR])polybench_alloc_data ((n1 + POLYBENCH_PADDING_FACTOR) * (n2 + POLYBENCH_PADDING_FACTOR), sizeof(type))
# define POLYBENCH_ALLOC_3D_ARRAY(n1, n2, n3, type)		\
  (type(*)[n1 + POLYBENCH_PADDING_FACTOR][n2 + POLYBENCH_PADDING_FACTOR][n3 + POLYBENCH_PADDING_FACTOR])polybench_alloc_data ((n1 + POLYBENCH_PADDING_FACTOR) * (n2 + POLYBENCH_PADDING_FACTOR) * (n3 + POLYBENCH_PADDING_FACTOR), sizeof(type))
# define POLYBENCH_ALLOC_4D_ARRAY(n1, n2, n3, n4, type)			\
  (type(*)[n1 + POLYBENCH_PADDING_FACTOR][n2 + POLYBENCH_PADDING_FACTOR][n3 + POLYBENCH_PADDING_FACTOR][n4 + POLYBENCH_PADDING_FACTOR])polybench_alloc_data ((n1 + POLYBENCH_PADDING_FACTOR) * (n2 + POLYBENCH_PADDING_FACTOR) * (n3 + POLYBENCH_PADDING_FACTOR) * (n4 + POLYBENCH_PADDING_FACTOR), sizeof(type))
# define POLYBENCH_ALLOC_5D_ARRAY(n1, n2, n3, n4, n5, type)		\
  (type(*)[n1 + POLYBENCH_PADDING_FACTOR][n2 + POLYBENCH_PADDING_FACTOR][n3 + POLYBENCH_PADDING_FACTOR][n4 + POLYBENCH_PADDING_FACTOR][n5 + POLYBENCH_PADDING_FACTOR])polybench_alloc_data ((n1 + POLYBENCH_PADDING_FACTOR) * (n2 + POLYBENCH_PADDING_FACTOR) * (n3 + POLYBENCH_PADDING_FACTOR) * (n4 + POLYBENCH_PADDING_FACTOR) * (n5 + POLYBENCH_PADDING_FACTOR), sizeof(type))

/* Macros for array declaration. */
# ifndef POLYBENCH_STACK_ARRAYS
#  define POLYBENCH_1D_ARRAY_DECL(var, type, dim1, ddim1)		\
  type POLYBENCH_1D(POLYBENCH_DECL_VAR(var), dim1, ddim1); \
  var = POLYBENCH_ALLOC_1D_ARRAY(POLYBENCH_C99_SELECT(dim1, ddim1), type);
#  define POLYBENCH_2D_ARRAY_DECL(var, type, dim1, dim2, ddim1, ddim2)	\
  type POLYBENCH_2D(POLYBENCH_DECL_VAR(var), dim1, dim2, ddim1, ddim2); \
  var = POLYBENCH_ALLOC_2D_ARRAY(POLYBENCH_C99_SELECT(dim1, ddim1), POLYBENCH_C99_SELECT(dim2, ddim2), type);
#  define POLYBENCH_3D_ARRAY_DECL(var, type, dim1, dim2, dim3, ddim1, ddim2, ddim3) \
  type POLYBENCH_3D(POLYBENCH_DECL_VAR(var), dim1, dim2, dim3, ddim1, ddim2, ddim3); \
  var = POLYBENCH_ALLOC_3D_ARRAY(POLYBENCH_C99_SELECT(dim1, ddim1), POLYBENCH_C99_SELECT(dim2, ddim2), POLYBENCH_C99_SELECT(dim3, ddim3), type);
#  define POLYBENCH_4D_ARRAY_DECL(var, type, dim1, dim2, dim3, dim4, ddim1, ddim2, ddim3, ddim4) \
  type POLYBENCH_4D(POLYBENCH_DECL_VAR(var), dim1, dim2, ,dim3, dim4, ddim1, ddim2, ddim3, ddim4); \
  var = POLYBENCH_ALLOC_4D_ARRAY(POLYBENCH_C99_SELECT(dim1, ddim1), POLYBENCH_C99_SELECT(dim2, ddim2), POLYBENCH_C99_SELECT(dim3, ddim3), POLYBENCH_C99_SELECT(dim4, ddim4), type);
#  define POLYBENCH_5D_ARRAY_DECL(var, type, dim1, dim2, dim3, dim4, dim5, ddim1, ddim2, ddim3, ddim4, ddim5) \
  type POLYBENCH_5D(POLYBENCH_DECL_VAR(var), dim1, dim2, dim3, dim4, dim5, ddim1, ddim2, ddim3, ddim4, ddim5); \
  var = POLYBENCH_ALLOC_5D_ARRAY(POLYBENCH_C99_SELECT(dim1, ddim1), POLYBENCH_C99_SELECT(dim2, ddim2), POLYBENCH_C99_SELECT(dim3, ddim3), POLYBENCH_C99_SELECT(dim4, ddim4), POLYBENCH_C99_SELECT(dim5, ddim5), type);
# else
#  define POLYBENCH_1D_ARRAY_DECL(var, type, dim1, ddim1)		\
  type POLYBENCH_1D(POLYBENCH_DECL_VAR(var), dim1, ddim1);
#  define POLYBENCH_2D_ARRAY_DECL(var, type, dim1, dim2, ddim1, ddim2)	\
  type POLYBENCH_2D(POLYBENCH_DECL_VAR(var), dim1, dim2, ddim1, ddim2);
#  define POLYBENCH_3D_ARRAY_DECL(var, type, dim1, dim2, dim3, ddim1, ddim2, ddim3) \
  type POLYBENCH_3D(POLYBENCH_DECL_VAR(var), dim1, dim2, dim3, ddim1, ddim2, ddim3);
#  define POLYBENCH_4D_ARRAY_DECL(var, type, dim1, dim2, dim3, dim4, ddim1, ddim2, ddim3, ddim4) \
  type POLYBENCH_4D(POLYBENCH_DECL_VAR(var), dim1, dim2, dim3, dim4, ddim1, ddim2, ddim3, ddim4);
#  define POLYBENCH_5D_ARRAY_DECL(var, type, dim1, dim2, dim3, dim4, dim5, ddim1, ddim2, ddim3, ddim4, ddim5) \
  type POLYBENCH_5D(POLYBENCH_DECL_VAR(var), dim1, dim2, dim3, dim4, dim5, ddim1, ddim2, ddim3, ddim4, ddim5);
# endif


/* Dead-code elimination macros. Use argc/argv for the run-time check. */
# ifndef POLYBENCH_DUMP_ARRAYS
#  define POLYBENCH_DCE_ONLY_CODE    if (argc > 42 && ! strcmp(argv[0], ""))
# else
#  define POLYBENCH_DCE_ONLY_CODE
# endif

# define polybench_prevent_dce(func)		\
  POLYBENCH_DCE_ONLY_CODE			\
  func


/* Performance-related instrumentation. See polybench.c */
# define polybench_start_instruments
# define polybench_stop_instruments
# define polybench_print_instruments


/* PAPI support. */
# ifdef POLYBENCH_PAPI
extern const unsigned int polybench_papi_eventlist[];
#  undef polybench_start_instruments
#  undef polybench_stop_instruments
#  undef polybench_print_instruments
#  define polybench_set_papi_thread_report(x)	\
   polybench_papi_counters_threadid = x;
#  define polybench_start_instruments				\
  polybench_prepare_instruments();				\
  polybench_papi_init();					\
  int evid;							\
  for (evid = 0; polybench_papi_eventlist[evid] != 0; evid++)	\
    {								\
      if (polybench_papi_start_counter(evid))			\
	continue;						\

#  define polybench_stop_instruments		\
      polybench_papi_stop_counter(evid);	\
    }						\
  polybench_papi_close();			\

#  define polybench_print_instruments polybench_papi_print();
# endif


/* Timing support. */
# if defined(POLYBENCH_TIME) || defined(POLYBENCH_GFLOPS)
#  undef polybench_start_instruments
#  undef polybench_stop_instruments
#  undef polybench_print_instruments
#  define polybench_start_instruments polybench_timer_start();
#  define polybench_stop_instruments polybench_timer_stop();
#  define polybench_print_instruments polybench_timer_print();
extern double polybench_program_total_flops;
extern void polybench_timer_start();
extern void polybench_timer_stop();
extern void polybench_timer_print();
# endif

/* Function declaration. */
# ifdef POLYBENCH_TIME
extern void polybench_timer_start();
extern void polybench_timer_stop();
extern void polybench_timer_print();
# endif

# ifdef POLYBENCH_PAPI
extern void polybench_prepare_instruments();
extern int polybench_papi_start_counter(int evid);
extern void polybench_papi_stop_counter(int evid);
extern void polybench_papi_init();
extern void polybench_papi_close();
extern void polybench_papi_print();
# endif

/* Function prototypes. */
extern void* polybench_alloc_data(unsigned long long int n, int elt_size);

/*
 LLVM: I'm appending the content of the file polybench.c here. It'll avoid us
       to have to copy it to the folder being compiled in the LLVM test suite.
 */

/**
 * polybench.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <math.h>
#ifdef _OPENMP
# include <omp.h>
#endif

/* By default, collect PAPI counters on thread 0. */
#ifndef POLYBENCH_THREAD_MONITOR
# define POLYBENCH_THREAD_MONITOR 0
#endif

/* Total LLC cache size. By default 32+MB.. */
#ifndef POLYBENCH_CACHE_SIZE_KB
# define POLYBENCH_CACHE_SIZE_KB 32770
#endif


int polybench_papi_counters_threadid = POLYBENCH_THREAD_MONITOR;
double polybench_program_total_flops = 0;

#ifdef POLYBENCH_PAPI
# include <papi.h>
# define POLYBENCH_MAX_NB_PAPI_COUNTERS 96
char* _polybench_papi_eventlist[] = {
#include "papi_counters.list"
  NULL
};
int polybench_papi_eventset;
int polybench_papi_eventlist[POLYBENCH_MAX_NB_PAPI_COUNTERS];
long_long polybench_papi_values[POLYBENCH_MAX_NB_PAPI_COUNTERS];

#endif


/* Timer code (gettimeofday). */
double polybench_t_start, polybench_t_end;
/* Timer code (RDTSC). */
unsigned long long int polybench_c_start, polybench_c_end;

static
double rtclock()
{
#ifdef POLYBENCH_TIME
  struct timeval Tp;
  int stat;
  stat = gettimeofday (&Tp, NULL);
  if (stat != 0)
    printf ("Error return from gettimeofday: %d", stat);
  return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
#else
  return 0;
#endif
}


#ifdef POLYBENCH_CYCLE_ACCURATE_TIMER
static
unsigned long long int rdtsc()
{
  unsigned long long int ret = 0;
  unsigned int cycles_lo;
  unsigned int cycles_hi;
  __asm__ volatile ("RDTSC" : "=a" (cycles_lo), "=d" (cycles_hi));
  ret = (unsigned long long int)cycles_hi << 32 | cycles_lo;
  
  return ret;
}
#endif

void polybench_flush_cache()
{
  int cs = POLYBENCH_CACHE_SIZE_KB * 1024 / sizeof(double);
  double* flush = (double*) calloc (cs, sizeof(double));
  int i;
  double tmp = 0.0;
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (i = 0; i < cs; i++)
    tmp += flush[i];
  assert (tmp <= 10.0);
  free (flush);
}


#ifdef POLYBENCH_LINUX_FIFO_SCHEDULER
void polybench_linux_fifo_scheduler()
{
  /* Use FIFO scheduler to limit OS interference. Program must be run
   as root, and this works only for Linux kernels. */
  struct sched_param schedParam;
  schedParam.sched_priority = sched_get_priority_max (SCHED_FIFO);
  sched_setscheduler (0, SCHED_FIFO, &schedParam);
}


void polybench_linux_standard_scheduler()
{
  /* Restore to standard scheduler policy. */
  struct sched_param schedParam;
  schedParam.sched_priority = sched_get_priority_max (SCHED_OTHER);
  sched_setscheduler (0, SCHED_OTHER, &schedParam);
}
#endif

#ifdef POLYBENCH_PAPI

static
void test_fail(char *file, int line, char *call, int retval)
{
  char buf[128];
  
  memset(buf, '\0', sizeof(buf));
  if (retval != 0)
    fprintf (stdout,"%-40s FAILED\nLine # %d\n", file, line);
  else
  {
    fprintf (stdout,"%-40s SKIPPED\n", file);
    fprintf (stdout,"Line # %d\n", line);
  }
  if (retval == PAPI_ESYS)
  {
    sprintf (buf, "System error in %s", call);
    perror (buf);
  }
  else if (retval > 0)
    fprintf (stdout,"Error: %s\n", call);
  else if (retval == 0)
    fprintf (stdout,"Error: %s\n", call);
  else
  {
    char errstring[PAPI_MAX_STR_LEN];
    PAPI_perror (retval, errstring, PAPI_MAX_STR_LEN);
    fprintf (stdout,"Error in %s: %s\n", call, errstring);
  }
  fprintf (stdout,"\n");
  if (PAPI_is_initialized ())
    PAPI_shutdown ();
  exit (1);
}


void polybench_papi_init()
{
# ifdef _OPENMP
#pragma omp parallel
  {
#pragma omp master
    {
      if (omp_get_max_threads () < polybench_papi_counters_threadid)
        polybench_papi_counters_threadid = omp_get_max_threads () - 1;
    }
#pragma omp barrier
    
    if (omp_get_thread_num () == polybench_papi_counters_threadid)
    {
# endif
      int retval;
      polybench_papi_eventset = PAPI_NULL;
      if ((retval = PAPI_library_init (PAPI_VER_CURRENT)) != PAPI_VER_CURRENT)
        test_fail (__FILE__, __LINE__, "PAPI_library_init", retval);
      if ((retval = PAPI_create_eventset (&polybench_papi_eventset))
          != PAPI_OK)
        test_fail (__FILE__, __LINE__, "PAPI_create_eventset", retval);
      int k;
      for (k = 0; _polybench_papi_eventlist[k]; ++k)
      {
        if ((retval =
             PAPI_event_name_to_code (_polybench_papi_eventlist[k],
                                      &(polybench_papi_eventlist[k])))
            != PAPI_OK)
          test_fail (__FILE__, __LINE__, "PAPI_event_name_to_code", retval);
      }
      polybench_papi_eventlist[k] = 0;
      
      
# ifdef _OPENMP
    }
  }
#pragma omp barrier
# endif
}


void polybench_papi_close()
{
# ifdef _OPENMP
#pragma omp parallel
  {
    if (omp_get_thread_num () == polybench_papi_counters_threadid)
    {
# endif
      int retval;
      if ((retval = PAPI_destroy_eventset (&polybench_papi_eventset))
          != PAPI_OK)
        test_fail (__FILE__, __LINE__, "PAPI_destroy_eventset", retval);
      if (PAPI_is_initialized ())
        PAPI_shutdown ();
# ifdef _OPENMP
    }
  }
#pragma omp barrier
# endif
}

int polybench_papi_start_counter(int evid)
{
# ifndef POLYBENCH_NO_FLUSH_CACHE
  polybench_flush_cache();
# endif
  
# ifdef _OPENMP
# pragma omp parallel
  {
    if (omp_get_thread_num () == polybench_papi_counters_threadid)
    {
# endif
      
      int retval = 1;
      char descr[PAPI_MAX_STR_LEN];
      PAPI_event_info_t evinfo;
      PAPI_event_code_to_name (polybench_papi_eventlist[evid], descr);
      if (PAPI_add_event (polybench_papi_eventset,
                          polybench_papi_eventlist[evid]) != PAPI_OK)
        test_fail (__FILE__, __LINE__, "PAPI_add_event", 1);
      if (PAPI_get_event_info (polybench_papi_eventlist[evid], &evinfo)
          != PAPI_OK)
        test_fail (__FILE__, __LINE__, "PAPI_get_event_info", retval);
      if ((retval = PAPI_start (polybench_papi_eventset)) != PAPI_OK)
        test_fail (__FILE__, __LINE__, "PAPI_start", retval);
# ifdef _OPENMP
    }
  }
#pragma omp barrier
# endif
  return 0;
}


void polybench_papi_stop_counter(int evid)
{
# ifdef _OPENMP
# pragma omp parallel
  {
    if (omp_get_thread_num () == polybench_papi_counters_threadid)
    {
# endif
      int retval;
      long_long values[1];
      values[0] = 0;
      if ((retval = PAPI_read (polybench_papi_eventset, &values[0]))
          != PAPI_OK)
        test_fail (__FILE__, __LINE__, "PAPI_read", retval);
      
      if ((retval = PAPI_stop (polybench_papi_eventset, NULL)) != PAPI_OK)
        test_fail (__FILE__, __LINE__, "PAPI_stop", retval);
      
      polybench_papi_values[evid] = values[0];
      
      if ((retval = PAPI_remove_event
           (polybench_papi_eventset,
            polybench_papi_eventlist[evid])) != PAPI_OK)
        test_fail (__FILE__, __LINE__, "PAPI_remove_event", retval);
# ifdef _OPENMP
    }
  }
#pragma omp barrier
# endif
}


void polybench_papi_print()
{
  int verbose = 0;
# ifdef _OPENMP
# pragma omp parallel
  {
    if (omp_get_thread_num() == polybench_papi_counters_threadid)
    {
#ifdef POLYBENCH_PAPI_VERBOSE
      verbose = 1;
#endif
      if (verbose)
        printf ("On thread %d:\n", polybench_papi_counters_threadid);
#endif
      int evid;
      for (evid = 0; polybench_papi_eventlist[evid] != 0; ++evid)
      {
        if (verbose)
          printf ("%s=", _polybench_papi_eventlist[evid]);
        printf ("%llu ", polybench_papi_values[evid]);
        if (verbose)
          printf ("\n");
      }
      printf ("\n");
# ifdef _OPENMP
    }
  }
#pragma omp barrier
# endif
}

#endif
/* ! POLYBENCH_PAPI */

void polybench_prepare_instruments()
{
#ifndef POLYBENCH_NO_FLUSH_CACHE
  polybench_flush_cache ();
#endif
#ifdef POLYBENCH_LINUX_FIFO_SCHEDULER
  polybench_linux_fifo_scheduler ();
#endif
}


void polybench_timer_start()
{
  polybench_prepare_instruments ();
#ifndef POLYBENCH_CYCLE_ACCURATE_TIMER
  polybench_t_start = rtclock ();
#else
  polybench_c_start = rdtsc ();
#endif
}


void polybench_timer_stop()
{
#ifndef POLYBENCH_CYCLE_ACCURATE_TIMER
  polybench_t_end = rtclock ();
#else
  polybench_c_end = rdtsc ();
#endif
#ifdef POLYBENCH_LINUX_FIFO_SCHEDULER
  polybench_linux_standard_scheduler ();
#endif
}


void polybench_timer_print()
{
#ifdef POLYBENCH_GFLOPS
  if  (__polybench_program_total_flops == 0)
	{
	  printf ("[PolyBench][WARNING] Program flops not defined, use polybench_set_program_flops(value)\n");
	  printf ("%0.6lf\n", polybench_t_end - polybench_t_start);
	}
  else
    printf ("%0.2lf\n",
            (__polybench_program_total_flops /
             (double)(polybench_t_end - polybench_t_start)) / 1000000000);
#else
# ifndef POLYBENCH_CYCLE_ACCURATE_TIMER
  printf ("%0.6f\n", polybench_t_end - polybench_t_start);
# else
  printf ("%Ld\n", polybench_c_end - polybench_c_start);
# endif
#endif
}



static
void *
xmalloc (size_t num)
{
  void* new = NULL;
  int ret = posix_memalign (&new, 32, num);
  if (! new || ret)
  {
    fprintf (stderr, "[PolyBench] posix_memalign: cannot allocate memory");
    exit (1);
  }
  return new;
}


void* polybench_alloc_data(unsigned long long int n, int elt_size)
{
  /// FIXME: detect overflow!
  size_t val = n;
  val *= elt_size;
  void* ret = xmalloc (val);
  
  return ret;
}



#endif /* !POLYBENCH_H */
