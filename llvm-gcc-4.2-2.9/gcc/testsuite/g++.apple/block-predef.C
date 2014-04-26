/* APPLE LOCAL file radar 5868913 */
/* Test that __BLOCKS__ is undefined in g++ */
/* { dg-do compile } */

#ifndef __OBJC__
#if __BLOCKS__
#error TEST FAILS
#endif
#endif
