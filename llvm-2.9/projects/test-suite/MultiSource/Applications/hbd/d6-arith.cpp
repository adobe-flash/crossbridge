/* d6-arith.cpp */
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

extern int cond_pcend;
extern Exp *cond_e;
extern Exp *cond_e2;
extern Exp **cond_donestkptr;
extern Exp **cond_stkptr;

int pushbinop(Classfile *c) /* push binary operation, popping operands e.g. lxor */
{
  unsigned pcval = currpc - 1;
  Exp *e2 = *(--stkptr);
  Exp *e1 = *(stkptr-1);
  *(stkptr-1) = new Exp(pcval, min(min(e1->minpc, e2->minpc), pcval), BINARY,
                        e1->e->type,
                        (Op)((ch < 0x74)?
                          ((ch - 0x60) >> 2) : (0x07 + ((ch - 0x78) >> 1))),
                        e1, e2);
  return 0;
}

int pushunop(Classfile *c) /* push unary operation, popping operand e.g. lneg */
{
  unsigned pcval = currpc - 1, branch_pc;
  char *tmpstr, *buff;
  Exp *e1 = *(stkptr-1);
  Op eop = CAST;
  Type etype = VOID;
  int val;
  switch (ch) {
    case 0x74: case 0x75: case 0x76: case 0x77: eop = NEG; etype = e1->e->type; break;
    case 0x88: case 0x8B: case 0x8E: etype = INT; break;
    case 0x85: case 0x8C: case 0x8F: etype = LONG; break;
    case 0x86: case 0x89: case 0x90: etype = FLOAT; break;
    case 0x87: case 0x8A: case 0x8D: etype = DOUBLE; break;
    case 0x91: etype = BYTE; break;
    case 0x92: etype = CHAR; break;
    case 0x93: etype = SHORT; break;
    case 0xBF:
      --stkptr;
      *donestkptr++ = new Exp(pcval, min(e1->minpc, pcval), PREUNARY, VOID, THROW, e1);
      return 0;
    case 0xBB:
      val = JDNEXT16U();
      tmpstr = c->cp(val)->chp;
      buff = new char[strlen(tmpstr) + 1]; strcpy(buff, tmpstr);
      e1 = new Exp(pcval, buff, VOID, CP, val);
      *(stkptr++) = new Exp(pcval, min(e1->minpc, pcval), PREUNARY, OBJECT, NEW, e1);
//      if ((ch = JDNEXT8()) == 0x59)
//	return actiontable[actions[ch = JDNEXT8()]]();
//      else
//	return actiontable[actions[ch]]();
    case 0xBA: eop = NEW; break;
    case 0xA7: /* GOTO really shouldn't be here! */
      if (stkptr!=stack) {
//	if (stkptr!=(stack+1)) { fprintf(stderr, "Error in conditional operator!\n"); return 1; }
	if (cond_pcend != -1) { fprintf(stderr, "Can't handle recursive conditional operators!\n"); return 1; }
	cond_pcend = pcval + JDNEXT16S();
	cond_stkptr = stkptr;
	cond_e2 = *(--stkptr);
	--donestkptr;
	if ((*donestkptr)->e->et == BRANCH) {
	  if ((*donestkptr)->branch_pc != currpc) {
	    fprintf(stderr, "Error in conditional operator!\n"); return 1;
	  }
	  cond_e = *donestkptr;
	} else {
	  fprintf(stderr, "Use of comma operator in conditionals not yet supported.\n");
	  return 1;
	}
	cond_donestkptr = donestkptr;
	return 0;
      }
      branch_pc = pcval + JDNEXT16S();
      tmpstr = new char[100];
      sprintf(tmpstr,"label%i", branch_pc);
      buff = new char[strlen(tmpstr) + 1]; strcpy(buff, tmpstr);
      delete [] tmpstr;
      e1 = new Exp(pcval, buff, VOID/*label*/, IM);
      *donestkptr++ = new Exp(pcval, PREUNARY, VOID, GOTO, e1, branch_pc);
      return 0;
    default:
      fprintf(stderr, "Error in pushing unary operation\n");
      exit(-1);
  }
  *(stkptr-1) = new Exp(pcval, min(e1->minpc, pcval), PREUNARY, etype, eop, e1);
  return 0;
}
