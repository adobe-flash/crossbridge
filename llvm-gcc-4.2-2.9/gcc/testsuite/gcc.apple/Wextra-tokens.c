/* APPLE LOCAL file -Wextra-tokens */
/* Lifted from gcc.dg/cpp/extratokens2.c. */
/* Copyright (C) 2002 Free Software Foundation, Inc.  */

/* { dg-do preprocess } */
/* { dg-options "-fno-show-column -Wextra-tokens" } */

/* Tests that -Wextra-tokens correctly enables the checks
   that are disabled by default.  */

#if 1 
#if 0
#else foo	/* { dg-warning "extra tokens" "bad warning" } */
#endif /	/* { dg-warning "extra tokens" "bad warning" } */
#endif

# 36 "file.c" 3

/* ... but in a system header, it's acceptable.  */
#ifdef KERNEL
#endif KERNEL  /* { dg-bogus "extra tokens" "bad warning" } */
