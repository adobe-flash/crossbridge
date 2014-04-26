/* APPLE LOCAL file 6187262 */
/* { dg-do compile { target { i?86-*-darwin* x86_64-*-darwin* } } } */
/* { dg-options { -O1 } } */
typedef long long __m64 __attribute__ ((__vector_size__ (8), __may_alias__));
int main()
{
  __m64 mm6;
  int i;
  for(i=0; i<1e7; ++i)
    {
      mm6 = (__m64)(unsigned long long)i;
      mm6 = __builtin_ia32_pand(mm6, mm6);
    }
}
