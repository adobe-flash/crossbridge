/* Copyright (C) 2003 Free Software Foundation, Inc.  */

/* { dg-do compile } */
/* { dg-options -fno-dollars-in-identifiers } */
/* APPLE LOCAL CW asm blocks 6338079 */
/* { dg-options "-fno-dollars-in-identifiers -fno-asm-blocks" } */

/* Test that -fno-dollars-in-identifiers is honoured.
   Neil Booth, 17 May 2003.  */

int foobar$;	/* { dg-error "stray '\\$'" } */
