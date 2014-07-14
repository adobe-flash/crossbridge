/* dc-misc.cpp */
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

int docheckcast(Classfile *c)
{
  unsigned pcval = currpc - 1;
  unsigned val = JDNEXT16U();
  Exp *e1 = *(stkptr-1);
  Exp *e2 = new Exp(pcval, c->cp(val)->chp, OBJECT, CP, val);
  *(stkptr-1) = new Exp(pcval, min(e1->minpc, pcval), PREUNARY, OBJECT, CAST, e1, e2);
  return 0;
}

int doinstanceof(Classfile *c)
{
  unsigned pcval = currpc - 1;
  unsigned val = JDNEXT16U();
  Exp *e1 = *(stkptr-1);
  Exp *e2 = new Exp(pcval, c->cp(val)->chp, OBJECT, CP, val);
  *(stkptr-1) = new Exp(pcval, e1->minpc, BINARY, BOOLEAN, INSTANCEOF, e1, e2);
  return 0;
}
