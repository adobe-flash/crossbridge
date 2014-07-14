/* da-field.cpp */
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
#include "field.h"

int doget(Classfile *c)
{
  unsigned pcval = currpc - 1;
  int val = JDNEXT16S();
  Ref *mr = (Ref*)c->cp[val]->p;
  NameAndType *nt = (NameAndType*)c->cp[mr->name_and_type]->p;
  char *tmpstr = c->cp[nt->name_index]->chp;
  Exp *e = new Exp(pcval, tmpstr, sig2type(c->cp[nt->signature_index]->chp), CP, val);
  if (ch == 0xB2) { /* getstatic */
    Exp *e1;
    tmpstr = c->cp(mr->class_index)->chp;
//    int tmpint = strlen(c->package_name);
    if (strcmp(tmpstr, c->this_class_name)) {
      Exp *e2 = new Exp(pcval, tmpstr, VOID, NO);
      e1 = new Exp(pcval, BINARY, e->e->type, DOT, e2, e);
      e->e->type = VOID;
    } else {
      e1 = e;
    }
    *stkptr++ = e1;
    return 0;
  } else {          /* getfield  */
    if (((*(stkptr-1))->e->et == IDENT) && !strcmp((*(stkptr-1))->e->id->name,"this")) {
      /* this.bar == bar */
      e->minpc = min(pcval, (*(stkptr-1))->minpc);
      *(stkptr-1) = e; return 0;
    } else {
      /* foo.bar */
      *(stkptr-1) = new Exp(pcval, min(pcval, (*(stkptr-1))->minpc), BINARY,
                            e->e->type, DOT, *(stkptr-1), e);
      e->e->type = VOID;
      return 0;
    }
  }
}

int doput(Classfile *c)
{
  unsigned pcval = currpc - 1;
  int val = JDNEXT16S();
  Ref *mr = (Ref*)c->cp[val]->p;
  NameAndType *nt = (NameAndType*)c->cp[mr->name_and_type]->p;
  char *tmpstr = c->cp[nt->name_index]->chp;
  Exp *e = new Exp(pcval, tmpstr, sig2type(c->cp[nt->signature_index]->chp), NO);
  if (ch == 0xB3) { /* putstatic */
    Exp *e1;
    tmpstr = c->cp(mr->class_index)->chp;
    if (strcmp(tmpstr, c->this_class_name)) {
      Exp *e2 = new Exp(pcval, tmpstr, VOID, NO);
      e1 = new Exp(pcval, BINARY, e->e->type, DOT, e2, e);
      e->e->type = VOID;
    } else {
      e1 = e;
    }
    Exp *e3 = *(--stkptr);
    if ((e3->e->type == INT) && (e1->e->type == BOOLEAN)) {
      if ((e3->e == std_exps + 2)||(e3->e == std_exps + 3))
	e3->e += 13;
      else
	/* CMPEQ */;
    }
    *donestkptr++ = new Exp(pcval, min(pcval, e3->minpc), BINARY,
                            e1->e->type, ASSIGN, e1, e3);
    return 0;
  } else {          /* putfield  */
    Exp *e3 = *(stkptr-2);
    if ((e3->e->et == IDENT) && !strcmp(e3->e->id->name,"this")) {
      /* this.bar == bar */
      e3 = *(--stkptr);
      if ((e3->e->type == INT) && (e->e->type == BOOLEAN)) {
	if ((e3->e == std_exps + 2)||(e3->e == std_exps + 3))
	  e3->e += 13;
	else
	  /* CMPEQ */;
      }
      stkptr--;
      *donestkptr++ = new Exp(pcval, min(min(pcval, (*stkptr)->minpc),
                                         (*stkptr)->minpc),
                              BINARY, e->e->type, ASSIGN, e, e3);
      return 0;
    } else {
      /* foo.bar */
      Exp *e1 = new Exp(pcval, min((*(stkptr-1))->minpc, pcval), BINARY,
                        e->e->type, DOT, e3, e);
      e->e->type = VOID;
      e3 = *(--stkptr);
      if ((e3->e->type == INT) && (e1->e->type == BOOLEAN)) {
	if ((e3->e == std_exps + 2)||(e3->e == std_exps + 3))
	  e3->e += 13;
	else
	  /* CMPEQ */;
      }
      *donestkptr++ = new Exp(pcval, min(e3->minpc, e1->minpc), BINARY,
                              e1->e->type, ASSIGN, e1, e3);
      return 0;
    }
  }
}
