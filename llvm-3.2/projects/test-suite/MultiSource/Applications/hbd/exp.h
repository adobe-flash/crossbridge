/* exp.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef EXP_H
#define EXP_H

#include "id.h"
#include "op.h"

/* The various types of expressions */
enum Exptype {
  NOEXP, IDENT, PREUNARY, POSTUNARY,
  BINARY, TERNARY, FUNCTIONCALL, ARRAYACCESS,
  BRANCH, SWITCH
};

/*
   This structure is a secondary structure to
   the actual Exp struct.  It is used to allow
   us to have a set of "standard" Expressions
   for things like an integer constant "1" which
   is often used.  This saves both time and memory.
*/
struct Exp_ {
  int isstd;  /* 1 if this is one of the standard Exp_'s */
  Exptype et; /* The tpye of expression this is */
  Type type;  /* The Java type of the result of this Exp_ */
  Op op;      /* The operation performed in this Exp_ */
  Id *id;     /* A reference to an Id if this Exp_
                 is simply an identifier */

  /* This constructor creates an expression for the
     given identifier */
  Exp_(Id *idptr) {
    isstd = 1; et = IDENT; type = idptr->type;
    op = ID; id = idptr;
  }

  /* These constructors will create more general Exp_'s */
  Exp_(Exptype _et, Type _type, Op _op) {
    isstd = 0; et = _et; type = _type; op = _op;
  }
  Exp_(Exptype _et, Type _type, Op _op, Id *_id) {
    isstd = 0; et = _et; type = _type; op = _op; id = _id;
  }
  Exp_(int _isstd, Exptype _et, Type _type, Op _op, Id *_id) {
    isstd = _isstd; et = _et; type = _type; op = _op; id = _id;
  }
};

/* This is the table of commonly-used Exp_'s */
extern Exp_ std_exps[];

struct Case { long caseval; long branch_pc; };

/* The main Exp struct */
struct Exp {
  /* The secondary Exp_ structure for this Exp */
  Exp_ *e;

  /* A reference count */
  unsigned numrefs;

  unsigned pc;
  unsigned minpc;
  Exp *exp1, *exp2;

  union {
    Exp *exp3;
    unsigned default_pc;
  };
  union {
    unsigned numexps;
    unsigned branch_pc;
    unsigned numcases;
  };
  union {
    Exp **explist;
    Case *cases;
  };

  /* Constructors for "standard" Exp_'s */
  Exp(unsigned pcval, unsigned stdexpnum) {
    numrefs = 1; pc = minpc = pcval; e = std_exps + stdexpnum;
  }
  Exp(unsigned pcval, unsigned minpcval,
      unsigned stdexpnum, Exp *_exp1, unsigned branchpcval) {
    numrefs = 1; pc = pcval; minpc = minpcval;
    e = std_exps + stdexpnum;
    exp1 = _exp1; branch_pc = branchpcval;
  }

  /* A constructor for Identifiers */
  Exp(unsigned pcval, Exptype et, Type type, Op op, Id *id) {
    numrefs = 1; pc = minpc = pcval;
    e = new Exp_(et, type, op, id);
  }

  /* A fairly general constructor */
  Exp(unsigned pcval, Exptype et,
      Type type, Op op, Exp *_exp1 = 0, Exp *_exp2 = 0) {
    numrefs = 1; pc = minpc = pcval;
    e = new Exp_(et, type, op);
    exp1 = _exp1; exp2 = _exp2;
  }

  /* A constructor for case "expressions" */
  Exp(unsigned pcval, unsigned minpcval, Exptype et,
      Type type, Op op, Exp *_exp1, unsigned defaultpcval,
      unsigned _numcases, Case *_cases) {
    numrefs = 1; pc = pcval; minpc = minpcval;
    e = new Exp_(et, type, op);
    exp1 = _exp1; default_pc = defaultpcval;
    numcases = _numcases; cases = _cases;
  }

  /* A constructor for method calls */
  Exp(unsigned pcval, unsigned minpcval, Exptype et,
      Type type, Op op, Exp *_exp1,
      unsigned _numexps, Exp** _explist) {
    numrefs = 1; pc = pcval; minpc = minpcval;
    e = new Exp_(et, type, op);
    exp1 = _exp1; numexps = _numexps;
    explist = _explist;
  }

  /* A constructor for conditionals */
  Exp(unsigned pcval, Exptype et, Type type,
      Op op, Exp *_exp1, unsigned branchpcval) {
    numrefs = 1; pc = minpc = pcval;
    e = new Exp_(et, type, op);
    exp1 = _exp1; branch_pc = branchpcval;
  }

