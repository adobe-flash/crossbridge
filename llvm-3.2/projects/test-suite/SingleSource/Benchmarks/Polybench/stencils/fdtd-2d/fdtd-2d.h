/**
 * fdtd-2d.h: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#ifndef FDTD_2D_H
# define FDTD_2D_H

/* Default to STANDARD_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define STANDARD_DATASET
# endif

/* Do not define anything if the user manually defines the size. */
# if !defined(NX) && ! defined(NY) && !defined(TMAX)
/* Define the possible dataset sizes. */
#  ifdef MINI_DATASET
#   define TMAX 2
#   define NX 32
#   define NY 32
#  endif

#  ifdef SMALL_DATASET
#   define TMAX 10
#   define NX 500
#   define NY 500
#  endif

#  ifdef STANDARD_DATASET /* Default if unspecified. */
#   define TMAX 50
#   define NX 1000
#   define NY 1000
#  endif

#  ifdef LARGE_DATASET
#   define TMAX 50
#   define NX 2000
#   define NY 2000
#  endif

#  ifdef EXTRALARGE_DATASET
#   define TMAX 100
#   define NX 4000
#   define NY 4000
#  endif
# endif /* !N */

# define _PB_TMAX POLYBENCH_LOOP_BOUND(TMAX,tmax)
# define _PB_NX POLYBENCH_LOOP_BOUND(NX,nx)
# define _PB_NY POLYBENCH_LOOP_BOUND(NY,ny)

# ifndef DATA_TYPE
#  define DATA_TYPE double
#  define DATA_PRINTF_MODIFIER "%0.2lf "
# endif


#endif /* !FDTD_2D */
