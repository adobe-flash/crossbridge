/* APPLE LOCAL file AltiVec */
/* Test for AltiVec type overloading.  */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */
    void foo(vector unsigned char) { }
    void foo(vector signed char) { }
    void foo(vector bool     char) { }
    void foo(vector unsigned short) { }
    void foo(vector signed short) { }
    void foo(vector bool short) { }
    void foo(vector unsigned int) { }
    void foo(vector signed int) { }
    void foo(vector bool int) { }
    void foo(vector float) { }
    void foo(vector pixel) { }
    void foo(int) { }
    void foo(unsigned int) { }
    void foo(float) { }
    void foo(bool) { }
