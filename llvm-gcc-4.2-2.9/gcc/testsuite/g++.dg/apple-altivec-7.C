/* APPLE LOCAL file */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

typedef __vector unsigned int vui; // 1
typedef __vector float vf; // 2

template <int i> vui do2 () // 3
{
  vui v; return v;
}

template <int j> vf do1 ()
{
  static_cast <vf> (do2 <j> ()); // 4
}

int main ()
{
  do1 <12> (); // 5
}
