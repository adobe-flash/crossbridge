/* errhandl.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef ERRHANDL_H
#define ERRHANDL_H

/* The various errors that can occur in the programs.
   These are passed to the fatalerror() function
   below. */
enum errorids {
  UNKNOWN_ERR, OUT_OF_MEM_ERR,
  COMMAND_LINE_ERR_HBD, COMMAND_LINE_ERR_HBT,
  NOT_A_CLASS_ERR, BAD_VERSION_ERR, CP_ERR
};

/* This function will exit the program giving
   an appropriate error.  The msgid should be
   from the enum above */
void fatalerror(int msgid,...);

/* Since this is commonly used, we have a macro for it */
#define memerr() fatalerror(OUT_OF_MEM_ERR)

#endif
