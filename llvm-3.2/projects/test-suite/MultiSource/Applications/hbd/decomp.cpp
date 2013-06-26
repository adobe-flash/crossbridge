/* decomp.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "general.h"
#include "options.h"
#include "version.h"
#include "cp.h"
#include "access.h"
#include "field.h"
#include "exp.h"
#include "method.h"
#include "consts.h"
#include "class.h"
#include "file.h"
#include "sig.h"
#include "decomp.h"

void printintlist(intlist *t) {
  fprintf(stderr, "[");
  intnode *n = t->head;
  while (n) {
    fprintf(stderr, "%d", n->node);
    n = n->next;
    if (!n)
      break;
    fprintf(stderr, ", ");
  }
  fprintf(stderr, "]\n");
}

int ch;
unsigned char *inbuff;
int bufflength;
unsigned currpc;
int lastaction;
method_info *miptr;

Exp *stack[8];
Exp **stkptr;
Exp *donestack[256];
Exp **donestkptr;

//Block *blocks[16];
//int numblocks;
//int currblock;

int indentlevel;

/*
char pass1size[] = {
  1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
  2, 3, 2, 3, 3, 2, 2, 2,  2, 2, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 0, 0, 0, 2, 2,  2, 2, 2, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 1,  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,

  1, 1, 1, 1, 3, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
*/// 1,1,1,1,/*C*/1,1,1,1,1,/*.*/-1,-1,-1,-1,-1,-1,-1, // cmps
// -1,-1,-1,-1,-1,-1,-1,-2,-3,-4,-5,-6,/*R*/1,1,1,1, // returns
//  1,1/*.*/,3,3,3,3, 3, 3,  5, 3, 3, 3, 0, 3, 1, 1,
/*  3, 0, 0, 0, 0, 3,-1,-1,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
};

int addifblock(void)
{
  unsigned pcval = currpc - 1;
  Block *b = blocks[currblock++] = new Block;
  b->tag = IF;
  b->start_pc = pcval + 3;
  b->end_pc = pcval + (signed)(((unsigned)JDNEXT8() << 8) + (unsigned)JDNEXT8());
  b->else_pc = 0;
  b->exp = 0;
  fprintf(stderr, "if(?) goto %d else %d\n", b->end_pc, b->start_pc);
  return 0;
}

int addgotoblock(void)
{
  unsigned pcval = currpc - 1;
  Block *b = blocks[currblock++] = new Block;
  b->tag = GOTOLABEL;
  b->start_pc = pcval + 3;
  b->end_pc = pcval + (signed)(((unsigned)JDNEXT8() << 8) + (unsigned)JDNEXT8());
  b->else_pc = 0;
  b->exp = 0;
  fprintf(stderr, "goto %d instead of %d\n", b->end_pc, b->start_pc);
  return 0;
}

int (*pass1actions[])(void) = {
  0, addifblock, addgotoblock, 0, 0, 0, 0, 0,
  0, 0, 0
};
*/
char actions[] = {
  0, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 3, 3, 3, 4, 4, 4,  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4, 4, 4, 4,15, 0,
  0, 0,15, 0, 0, 0, 5, 5,  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,  5, 5, 5, 5, 5, 5, 5, 0,
  0, 0, 0,18, 0, 0, 0,21,  0,13,25, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 6, 6, 6, 6,  6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 7, 7, 7, 7,  6, 6, 6, 6, 6, 6, 6, 6,

  6, 6, 6, 6,16, 7, 7, 7,  7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 0,22,22,22,22, 22,12,12,12,12,12,12,23,
 23,23,23,23,23,23,23, 7,  0, 0,27,24,10,10,10,10,
 10,10, 8, 9, 8, 9,11,11, 11,11, 7, 7, 0,19,14, 7,
 17,26, 0, 0, 0,20/*?(multianewarray)*/,12,12,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
};

