/* APPLE LOCAL file 4505813 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-options "-msse2" } */

#include <xmmintrin.h>

__m64 i1 = { 0x0123456789ABCDEFLL };
__m64 i2 = { 0x01234567, 0x89ABCDEF }; /* { dg-warning "excess elements" } */
__m64 i3 = { 0x89ABCDEF };
__m64 i4 = { 0x89ABCDEFLL };
__m64 i5 = { 0x01234567LL, 0x89ABCDEFLL }; /* { dg-warning "excess elements" } */
__m64 i6 = { 1, 2, 3 }; /* { dg-warning "excess elements" } */
__m64 i7 = { 1, 2, 3, 4 }; /* { dg-warning "excess elements" } */
__m64 i8 = { 1, 2, 3, 4, 5, 6, 7 }; /* { dg-warning "excess elements" } */
__m64 i9 = { 1, 2, 3, 4, 5, 6, 7, 8 }; /* { dg-warning "excess elements" } */
__m64 i10 = 0x0123456789ABCDEFLL; /* { dg-error "incompatible types" } */
__m64 i11 = 1; /* { dg-error "incompatible types" } */

/* { dg-warning "near initialization" "initialization" { target *-*-* } 8 } */
/* { dg-warning "near initialization" "initialization" { target *-*-* } 11 } */
/* { dg-warning "near initialization" "initialization" { target *-*-* } 12 } */
/* { dg-warning "near initialization" "initialization" { target *-*-* } 13 } */
/* { dg-warning "near initialization" "initialization" { target *-*-* } 14 } */
/* { dg-warning "near initialization" "initialization" { target *-*-* } 15 } */


