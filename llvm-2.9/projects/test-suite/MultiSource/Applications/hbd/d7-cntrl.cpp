/* d7-cntrl.cpp */
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

int cond_pcend;
Exp *cond_e;
Exp *cond_e2;
Exp **cond_donestkptr;
Exp **cond_stkptr;

int doif1(Classfile *c)
{
  unsigned pcval = currpc - 1;
  Exp *e = *(--stkptr), *e1, *e2;
  switch (e->e->type) {
    case CMPTYPE:
      if (e->e->op != CMP) { fprintf(stderr, "doif1 error\n"); return 1; }
      e->e->op = (Op)(EQUAL + ch - 0x99); e->e->type = BOOLEAN;
      break;
    case BOOLEAN:
      if (ch == 0x99) // ifeq (ie if false)
        if (notexp(&e)) { fprintf(stderr, "doif1 error\n"); return 1; }
      break;
    case INT:
      e1 = *stkptr; e2 = new Exp(pcval, I0EXP);
      e = new Exp(pcval, e1->minpc, BINARY, BOOLEAN, (Op)(EQUAL + ch - 0x99), e1, e2);
      break;
    case OBJECT:
      e1 = *stkptr; e2 = new Exp(pcval, NULLEXP);
      e = new Exp(pcval, e1->minpc, BINARY, BOOLEAN, (Op)(EQUAL + ch - 0xC6), e1, e2);
      break;
    default:
      break;
  }
  *donestkptr++ = new Exp(pcval, e->minpc, IFEXP, e, pcval + JDNEXT16S());
  return 0;
}

int doif2(Classfile *c)
{
  unsigned pcval = currpc - 1;
  Exp *e2 = *(--stkptr);
  Exp *e1 = *(--stkptr);
  Exp *e = new Exp(pcval, min(e1->minpc, e2->minpc), BINARY, BOOLEAN,
                   (Op)(EQUAL + ((ch - 0x9F) % 6)), e1, e2);
  *donestkptr++ = new Exp(pcval, e->minpc, /*std_exp*/IFEXP, e,
                          /*branch_pc*/pcval + JDNEXT16S());
  return 0;
}

int docmp(Classfile *c)
{
  unsigned pcval = currpc - 1;
  Exp *e2 = *(--stkptr);
  Exp *e1 = *(stkptr-1);
  *(stkptr-1) = new Exp(pcval, min(e1->minpc, e2->minpc), BINARY, CMPTYPE, CMP, e1, e2);
  return 0;
}

int finishconditional(Classfile *c) {
  if ((stkptr != cond_stkptr) || (donestkptr != cond_donestkptr))
    { fprintf(stderr, "Error cond\n"); return 1; }
  cond_e->e++;
  if ((cond_e->exp1->e->op > LESSOREQUAL) || (cond_e->exp1->e->op < EQUAL)) {
    if (cond_e->exp1->e->type != BOOLEAN) { fprintf(stderr, "Can't not a non-boolean\n"); return 1; }
    Exp *e1 = cond_e->exp1;
    cond_e->exp1 = new Exp(currpc, e1->minpc, PREUNARY, BOOLEAN, NOT_BOOL, e1);
  } else {
    *((int*)(&cond_e->exp1->e->op)) ^= 1;
  }
  cond_e->exp2 = cond_e2; cond_e->exp3 = *(stkptr-1);
  *(stkptr-1) = cond_e;
  cond_pcend = -1;
  return 0;
}
