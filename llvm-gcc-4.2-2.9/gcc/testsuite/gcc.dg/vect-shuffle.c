/* APPLE LOCAL file radar 4534261 */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-O2 -faltivec" } */

typedef vector float vFloat;

static vFloat _v_shuffle_two (vFloat fp0, vFloat fp1, const int i0)
{
  float fu = fu;
  float f0 = i0 < 4 ? ((float *) &fp0)[i0] : (i0 < 8 ? ((float *) &fp1)[i0 - 4] : fu);
  return (vFloat) {f0, f0, f0, f0};
}

void foo()
{
  vFloat *argA;
  vFloat *res;
  vFloat vfpconst;
  unsigned int sw, sc;
  res[2] = _v_shuffle_two (_v_shuffle_two (argA[2], -argA[2], sw), vfpconst, sc);
  res[1] = _v_shuffle_two (_v_shuffle_two (argA[1], -argA[1], sw), vfpconst, sc);
}
/* APPLE LOCAL file radar 4534261 */
