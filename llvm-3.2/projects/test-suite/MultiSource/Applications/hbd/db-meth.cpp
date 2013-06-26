/* db-meth.cpp */
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

int invokefunc(Classfile *c)
{
  unsigned pcval = currpc - 1;
  unsigned minpcval = pcval;
  int i;
  Type exptypes[256];
  int val = JDNEXT16S();
  Ref *mr = (Ref*)c->cp[val]->p;
  NameAndType *nt = (NameAndType*)c->cp[mr->name_and_type]->p;
  char *classname = c->cp(mr->class_index)->chp;
  char *tmpstr = c->cp[nt->signature_index]->chp;
  char *name = c->cp[nt->name_index]->chp;
  Exp **el = new Exp*[strlen(tmpstr)-2];
  Exp *e1 = new Exp(pcval, name, VOID, NO);
  unsigned numexps = 0;
  while (*(++tmpstr) != ')') {
    exptypes[numexps++] = sig2type(tmpstr);
    if (*tmpstr == '[') tmpstr++;
    if (*tmpstr == 'L') while (*(++tmpstr) != ';') /* do nothing */;
  }
  Type etype = sig2type(tmpstr + 1);
  Exp **elp = el;
  for (i = numexps; i--;) {
    if (((*(--stkptr))->e->type == INT) && (exptypes[i] == BOOLEAN)) {
      if (((*stkptr)->e == std_exps + 2)||((*stkptr)->e == std_exps + 3))
	(*stkptr)->e += 13;
      else
	/* CMPEQ */;
    }
    *elp++ = *stkptr;
    minpcval = min(minpcval, (*stkptr)->minpc);
  }
  if (ch == 0xB9) { /* invokeinterface */
    if (numexps != (unsigned)(JDNEXT8U() - 1)) {
      fprintf(stderr,"Error in interface method invocation - nargs doesn't match.\n");
      return 1;
    }
    JDNEXT8U(); /* reserved byte */
  }
  if (ch != 0xB8) { /* invokevirtual OR invokenonvirtual OR invokeinterface */
    if (((*(stkptr-1))->e->et == IDENT) && !strcmp((*(stkptr-1))->e->id->name,"this")) {
      /* this.bar(...) == bar(...) */
      if (strcmp(classname,c->this_class_name)) { /* super.bar(...) */
	if (!strcmp(name,"<init>")) { /* super() */
	  e1->e->id->name = "super";
	  minpcval = min(minpcval, (*(stkptr-1))->minpc);
          Exp *e = new Exp(pcval, minpcval, FUNCTIONCALL, etype, ID, e1, numexps, el);
	  if (etype == VOID) {
            --stkptr; *donestkptr++ = e;
	  } else *(stkptr-1) = e;
	  return 0;
	}
	minpcval = min(minpcval, (*(stkptr-1))->minpc);
	Exp *e2 = new Exp(pcval, "super", VOID, NO);
	Exp *e3 = new Exp(pcval, minpcval, BINARY, FUNC, DOT, e2, e1);
        Exp *e = new Exp(pcval, minpcval, FUNCTIONCALL, etype, ID, e3, numexps, el);
	if (etype == VOID) {
          --stkptr; *donestkptr++ = e;
	} else *(stkptr-1) = e;
	return 0;
      } else {
	if (!strcmp(name,"<init>")) {
          minpcval = min(minpcval, (*(stkptr-1))->minpc);
          e1->e->id->name = "this";
        }
        Exp *e = new Exp(pcval, minpcval, FUNCTIONCALL, etype, ID, e1, numexps, el);
        if (e->e->type == VOID) {
          --stkptr; *donestkptr++ = e;
        } else *(stkptr-1) = e;
        return 0;
      }
    } else {
      /* foo.bar(...) */
      Exp *e;
      if (!strcmp(name,"<init>")) {
        /* killexp(e1)? */
        minpcval = min(minpcval, (*(stkptr-1))->minpc);
        e = new Exp(pcval, minpcval, FUNCTIONCALL, OBJECT, ID, *(stkptr-1), numexps, el);
      } else {
        minpcval = min(minpcval, (*(stkptr-1))->minpc);
        Exp *e3 = new Exp(pcval, minpcval, BINARY, FUNC, DOT, *(stkptr-1), e1);
        e = new Exp(pcval, minpcval, FUNCTIONCALL, etype, ID, e3, numexps, el);
      }
      if ((e->exp1->e->op != NEW) && (etype == VOID)) {
        --stkptr; *donestkptr++ = e;
      } else *(stkptr-1) = e;
      return 0;
    }
  } else { /* invokestatic */
    Exp *e;
    tmpstr = c->cp(mr->class_index)->chp;
    if (strcmp(tmpstr, c->this_class_name)) {
      Exp *e2 = new Exp(pcval, tmpstr, VOID, NO);
      Exp *e3 = new Exp(pcval, minpcval, BINARY, FUNC, DOT, e2, e1);
      e = new Exp(pcval, minpcval, FUNCTIONCALL, etype, ID, e3, numexps, el);
    } else
      e = new Exp(pcval, minpcval, FUNCTIONCALL, etype, ID, e1, numexps, el);
    if (etype == VOID)
      *donestkptr++ = e;
    else
      *stkptr++ = e;
    return 0;
  }
}
