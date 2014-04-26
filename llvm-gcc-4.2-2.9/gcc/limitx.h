/* This administrivia gets added to the beginning of limits.h
   if the system has its own version of limits.h.  */

/* APPLE LOCAL begin 4401222 */
#ifndef _LIBC_LIMITS_H_
/* Use "..." so that we find syslimits.h only in this same directory.  */
#include "syslimits.h"
#endif
#ifdef _GCC_NEXT_LIMITS_H
/* LLVM LOCAL begin Fix for Linux builds */
#ifdef __linux__
#define _GCC_LIMITS_H_
#endif
/* LLVM LOCAL end Fix for Linux builds */
#include_next <limits.h>
#undef _GCC_NEXT_LIMITS_H
#endif
/* APPLE LOCAL end 4401222 */
