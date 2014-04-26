/* { dg-options "-O2 -ftree-vectorize -fno-strict-aliasing" } */
/* { dg-compile } */

void foo()
{
}

/* { dg-warning "aliasing" "" { target *-*-* } 0 } */
