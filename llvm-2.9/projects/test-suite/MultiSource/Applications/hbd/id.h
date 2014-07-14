/* id.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef ID_H
#define ID_H

#include "sig.h"

/* The location of an identifier */
enum Loc {
  NO /* Not Applicable */, IM /* Immediate */,
  CP /* Const Pool */, LO /* Local */
}; /* Id location */

/* The Id structure, which contains information
   about one identifier. */
struct Id {
  /* Its name */
  char *name;

  /* Its Java type */
  Type type;

  /* Where this ident is located */
  Loc loc;

  /* An index into the "loc" of where this ident is */
  int locinfo;

  union {
    long linfo;
    double dinfo;
    long llinfo[2];
  };
};

/* "Standard" identifiers used by the standard Exp_'s */
extern Id idnull, idneg1,
          id0i, id1i, id2i, id3i, id4i, id5i,
          id0L, id1L, 
	  id0f, id1f, id2f,
          id0d, id1d,
          idfalse, idtrue;

#endif
