/* APPLE LOCAL file KEXT indirect-virtual-calls --sts */
/* Radar 3008388: Negative C++ test case. */
/* Origin: Matt Austern <austern@apple.com> */
/* { dg-do compile } */
/* { dg-options "-fapple-kext" } */

struct B1 { virtual ~B1(); virtual void f(); };	/* ok */
struct D1 : B1 { };		                /* ok */
struct D2 { };                                  /* ok */

struct X1 : D1, D2 {  };                        /* ok */
struct X2 : virtual D1 {  };                    /* ok */

struct Y1 : X1 { virtual void f(); };           /* ok */
struct Y2 : X2 { virtual void f(); };           /* ok */

void Y1::f() { X1::f(); }               /* { dg-error "indirect virtual" } */
void Y2::f() { X2::f(); }               /* { dg-error "indirect virtual" } */
