/* PR middle-end/30473 */
/* Make sure this doesn't ICE.  */
/* { dg-do compile } */
/* APPLE LOCAL default to Wformat-security 5764921 */
/* { dg-options "-O2 -Wno-format" } */

extern int sprintf (char *, const char *, ...);

void
foo (char *buf1, char *buf2)
{
  sprintf (buf1, "%s", "abcde");
  sprintf (buf2, "%s");
}
