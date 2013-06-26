/* cp.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef CP_H
#define CP_H

#include "general.h"

/* These are structures that the constant pool's table
   can contain. */

typedef struct {
  u16 class_index;
  u16 name_and_type;
} Ref;

typedef struct {
  u16 name_index;
  u16 signature_index;
} NameAndType;

/* The generic structure of constants that will appear
   in the table of constants in the constant pool */
struct cp_info {
  unsigned char tag;
  union {
    long i;
    void *p;
    double d;
    float f;
    char *chp;
  };
};

/* Forward declaration of the Classfile struct */
struct Classfile;

/* The ConstPool struct, which contains the Constant
   Pool. */
struct ConstPool {
  /* The number of entries in this Constant Pool */
  u16 constant_pool_count;

  /* The table of constants */
  cp_info *constant_pool;

  /* This will parse the input file for a constant pool */
  void read(Classfile *c, u16 *imports_count);

  /* These methods provide easy access to commonly
     used parts of constant pool entries */
  cp_info *operator [](u16 i) {
    return &(constant_pool[i]);
  }
  cp_info *operator ()(u16 i) {
    return &(constant_pool[constant_pool[i].i]);
  }
  u16 count() {
    return constant_pool_count;
  }
};

#endif
