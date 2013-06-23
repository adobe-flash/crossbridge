/* Copyright (C) 1989, 1990 Aladdin Enterprises.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* std.h */
/* Standard definitions for Aladdin Enterprises code */

/*
 * Here we deal with the vagaries of various C compilers.  We assume that:
 *	ANSI-standard Unix compilers define __STDC__.
 *	Turbo C defines __MSDOS__.
 *	Microsoft C defines MSDOS.
 * Also, not used much here, but used in other header files, we assume:
 *	Unix System V environments define USG or SYSV.
 *	  (GNU software uses the former, non-GNU tends to use the latter.)
 *	VMS systems define VMS.
 *	bsd 4.2 or 4.3 systems define BSD4_2.
 *	POSIX-compliant environments define _POSIX_SOURCE.
 *
 * We make fairly heroic efforts to confine all uses of these flags to
 * header files, and never to use them in code.
 */

#ifdef __STDC__
#  define STDC_OR_MSDOS /* */
#endif
#ifdef __MSDOS__
#  define STDC_OR_MSDOS /* */
#endif

/* Recognize USG and SYSV as synonyms.  GNU software uses the former, */
/* non-GNU tends to use the latter.  We use the latter. */
#ifdef USG
#  define SYSV /* */
#endif

/* Disable 'const' if the compiler can't handle it. */
#ifndef STDC_OR_MSDOS
#  define const /* */
#endif

/* Disable MS-DOS specialized pointer types on non-MS-DOS systems. */
#ifndef __MSDOS__
#  define near /* */
#  define far /* */
#  define huge /* */
#  define _cs /* */
#  define _ds /* */
/* _es is never safe to use */
#  define _ss /* */
#endif

/* Miscellaneous types */
typedef unsigned char byte;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

/* Maximum values for the unsigned types. */
#include <limits.h>
#define max_uchar UCHAR_MAX
#define max_ushort USHRT_MAX
#define max_uint UINT_MAX
#define max_ulong ULONG_MAX

/* Define a reliable arithmetic right shift. */
#if (-2 >> 1) == -1
#  define arith_rshift(x,n) ((x) >> (n))
#else
#  define arith_rshift(x,n) ((x) < 0 ? ~(~(x) >> (n)) : (x) >> (n))
#endif

/* The type to be used for comparing pointers for order (<, >=, etc.). */
/* Turbo C large model doesn't compare pointers per se correctly. */
#ifdef __MSDOS__
typedef unsigned long ptr_ord_t;
#else
typedef char *ptr_ord_t;
#endif

/* In case stdio.h doesn't have these: */
#ifndef min
#  define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#  define max(a, b) ((a) > (b) ? (a) : (b))
#endif

/* math.h is different for Turbo and Unix.... */
#ifndef M_PI
#  ifdef PI
#    define M_PI PI
#  else
#    define M_PI 3.14159265358979324
#  endif
#endif

/* VMS doesn't have the unlink system call.  Use delete instead. */
#ifdef VMS
#  define unlink(fname) delete(fname)
#endif

/* K&R specifies that float parameters get converted to double. */
/* Most compilers allow a formal parameter to be declared float, */
/* and treat it as though it had been declared double; however, */
/* Turbo C (and perhaps the ANSI standard) doesn't allow this. */
/* Pending a deeper understanding of this issue, we define a */
/* special type for float parameters. */
#ifdef __STDC__
typedef double floatp;
#else
typedef float floatp;
#endif

/* If we are debugging, make all static variables and procedures public */
/* so they get passed through the linker. */
#ifdef NOPRIVATE
# define private /* */
#else
# define private static
#endif

/* Macros for argument templates.  ANSI has these, as does Turbo C, */
/* but most Unix compilers don't. */

#ifdef STDC_OR_MSDOS
# define P0() void
# define P1(t1) t1
# define P2(t1,t2) t1,t2
# define P3(t1,t2,t3) t1,t2,t3
# define P4(t1,t2,t3,t4) t1,t2,t3,t4
# define P5(t1,t2,t3,t4,t5) t1,t2,t3,t4,t5
# define P6(t1,t2,t3,t4,t5,t6) t1,t2,t3,t4,t5,t6
# define P7(t1,t2,t3,t4,t5,t6,t7) t1,t2,t3,t4,t5,t6,t7
# define P8(t1,t2,t3,t4,t5,t6,t7,t8) t1,t2,t3,t4,t5,t6,t7,t8
# define P9(t1,t2,t3,t4,t5,t6,t7,t8,t9) t1,t2,t3,t4,t5,t6,t7,t8,t9
# define P10(t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)\
   t1,t2,t3,t4,t5,t6,t7,t8,t9,t10
# define P11(t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)\
   t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11
# define P12(t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)\
   t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12
#else
# define P0() /* */
# define P1(t1) /* */
# define P2(t1,t2) /* */
# define P3(t1,t2,t3) /* */
# define P4(t1,t2,t3,t4) /* */
# define P5(t1,t2,t3,t4,t5) /* */
# define P6(t1,t2,t3,t4,t5,t6) /* */
# define P7(t1,t2,t3,t4,t5,t6,t7) /* */
# define P8(t1,t2,t3,t4,t5,t6,t7,t8) /* */
# define P9(t1,t2,t3,t4,t5,t6,t7,t8,t9) /* */
# define P10(t1,t2,t3,t4,t5,t6,t7,t8,t9,t10) /* */
# define P11(t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11) /* */
# define P12(t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12) /* */
#endif

/* Types for client-supplied allocate and free procedures. */
/* For accountability, debugging, and error messages, */
/* we pass an identifying string to alloc and free. */
/* Note that the arguments are like calloc, not like malloc, */
/* but an alloc procedure doesn't clear the block. */
typedef char *(*proc_alloc_t)(P3(unsigned num_elements, unsigned element_size, char *client_name));
typedef void (*proc_free_t)(P4(char *data, unsigned num_elements, unsigned element_size, char *client_name));

/* Standard error printing macros */
/* Use eprintf for ordinary error messages, */
/* dprintf for messages that should include debugging info. */

#ifdef PROGRAM_NAME
#  define _epn fprintf(stderr, PROGRAM_NAME),
#else
#  define _epn /* */
#endif

#define eprintf(str)\
  (_epn fprintf(stderr, str))
#define eprintf1(str,arg1)\
  (_epn fprintf(stderr, str, arg1))
#define eprintf2(str,arg1,arg2)\
  (_epn fprintf(stderr, str, arg1, arg2))
#define eprintf3(str,arg1,arg2,arg3)\
  (_epn fprintf(stderr, str, arg1, arg2, arg3))

#define _epfl /* */
#ifdef __FILE__
#  ifdef __LINE__
#    undef _epfl
#    define _epfl fprintf(stderr, "%s(%d): ", __FILE__, __LINE__),
#  endif
#endif

#define _epd _epn _epfl

#define dprintf(str)\
  (_epd fprintf(stderr, str))
#define dprintf1(str,arg1)\
  (_epd fprintf(stderr, str, arg1))
#define dprintf2(str,arg1,arg2)\
  (_epd fprintf(stderr, str, arg1, arg2))
#define dprintf3(str,arg1,arg2,arg3)\
  (_epd fprintf(stderr, str, arg1, arg2, arg3))
