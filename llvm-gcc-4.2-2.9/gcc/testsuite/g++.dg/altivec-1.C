/* APPLE LOCAL file AltiVec */
/* Test for static_cast<...> among AltiVec types.  */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

struct Foo2 {
    vector unsigned int	vui;
    vector signed int As_vsi() { 
      return static_cast<vector signed int>(vui);  /* { dg-bogus "invalid static_cast" } */
    }
};

