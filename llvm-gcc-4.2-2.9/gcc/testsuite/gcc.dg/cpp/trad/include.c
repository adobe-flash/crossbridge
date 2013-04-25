/* Copyright (c) 2002 Free Software Foundation Inc.  */

/* Test that macros are not expanded in the <> quotes of #inlcude.  */

/* { dg-do preprocess } */

#define __STDC__ 1	/* Stop complaints about non-ISO compilers.  */
/* APPLE LOCAL begin Radar 6112293 */
#define errno 1
#include <errno.h>		/* { dg-bogus "o such file or directory" } */
/* APPLE LOCAL end Radar 6112293 */
