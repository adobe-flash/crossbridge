/* APPLE LOCAL file AltiVec */
/* Test for distinguishing 'vector bool ...' from 'vector unsigned ...'
   types by the front-end.  */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

struct Foo1 {
    void foo(vector unsigned char) { }
    void foo(vector bool     char) { }   /* { dg-bogus "has already been declared" } */
    void bar(vector unsigned short) { }
    void bar(vector bool     short) { }   /* { dg-bogus "has already been declared" } */
    void baz(vector unsigned int) { }
    void baz(vector bool     int) { }   /* { dg-bogus "has already been declared" } */
};

