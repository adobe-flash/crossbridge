/* APPLE LOCAL file headermaps */
/* Copyright (C) 2005 Free Software Foundation, Inc.  */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* This test is ppc specific, as the headermap binary was generated for ppc.  */
/* { dg-options "-I $srcdir/gcc.dg/cpp/headermap.hmap" } */

#define COUNT 1
#include <A.h>
#include <bA.h>
#include <c.h>
#import <d.h>
#import <d.h>
#import <A.h>

#if COUNT != 5
  #error COUNT not 5 in headermap-1
#endif
