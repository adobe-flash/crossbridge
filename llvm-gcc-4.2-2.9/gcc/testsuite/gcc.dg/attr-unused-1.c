/* APPLE LOCAL file for-fsf-4_4 3274130 5295549 */
/* { dg-do compile } */
/* { dg-options "-Wunused" } */

int foo(int * p)
{
  int i, c;
  
  for __attribute__((unused))
      (i = c = 0; i < 100; i++)
    c += p[i]; /* { dg-warning "ignored" } */
  return c;
}
