/* d5-stack.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include "exp.h"
#include "class.h"
#include "decomp.h"
#include "cp.h"

int dopop(Classfile *c)
{
  if (stkptr != stack) *donestkptr++ = *(--stkptr);
  return 0;
}


int dodup(Classfile *c)
{
  if ((*(stkptr-1))->e->op != NEW) {
    *stkptr = *(stkptr-1);
    (*stkptr)->numrefs++;
    stkptr++;
  }
  return 0;
}

int dodup_x1(Classfile *c)
{
  *stkptr = *(stkptr-1);
  *(stkptr-1) = *(stkptr-2);
  *(stkptr-2) = *stkptr;
  (*stkptr)->numrefs++;
  stkptr++;
  return 0;
}
