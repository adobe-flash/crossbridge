/* APPLE LOCAL file mainline 4.3 2006-10-31 4134307 */
/* Check that an error message is produced when a C99 inline function
   is never defined.  */
/* { dg-do compile } */
/* { dg-options "-std=c99" } */

extern inline int func1 (void); /* { dg-error "never defined" } */
inline int func2 (void); /* { dg-error "never defined" } */
