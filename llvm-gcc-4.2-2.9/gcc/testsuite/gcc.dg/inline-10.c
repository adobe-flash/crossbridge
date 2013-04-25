/* Test inline main, gnu99 mode, freestanding, -pedantic-errors.  */
/* Origin: Joseph Myers <jsm@polyomino.org.uk> */
/* { dg-do compile } */
/* { dg-options "-std=gnu99 -ffreestanding -pedantic-errors" } */

/* APPLE LOCAL begin mainline 4.3 2006-10-31 4134307 */
inline int main (void) { return 1; }
/* APPLE LOCAL end mainline 4.3 2006-10-31 4134307 */
