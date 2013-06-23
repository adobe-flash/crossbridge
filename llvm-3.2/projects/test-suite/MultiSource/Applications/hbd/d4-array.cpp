/* d4-array.cpp */
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

int anewarray(Classfile *c)
{
  unsigned pcval = currpc - 1;
  int val = JDNEXT16U();
  char *class_name = c->cp(val)->chp;
  Exp *e1 = new Exp(pcval, class_name, VOID, NO);
  Exp *e2 = new Exp(pcval, ARRAYACCESS, VOID, ID, e1, *(stkptr-1));
  *(stkptr-1) = new Exp(pcval, min(pcval, e2->exp2->minpc), PREUNARY, ARRAY, NEW, e2);
  return 0;
}

int multianewarray(Classfile *c)
{
  printf("Multi");
  unsigned pcval = currpc - 1;
  /*int val = */JDNEXT16U();
//  char *class_name; // = c->constant_pool[c->constant_pool[val].i].cp;
//  for (val = JDNEXT8U(); val--;) {
//    --stkptr;
//  } stkptr++;
  Exp *e1 = *(stkptr-1);
  *(stkptr-1) = new Exp(pcval, min(pcval, e1->minpc), PREUNARY, ARRAY, NEW, e1);
  return 0;
}

int doarraylength(Classfile *c) /* 190 == 0xBE */
{
  unsigned pcval = currpc - 1;
  Exp *e1 = *(stkptr-1);
  Exp *e2 = new Exp(pcval, "length", VOID, NO);
  *(stkptr-1) = new Exp(pcval, min(pcval, e1->minpc), BINARY, INT, DOT, e1, e2);
  return 0;
}

int doarrayget(Classfile *c)
{
  unsigned pcval = currpc - 1;
  Exp *e2 = *(--stkptr);
  Exp *e1 = *(stkptr-1);
  *(stkptr-1) = new Exp(pcval, min(min(pcval, e1->minpc), e2->minpc), ARRAYACCESS,
                        (Type)(ch - (0x2E - INT)), ID, e1, e2);
  return 0;
}

int doarrayput(Classfile *c)
{
  unsigned pcval = currpc - 1;
  Exp *e4 = *(--stkptr);
  Exp *e3 = *(--stkptr);
  Exp *e2 = *(--stkptr);
  unsigned minpcval = min(min(pcval, e2->minpc), e3->minpc);
  Exp *e1 = new Exp(pcval, minpcval, ARRAYACCESS, OBJECT, ID, e2, e3);
  *donestkptr++ = new Exp(pcval, minpcval, BINARY, OBJECT, ASSIGN, e1, e4);
  return 0;
}
