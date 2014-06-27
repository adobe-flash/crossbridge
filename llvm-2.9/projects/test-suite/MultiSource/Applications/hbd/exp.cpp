/* exp.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exp.h"

Exp_ std_exps[] = {
  Exp_(&idnull), Exp_(&idneg1), Exp_(&id0i), Exp_(&id1i), Exp_(&id2i), Exp_(&id3i),
  Exp_(&id4i), Exp_(&id5i), Exp_(&id0L), Exp_(&id1L), Exp_(&id0f), Exp_(&id1f),
  Exp_(&id2f), Exp_(&id0d), Exp_(&id1d), Exp_(&idfalse), Exp_(&idtrue),
  Exp_(1, BRANCH, VOID, ID, 0), Exp_(1, TERNARY, BOOLEAN, COND, 0)
};

void killexp(Exp *e) {
  if (!(--e->numrefs)) {
    if (!e->e->isstd) {
      if (e->e->et == IDENT) {
//      delete e->e->id->name;
//        delete e->e->id;
      }
//      delete e->e;
    }
//    delete e;
  }
}

int notexp(Exp **e_ptr) {
  Exp *e = *e_ptr;
  switch (e->e->op) {
    case NOT_BOOL:
      *e_ptr = e->exp1;
      killexp(e);
      break;
    case OR_BOOL:
      e->e->op = AND_BOOL;
      notexp(&(e->exp1));
      notexp(&(e->exp2));
      break;
    case AND_BOOL:
      e->e->op = OR_BOOL;
      notexp(&(e->exp1));
      notexp(&(e->exp2));
      break;
    case OR: case AND:
      notexp(&(e->exp1));
      notexp(&(e->exp2));
    case EQUAL: case NOTEQUAL: case LESS: case GREATEROREQUAL:
    case GREATER: case LESSOREQUAL:
      *((int*)(&e->e->op)) ^= 1;
      break;
    default:
      if (e->e->type != BOOLEAN) {
        fprintf(stderr, "Can't not a non-boolean\n");
        return 1;
      }
      *e_ptr = new Exp(e->pc, e->minpc, PREUNARY, BOOLEAN, NOT_BOOL, e);
  }
  return 0;
}

/*
Exp::Exp(int pcval, char *idname, Type idtype) {
  numrefs = 1;
  minpc = pc = pcval;
  e = new Exp_;
  e->isstd = 0;
  e->et = IDENT;
  e->op = ID;
  e->type = idtype;
  e->id = new Id;
  e->id->name = idname;
}
*/

