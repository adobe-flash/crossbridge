/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec" } */

static vector int x, y;

static vector signed int i,j;
static vector signed short s,t;
static vector signed char c,d;
static vector float f,g;
static vector unsigned short us;

static vector unsigned char uc;

static vector signed int *pi;

static int int1, int2;

void
b()
{
  vec_add (x, y);

  /* Make sure the predicates accept correct argument types.  */

  int1 = vec_all_in (f, g);
  int1 = vec_all_ne (f, g);
  int1 = vec_all_ne (c, d);
  int1 = vec_all_ne (s, t);
  int1 = vec_all_ne (i, j);
  int1 = vec_all_nge (f, g);
  int1 = vec_all_ngt (f, g);
  int1 = vec_all_ge (c, d);
  int1 = vec_all_ge (s, t);
  int1 = vec_all_ge (i, j);
  int1 = vec_all_ge (c, d);
  int1 = vec_all_ge (s, t);
  int1 = vec_all_ge (i, j);

  vec_mtvscr (i);
  vec_dssall ();
  us = vec_mfvscr ();
  vec_dss (3);

  vec_dst (pi, int1 + int2, 3);
  vec_dstst (pi, int1 + int2, 3);
  vec_dststt (pi, int1 + int2, 3);
  vec_dstt (pi, int1 + int2, 3);

  uc = vec_lvsl (int1 + 69, pi);
  uc = vec_lvsr (int1 + 69, pi);

  c = vec_lde (int1, (vector bool char *)pi);
  s = vec_lde (int1, (vector pixel *)pi);
  i = vec_lde (int1, pi);
  i = vec_ldl (int1, pi);
  i = vec_ld (int1, pi);

  vec_st (i, int2, pi);
  vec_ste (c, int2, (vector unsigned char *)pi);
  vec_ste (s, int2, (vector bool short *)pi);
  vec_ste (i, int2, pi);
  vec_stl (i, int2, pi);
}
