/* d8-ret.cpp */
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

int doreturn(Classfile *c) /* push return op, popping operand e.g. ireturn LO3 */
{
  unsigned pcval = currpc - 1;
  if (ch == 0xB1) {
    if (bufflength > 0) {
      Exp *e1 = new Exp(pcval, "/* void */", VOID, IM);
      *donestkptr++ = new Exp(pcval, PREUNARY, VOID, RETURN, e1);
    }
  } else {
    Exp *e1 = *(stkptr-1);
    if ((e1->e->type == INT) && (miptr->ret_type == BOOLEAN)) {
      if ((e1->e == std_exps + 2)||(e1->e == std_exps + 3))
	e1->e += 13; /* convert 0 or 1 to false or true */
      else
	/* CMPEQ */;
    }
    --stkptr;
    *donestkptr++ = new Exp(pcval, min(pcval, e1->minpc), PREUNARY, VOID, RETURN, e1);
  }
  return 0;
}

