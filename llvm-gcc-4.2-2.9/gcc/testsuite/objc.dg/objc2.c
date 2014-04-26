/* APPLE LOCAL file 5726269 */
/* Verify that __OBJC2__ is not defined on arm when compiling a C file. */
/* { dg-do compile { target arm*-*-darwin* } } */

#ifdef __OBJC2__
error __OBJC2__ 
#else
int i = 3;
#endif
