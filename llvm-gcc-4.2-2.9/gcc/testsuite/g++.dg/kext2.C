/* APPLE LOCAL file KEXT indirect-virtual-calls --sts */
/* Radar 3008388: Positive C++ test case. */
/* Origin: Matt Austern <austern@apple.com> */
/* { dg-do compile } */
/* { dg-options "-fapple-kext" } */

struct B1 { virtual ~B1(); virtual void f(); };	/* ok */
struct D1 : B1 { };		                /* ok */
struct X1 : D1 { virtual void f(); };           /* ok */

void X1::f() { D1::f(); }                       /* ok */