  /* Another general constructor */
  Exp(unsigned pcval, int minpcval, Exptype et,
      Type type, Op op, Exp *_exp1, Exp *_exp2 = 0) {
    numrefs = 1; pc = pcval; minpc = minpcval;
    e = new Exp_(et, type, op);
    exp1 = _exp1; exp2 = _exp2;
  }

  /* Another constructor for identifiers */
  Exp(unsigned pcval, char *idname,
      Type idtype, Loc idloc, int idlocinfo = 0) {
    Id *id = new Id; id->name = idname; id->type = idtype;
    id->loc = idloc; id->locinfo = idlocinfo;
    numrefs = 1; pc = minpc = pcval;
    e = new Exp_(IDENT, idtype, ID, id);
  }

  /* Provides a string representation of this Expression */
  char *toString(unsigned nextpc);
};

/* This enum provides an easier way to look up the table of
   "standard" Exp_'s */
enum stdexp_vals {
  NULLEXP, INEG1EXP, I0EXP, I1EXP, I2EXP, I3EXP, I4EXP, I5EXP,
  L0EXP, L1EXP, F0EXP, F1EXP, F2EXP, D0EXP, D1EXP, FALSEEXP,
  TRUEEXP, IFEXP
};

/* This recursively deletes an expression and it's children */
void killexp(Exp *e);

/* This will negate a boolean expression. It will cause an
   error if called on a non-boolean expression */
int notexp(Exp **e);

/* These handle the indent level during the print phase */

struct intnode {
  unsigned node;
  intnode *next;
  
  intnode(unsigned i) {
    node = i;
    next = 0;
  }
  
  intnode(unsigned i, intnode *n) {
    node = i;
    next = n;
  }
};

struct intlist {
  intnode *head;
  intnode *tail;
  
  intlist() {
    head = 0;
    tail = 0;
  }

  void push(unsigned i) {
    intnode *n = new intnode(i, head);
    head = n;
    if (tail == 0)
      tail = head;
  }

  void add(unsigned i) {
    intnode *n = new intnode(i);
    if (tail == 0) {
      head = tail = n;
    } else {
      tail->next = n;
      tail = n;
    }
  }

  unsigned top() {
    return head?head->node:0;
  }
  
  int isempty() {
    return head==0;
  }
  
  unsigned pop() {
    intnode *n = head;
    unsigned i = n->node;
    head = n->next;
    delete n;
    if (head == 0)
      tail = 0;
    return i;
  }
  
  int contains(unsigned i) {
    intnode *n = head;
    while (n) {
      if (n->node == i)
        return 1;
      n = n->next;
    }
    return 0;
  }
};

typedef enum { LOOP_WHILE, LOOP_DOWHILE, LOOP_FOR } LoopType;

struct Loop {
  unsigned jumpfrom_pc, jumpto_pc, jumppast_pc;
  Exp *condition;
  LoopType type;
  
  Loop(unsigned from, unsigned to, unsigned past, Exp *cond, LoopType lt) {
    jumpfrom_pc = from; jumpto_pc = to; jumppast_pc = past;
    condition = cond; type = lt;
  }
};

struct loopnode {
  Loop *node;
  loopnode *next;
  
  loopnode(Loop *e) {
    node = e;
    next = 0;
  }
  
  loopnode(Loop *e, loopnode *n) {
    node = e;
    next = n;
  }
};

struct looplist {
  loopnode *head;
  loopnode *tail;
  
  looplist() {
    head = 0;
    tail = 0;
  }

  void push(Loop *e) {
    loopnode *n = new loopnode(e, head);
    head = n;
    if (tail == 0)
      tail = head;
  }

  void add(Loop *e) {
    loopnode *n = new loopnode(e);
    if (tail == 0) {
      head = tail = n;
    } else {
      tail->next = n;
      tail = n;
    }
  }

  Loop *top() {
    return head?head->node:0;
  }
  
  int isempty() {
    return head==0;
  }
  
  Loop *pop() {
    loopnode *n = head;
    Loop *e = n->node;
    head = n->next;
    delete n;
    if (head == 0)
      tail = 0;
    return e;
  }
  
  int containsPast(unsigned i) {
    loopnode *n = head;
    while (n) {
      if (n->node->jumppast_pc == i)
        return 1;
      n = n->next;
    }
    return 0;
  }
};

extern int indentlevel;

#endif