char *Exp::toString(unsigned nextpc) {
  char *e1, *e2, *e3, *o, *o2, *s, *t1;
  int sizestr, i;
  switch (e->et) {
  case IDENT:
    s = new char[strlen(e->id->name) + 1];
    strcpy(s, e->id->name);
    return s;
  case PREUNARY:
    exp1->numrefs += numrefs-1;
    e1 = exp1->toString(0);
    if (e->op == CAST) {
      if (e->type == OBJECT) {
        exp2->numrefs += numrefs-1;
        e2 = exp2->toString(0);
        killexp(exp2);
        o = new char[strlen(e2) + 3];
        sprintf(o, "(%s)", e2);
        delete [] e2;
      } else {
        o = new char[strlen(type2str[e->type]) + 3];
        sprintf(o, "(%s)", type2str[e->type]);
      }
    } else {
      o = strdup(op2str[e->op]);
    }
    s = new char[5 + strlen(o) + strlen(e1)];
    if (op_prec[exp1->e->op] < (op_prec[e->op] + 0))
      sprintf(s, "%s(%s)", o, e1);
    else
      sprintf(s, "%s%s", o, e1);
    killexp(exp1); delete [] e1; free(o);
    return s;
  case POSTUNARY:
    exp1->numrefs += numrefs-1;
    e1 = exp1->toString(0); o = op2str[e->op];
    s = new char[5 + strlen(o) + strlen(e1)];
    sprintf(s, (op_prec[exp1->e->op] < (op_prec[e->op] + 0))?"(%s)%s":"%s%s", e1, o);
    killexp(exp1); delete [] e1;
    return s;
  case BINARY:
    exp1->numrefs += numrefs-1;
    exp2->numrefs += numrefs-1;
    e1 = exp1->toString(0); e2 = exp2->toString(0);
    o = op2str[e->op];
    t1 = new char[9 + strlen(o)];
    sprintf(t1, "%s%s%s",
      (op_prec[exp1->e->op] < (op_prec[e->op] + 0))?"(%s)":"%s",
      o, (op_prec[exp2->e->op] < (op_prec[e->op] + 0))?"(%s)":"%s");
    s = new char[strlen(t1) + strlen(e1) + strlen(e2) - 3];
    sprintf(s, t1, e1, e2); delete [] t1;
    killexp(exp1); killexp(exp2); delete [] e1; delete [] e2;
    return s;
  case TERNARY:
    exp1->numrefs += numrefs-1; exp2->numrefs += numrefs-1; exp3->numrefs += numrefs-1;
    e1 = exp1->toString(0); e2 = exp2->toString(0); e3 = exp3->toString(0);
    o = op2str[e->op]; o2 = op2str[e->op + 1];
    t1 = new char[19];
    sprintf(t1, "%s%s%s%s%s",
      (op_prec[exp1->e->op] < (op_prec[e->op] + 0))?"(%s)":"%s",
      o, (op_prec[exp2->e->op] < (op_prec[e->op] + 0))?"(%s)":"%s",
      o2, (op_prec[exp3->e->op] < (op_prec[e->op] + 0))?"(%s)":"%s");
    s = new char[strlen(t1) + strlen(e1) + strlen(e2) + strlen(e3) - 5];
    sprintf(s, t1, e1, e2, e3); delete [] t1;
    killexp(exp1); killexp(exp2); killexp(exp3);
    delete [] e1; delete [] e2; delete [] e3;
    return s;
  case FUNCTIONCALL:
    t1 = new char[256];
    exp1->numrefs += numrefs-1;
    e1 = exp1->toString(0); sizestr = strlen(e1) + 3;
    sprintf(t1, "%s(", e1);
    killexp(exp1); delete [] e1;
    i = numexps;
    if (i) {
      while (--i) {
        explist[i]->numrefs += numrefs-1;
        e1 = explist[i]->toString(0); strcat(t1, e1); sizestr += strlen(e1) + 2;
        killexp(explist[i]); delete [] e1; strcat(t1, ", ");
      }
      explist[0]->numrefs += numrefs-1;
      e1 = explist[0]->toString(0); strcat(t1, e1); sizestr += strlen(e1);
      killexp(explist[0]); delete [] e1;
    }
    strcat(t1,")");
    s = new char[sizestr];
    strcpy(s, t1);
    delete [] t1;
    return s;
  case ARRAYACCESS:
    exp1->numrefs += numrefs-1; exp2->numrefs += numrefs-1;
    e1 = exp1->toString(0); e2 = exp2->toString(0);
    s = new char[strlen(e1) + strlen(e2) + 3];
    sprintf(s, "%s[%s]", e1, e2);
    killexp(exp1); killexp(exp2); delete [] e1; delete [] e2;
    return s;
  case BRANCH:
//    if ((unsigned)e->op > minpc) {
//    {
//      intlist *i = indents_end, *j;
//      if ((!i) || (i->node >= branch_pc)) {
//        indents_end = new intlist;
//        indents_end->node = branch_pc;
//        indents_end->next = i;
//      } else {
//        while ((i->next) && (i->next->node < branch_pc)) i = i->next;
//        j = i->next; i = i->next = new intlist; i->next = j;
//        i->node = branch_pc;
//      }
//      indentlevel++;
      notexp(&exp1);
      exp1->numrefs += numrefs-1;
      e1 = exp1->toString(0);
      s = new char[strlen(e1) + 8];
      sprintf(s, "if (%s) {", e1);
//    } else {
//      exp1->numrefs += numrefs-1;
//      e1 = exp1->toString(0);
//      s = new char[strlen(e1) + 21];
//      sprintf(s, "if (%s) goto label%d", e1, branch_pc);
//    }
    killexp(exp1); delete [] e1;
    return s;
  case SWITCH:
    exp1->numrefs += numrefs-1;
    e1 = exp1->toString(0);
    s = new char[strlen(e1) + 29];
    sprintf(s, "switch (%s) default: label%d", e1, default_pc);
    killexp(exp1); delete [] e1;
    return s;
  default:
    fprintf(stderr, "Error converting expressions to strings. %d\n", e->et);
    exit(-1);
    return 0;
  }
}
