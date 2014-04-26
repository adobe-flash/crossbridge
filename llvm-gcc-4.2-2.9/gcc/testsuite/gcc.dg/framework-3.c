/* APPLE LOCAL file iframework for 4.3 4094959 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-iframework$srcdir/gcc.dg" } */

#include <Foundation/Foundation.h> /* { dg-error "error: Foundation/Foundation.h: No such file" } */