int (*actiontable[])(Classfile *c) = {
  0, pushimp, pushimm, pushconst, pushlocal, storelocal, pushbinop, pushunop,
  doget, doput, doreturn, invokefunc, doif1, dodup, doarraylength, doarrayget,
  iinclocal, docheckcast, doarrayput, anewarray, multianewarray, dopop, docmp, doif2,
  doluswitch, dodup_x1, doinstanceof, dotableswitch
};

int decompileblock(Classfile *c, method_info_ptr mi) {
  char *str;
//  int i;
  miptr = mi;
  cond_pcend = -1;
  char *strptr;
  int dodecompile = ((int)(c->options)) ^ 2;

  if (mi->name == "<init>") {
  }

  strptr = new char[mi->access_flags.strlen() + 1];
  fprintf(c->outfile, "\n  %s", mi->access_flags.toString(strptr));
  delete [] strptr;
  char *tmp = mi->sig;
  if (printsigname(c, c->outfile, tmp, mi->name, mi)) return 1;
  for (int m = 0; m != mi->num_throws;) {
    fprintf(c->outfile, " throws %s", c->cp(mi->throws[m++])->chp);
  }
  if ((mi->access_flags & (ACC_NATIVE | ACC_ABSTRACT))) dodecompile = 0;
  if (dodecompile) {

/**********************************************************************/
/** PASS1 - input stage, analyse opcodes, create stack of statements **/
/**********************************************************************/
    int action = 0;
    inbuff = mi->code;
    bufflength = mi->code_length;
    currpc = 0;
    stkptr = stack;
    donestkptr = donestack;
    
    indentlevel = 0;
//    indents_end = indents_begin = (intlist *)0;

    while (bufflength > 0) {
      if (((int)currpc == cond_pcend)) if (finishconditional(c)) return 1;
      ch = JDNEXT8U();
      lastaction = action;
      action = actions[ch];
      if (action) {
        if(actiontable[action](c)) return 1;
      } else {
        fprintf(c->outfile, "//    unknown opcode 0x%02X\n", ch);
      }
    }

    fprintf(c->outfile, " {\n");

/**********************************************************************/
/**  PASS2 - lazy conditionals (&& and ||), note backward jump refs  **/
/**********************************************************************/
    looplist *branchbacklist = new looplist();
    Exp **p = donestack;
    for (;p < (donestkptr-1);p++) {
      Exp *pptr = *p, *pptr1 = *(p+1), *pptr2 = *(p+2);
      if (pptr->e->op == GOTO)
        goto here;
      if (pptr->e->et == BRANCH) {
        if (pptr1->e->et == BRANCH || pptr1->e->op == COND) {
          unsigned minpc = min(pptr->minpc, pptr1->minpc);
          Exp *e;
          if (pptr->branch_pc == pptr1->branch_pc) {
            e = new Exp(minpc, BINARY, BOOLEAN, OR_BOOL,
                pptr->exp1, pptr1->exp1);
          } else {
            if (pptr->branch_pc == pptr2->minpc) {
              e = new Exp(minpc, BINARY, BOOLEAN, AND_BOOL,
                  pptr->exp1, pptr1->exp1);
              if (notexp(&(e->exp1))) return 1;
            } else {
              goto here;
            }
          }
          killexp(pptr);
          pptr1->exp1 = e;
          pptr1->minpc = minpc;
          *p = pptr1; *(++p) = 0;
          pptr = pptr1; pptr1 = pptr2;
        }
here:
        if (pptr->minpc >= pptr->branch_pc) {
          branchbacklist->add(new Loop(pptr->minpc, pptr->branch_pc,
                pptr1->minpc, pptr->exp1, LOOP_DOWHILE));
        }
      }
    }
    
/**********************************************************************/
/** PASS3 - analyse control flow, recursively print statement stack  **/
/**********************************************************************/
    intlist *iflist = new intlist();
    intlist *elselist = new intlist();
    looplist *branchbacklist2 = new looplist();
    
    p = donestack;
    while (p != donestkptr) {
     Exp *pptr = *p++;
     if (pptr) {
       if (!branchbacklist->isempty()) {
         Loop *l = branchbacklist->top();
         if (l->jumpto_pc == pptr->minpc) {
           l->type = LOOP_DOWHILE;
           fprintf(c->outfile, "    do {\n", str);
           indentlevel++;
           for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
           branchbacklist2->push(branchbacklist->pop());
         }
       }
       if (!branchbacklist->isempty()) {
         Loop *l = branchbacklist->top();
         if (pptr->e->op == GOTO && pptr->branch_pc == l->jumpfrom_pc) {
           l->type = LOOP_WHILE;
           str = l->condition->toString(0);
           fprintf(c->outfile, "    while (%s) {\n", str);
           delete [] str;
           indentlevel++;
           for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
           branchbacklist2->push(branchbacklist->pop());
           continue;
         }
       }
       if (!iflist->isempty() && iflist->top() == pptr->minpc) {
         iflist->pop();
         fprintf(c->outfile, "  }\n");
         indentlevel--;
         for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
       }
       if (pptr->e->op == GOTO || pptr->e->et == BRANCH) {
         if (!branchbacklist2->isempty()) {
           Loop *l = branchbacklist2->top();
           if (l->jumpfrom_pc == pptr->minpc) {
             if (l->type == LOOP_DOWHILE) {
               if (pptr->e->op == GOTO) {
                 fprintf(c->outfile, "  } while(true);\t/*%d*/\n", pptr->minpc);
               } else {
                 char *str = l->condition->toString(0);
                 fprintf(c->outfile, "  } while(%s);\t/*%d*/\n", str,
                     pptr->minpc);
                 delete [] str;
               }
             } else {
               fprintf(c->outfile, "  }\t/*%d*/\n", pptr->minpc);
             }
             branchbacklist2->pop();
             indentlevel--;
             for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
             continue;
           }
           if (branchbacklist2->containsPast(pptr->branch_pc)) {
             fprintf(c->outfile, "    break;\t/*%d*/\n", (*(p-1))->minpc);
             for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
             continue;
           }
         }
         if (!iflist->isempty() && iflist->top() == ((*p)?(*p)->minpc:0)) {
           elselist->push(pptr->branch_pc);
           iflist->pop();
           fprintf(c->outfile, "  } else {\t/*%d*/\n", (*(p-1))->minpc);
           for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
           continue;
         }
         if (pptr->e->et == BRANCH && pptr->branch_pc > pptr->minpc) {
           iflist->push(pptr->branch_pc);
           indentlevel++;
         }
       } else {
         if (pptr->e->op == RETURN && !iflist->isempty()
             && iflist->top() == ((*p)?(*p)->minpc:0)) {
           iflist->pop();
           str = pptr->toString(0);
           if (str) {
             fprintf(c->outfile, "    %s;\t/*%d*/\n", str, (*(p-1))->minpc);
             for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
           }
           delete [] str;
           fprintf(c->outfile, "  }\n");
           indentlevel--;
           for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
           continue;
         } else {
           if (!elselist->isempty() && elselist->top() == pptr->minpc) {
             elselist->pop();
             fprintf(c->outfile, "  }\n");
             indentlevel--;
             for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
           }
         }
       }
       str = pptr->toString(0);
       if (str) {
         fprintf(c->outfile, strrchr(str,'{')?"    %s":"    %s;", str);
         fprintf(c->outfile, "\t/*%d*/",(*(p-1))->minpc);
         fprintf(c->outfile, "\n");
         for (int i=indentlevel; i--;) fprintf(c->outfile, "  ");
       }
       delete [] str;
      }
    }
    fprintf(c->outfile, "  }");
  } else {
    fprintf(c->outfile, ";");
  }
  return 0;
}
