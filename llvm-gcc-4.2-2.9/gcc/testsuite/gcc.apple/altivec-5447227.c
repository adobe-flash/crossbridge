/* APPLE LOCAL file 5447227 */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec" } */

vector unsigned char foo1 ()
{
  return vec_lvsl (); /* { dg-error "error: too few arguments to function" } */
}

void foo2 ()
{
  vector signed char i;
  vec_stl (i); /* { dg-error "error: too few arguments to function" } */
}

vector float f;
int foo3 ()
{
  return  vec_all_in (f); /* { dg-error "error: too few arguments to function" } */
}

void foo4 ()
{
  vec_dst (); /* { dg-error "error: too few arguments to function|bailing out" } */
}
