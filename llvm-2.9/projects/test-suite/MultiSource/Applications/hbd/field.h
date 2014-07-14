/* field.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef FIELD_H
#define FIELD_H

#include "access.h"

/* This struct contains the info for one field */
typedef struct {
  /* The access flags for this field */
  AccessFlags access_flags;

  /* The name of this field.
     Resolved from the constant pool */
  char *name;

  /* The signature of this field.
     Also, resolved from the constant pool */
  char *sig;

  /* 1 if this is a constant */
  int isconstant;

  /* If it is a constant, its index to the constant pool */
  u16 constval_index;
} field_info, *field_info_ptr;

#endif
