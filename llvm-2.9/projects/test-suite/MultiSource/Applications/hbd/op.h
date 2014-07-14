/* op.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef _OP_H_
#define _OP_H_

/* The operations that can go in
   the op field of the Exp struct */
enum Op {
  ADD, SUB, MUL, DIV,
  MOD, DOT, ASSIGN, SHL,
  SHR, USHR, AND, OR,
  XOR, NOT, NEG, CAST,
  RETURN, THROW, NEW, GOTO,
  ADDASSIGN, SUBASSIGN, INC, DEC,
  COND, COND_, CMP, DUMMY,
  EQUAL, NOTEQUAL, LESS, GREATEROREQUAL,
  GREATER, LESSOREQUAL, NOT_BOOL, AND_BOOL,
  OR_BOOL, INSTANCEOF, COMMA, ID
};

/* The Java string representation of the operations */
extern char *op2str[];

/* The precedence of the ops */
extern int op_prec[];

/* The accociativity of the ops */
extern int op_assoc[];

#endif
