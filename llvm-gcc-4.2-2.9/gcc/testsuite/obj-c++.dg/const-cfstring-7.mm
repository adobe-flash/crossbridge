/* APPLE LOCAL file 4493912 */
/* Test that CFString.length is (long) 64 bit with -m64. */
/* { dg-options "-mmacosx-version-min=10.5 -fconstant-cfstrings -m64" } */
/* { dg-do compile { target powerpc*-*-darwin* } } */

#include <Foundation/NSString.h>

NSString * string = @"anything here" ;
/* { dg-final { scan-assembler     "\\.quad\t13" } } */
