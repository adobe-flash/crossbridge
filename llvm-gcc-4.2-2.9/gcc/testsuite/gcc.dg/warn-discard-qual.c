/* APPLE LOCAL file -Wdiscard-qual 4086969 */
/* Test -Wno-discard-qual */
/* { dg-do compile } */
/* { dg-options "-Wno-discard-qual" } */

const char *a( void )
{
  return "abc";
}

int main( void )
{
  char *s = a();
  return 0;
}

