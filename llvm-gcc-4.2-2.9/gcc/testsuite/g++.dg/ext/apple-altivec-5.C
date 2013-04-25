/* APPLE LOCAL file AltiVec */
/* Test for handling of reference vector parameters.  */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec" } */

struct Vect
{
  typedef vector unsigned int vector_1;

  static vector_1 ConvertPixel1(const vector_1 &source, const vector unsigned char &vp);
  static vector_1 ConvertPixel2(const vector_1 source, const vector unsigned char vp);
  static vector_1 ConvertPixel3(vector_1 &source, vector unsigned char &vp);
  static vector_1 ConvertPixel4(vector_1 source, vector unsigned char vp);
};

Vect::vector_1 Vect::ConvertPixel1(const vector_1 &source, const vector unsigned char &vp)
{
  return (vector_1) vec_perm(source, source, vp);
}
Vect::vector_1 Vect::ConvertPixel2(const vector_1 source, const vector unsigned char vp)
{
  return (vector_1) vec_perm(source, source, vp);
}
Vect::vector_1 Vect::ConvertPixel3(vector_1 &source, vector unsigned char &vp)
{
  return (vector_1) vec_perm(source, source, vp);
}
Vect::vector_1 Vect::ConvertPixel4(vector_1 source, vector unsigned char vp)
{
  return (vector_1) vec_perm(source, source, vp);
}
