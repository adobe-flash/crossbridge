/* general.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* These allow us to set debugging mode on at
   compile-time or make it an option at run-time. */
//#define debug
#define debug_optional

/* The D macro */
#if defined(debug)
#  define D(x) x
#elif defined(debug_optional)
extern int debugon;
#  define D(x) if(debugon){ x; } else {}
#else
#  define D(x)
#endif /* debug */

/* The min macro */
#define min(a,b) (((a) < (b)) ? (a) : (b))

/* Miscellaneous typedefs for specific-sized quantities.
   These should be in a machine-dependant file. */
typedef unsigned  char  u8;
typedef   signed  char  i8;
typedef unsigned short u16;
typedef   signed short i16;
typedef unsigned  long u32;
typedef   signed  long i32;

typedef char *char_ptr;

#endif
