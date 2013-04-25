/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-maltivec -O2 -faltivec" } */

/* ICEs at -O1 or more */

extern void abort();

int main()
{

  const static vector float vINF = 
     ( vector float ) ( ( vector unsigned int ) ( 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000 ) );

  unsigned int af[4] = {
                       (unsigned int)0x7F800000,
                       (unsigned int)0x7F800000,
                       (unsigned int)0x7F800000,
                       (unsigned int)0x7F800000};

 union u {
          vector float vf;
          unsigned int   ai[4];
 }vu;

 int i;

 vu.vf = vINF;

 for (i=0; i < 4; i++)
    if (af[i] != vu.ai[i])
      abort();

 return 0;
}
