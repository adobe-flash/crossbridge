/* { dg-do compile } */
/* APPLE LOCAL begin radar 4875115 */
/* { dg-options "-Oz" } */
/* APPLE LOCAL end radar 4875115 */
/* { dg-final { scan-assembler-not "test" } } */

int fct1 (void);
int fct2 (void);

int
fct (unsigned nb)
{
  if ((nb >> 5) != 0)
    return fct1 ();
  else
    return fct2 ();
}
