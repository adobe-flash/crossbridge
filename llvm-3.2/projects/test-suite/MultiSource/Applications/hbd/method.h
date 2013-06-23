/* method.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef METHOD_H
#define METHOD_H

#include "access.h"
#include "exp.h"

/* These are used by our table of blocks */
typedef enum { TRY, IF, DOWHILE, WHILE, GOTOLABEL } Blocktype;

typedef struct {
  /* The type of block this is */
  Blocktype tag;

  /* Its starting and ending code offsets */
  unsigned short start_pc;
  unsigned short end_pc;

  /* Other offset, depending on the block type */
  union {
    unsigned else_pc;
    unsigned short handler_pc;
  };

  /* An index into the constant pool
     of the class we're catching */
  unsigned short catch_type;

  /* The list of expressions within this block */
  Exp *exp;
} Block, ExceptionTableEntry;

/* The line number table has these as entries.
   They are currently parsed from the input file,
   but not used */
typedef struct {
  unsigned short start_pc;
  unsigned short line_number;
} LineNumberTableEntry;

/* This table stores the local table information.
   This is parsed from the input file in the
   optional LocalVariableTable attribute */
typedef struct {
  /* The first code offset where this var is used */
  unsigned short start_pc;

  /* The length of the scope of this local */
  unsigned short length;

  /* The constant pool index to its name */
  unsigned short name_index;

  /* The constant pool index to its type signature */
  unsigned short signature_index;

  /* Which local we are talking about */
  unsigned short slot;
} LocalVariableTableEntry;

/* The main method structure */
typedef struct {
  /* The access flags for this method */
  AccessFlags access_flags;

  /* The name of the method */
  char *name;

  /* Its type signature */
  char *sig;

  /* The maximum stack size the JVM code can use */
  unsigned char max_stack;

  /* The maximium number of locals in scope at any one time */
  unsigned char max_locals;

  /* The length of the code in bytes */
  unsigned code_length;

  /* The code */
  unsigned char *code;

  /* The exception table */
  unsigned short exception_table_length;
  ExceptionTableEntry *exception_table;

  /* The line number table */
  unsigned short line_number_table_length;
  LineNumberTableEntry *line_number_table;

  /* The local variable table */
  unsigned short local_variable_table_length;
  LocalVariableTableEntry *local_variable_table;

  /* A table of the names of the locals.
     These are potentially created
     as each local is encountered. */
  char **local_names;

  /* The type signatures of the locals.
     These need to be guessed if there is no
     LocalVariableTable attribute in the method. */
  char **local_sigs;

  /* The first code offset where each local is used.
     Again, these need to be guessed if there is no
     LocalVariableTable attribute in the method. */
  unsigned *local_firstuses;

  /* The types of the locals,
     corresponding to the local_sigs above */
  Type *local_types;

  /* The type signature of the
     return value of the method */
  char *ret_sig;

  /* The type of the return value
     corresponding to the signature above */
  Type ret_type;

  /* The number of different exceptions that
     this method is able to throw and not catch */
  int num_throws;

  /* A table of constant-pool entries to the classes
     that this method throws */
  int *throws;
} method_info, *method_info_ptr;

/* This global contains the method
   we are currently working on */
extern method_info *miptr;

/* Forward declaration */
typedef struct Classfile Classfile;
/* Decompiles the given method's code */
int decompileblock(Classfile *c, method_info_ptr mi);

#endif
