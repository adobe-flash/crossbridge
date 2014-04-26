/* Test error handling of optimization pragmas. */
/* Radar 3124235 */
/* LLVM LOCAL disable pragma opt -- whole file */
/* { dg-do compile { xfail { *-*-* } } } */
#include <stdio.h>
