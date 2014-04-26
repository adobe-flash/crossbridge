/* APPLE LOCAL begin radar 4571747 */
/* Test vcmpequw. and vcmpequw and other simimlar patterns are combined by cse. */ 
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-O -faltivec" } */
/* { dg-final { scan-assembler "vcmpequw." } } */
/* { dg-final { scan-assembler-not "vcmpequw\[^\.\]" } } */
/* { dg-final { scan-assembler "vcmpgtsw." } } */
/* { dg-final { scan-assembler-not "vcmpgtsw\[^\.\]" } } */
/* { dg-final { scan-assembler "vcmpgtuw." } } */
/* { dg-final { scan-assembler-not "vcmpgtuw\[^\.\]" } } */
/* { dg-final { scan-assembler "vcmpgtuh." } } */
/* { dg-final { scan-assembler-not "vcmpgtuh\[^\.\]" } } */
/* { dg-final { scan-assembler "vcmpgtsh." } } */
/* { dg-final { scan-assembler-not "vcmpgtsh\[^\.\]" } } */
/* { dg-final { scan-assembler "vcmpequh." } } */
/* { dg-final { scan-assembler-not "vcmpequh\[^\.\]" } } */
/* { dg-final { scan-assembler "vcmpequb." } } */
/* { dg-final { scan-assembler-not "vcmpequb\[^\.\]" } } */
/* { dg-final { scan-assembler "vcmpgtsb." } } */
/* { dg-final { scan-assembler-not "vcmpgtsb\[^\.]" } } */
/* { dg-final { scan-assembler "vcmpgtub." } } */
/* { dg-final { scan-assembler-not "vcmpgtub\[^\.\]" } } */

vector unsigned int test_vec_cmpequw(vector unsigned int a, vector unsigned int b) {
  vector unsigned int mask;
  if (vec_all_eq(a, b))	return a;
  mask = (vector unsigned int) vec_cmpeq(a, b);
  return mask;
}

vector int test_vec_cmpgtsw(vector int a, vector int b) {
  vector int mask;
  if (vec_all_gt(a, b))	return a;
  mask = (vector int) vec_cmpgt(a, b);
  return mask;
}

vector unsigned int test_vec_cmpgtuw(vector unsigned int a, vector unsigned int b) {
  vector unsigned int mask;
  if (vec_all_gt(a, b))	return a;
  mask = (vector unsigned int) vec_cmpgt(a, b);
  return mask;
}

vector unsigned short test_vec_cmpgtuh(vector unsigned short a, vector unsigned short b) {
  vector unsigned short mask;
  if (vec_all_ge(a, b))	return a;
  mask = (vector unsigned short) vec_cmplt(a, b);
  return mask;
}

vector short test_vec_cmpgtsh(vector short a, vector short b) {
  vector short mask;
  if (vec_all_ge(a, b))	return a;
  mask = (vector short) vec_cmplt(a, b);
  return mask;
}

vector unsigned short test_vec_cmpequh(vector unsigned short a, vector unsigned short b) {
  vector unsigned short mask;
  if (vec_all_eq(a, b))	return a;
  mask = (vector unsigned short) vec_cmpeq(a, b);
  return mask;
}

vector unsigned char test_vec_cmpequb(vector unsigned char a, vector unsigned char b) {
  vector unsigned char mask;
  if (vec_all_eq(a, b))	return a;
  mask = (vector unsigned char) vec_cmpeq(a, b);
  return mask;
}

vector char test_vec_cmpgtsb(vector char a, vector char b) {
  vector char mask;
  if (vec_all_ge(a, b))	return a;
  mask = (vector char) vec_cmplt(a, b);
  return mask;
}

vector unsigned char test_vec_cmpgtub(vector unsigned char a, vector unsigned char b) {
  vector unsigned char mask;
  if (vec_all_ge(a, b))	return a;
  mask = (vector unsigned char) vec_cmplt(a, b);
  return mask;
}
/* APPLE LOCAL end radar 4571747 */
