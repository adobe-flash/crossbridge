/* err.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <stdio.h>
#include <stdarg.h>
#include "general.h"
#include "options.h"

char *errmsgs[] = {
  "Unknown error.",
  "Out of memory error.",
  DEBUG_ON? (char*)"Usage: %s [-O] [-D] InFile.class [OutFile.java]\n"
           :(char*)"Usage: %s [-O] InFile.class [OutFile.java]\n",
  DEBUG_ON? (char*)"Usage: %s [-D] -Ifuncname InFile.class\n"
           :(char*)"Usage: %s -Ifuncname InFile.class\n",
  "Not a class.",
  "Unsupported Class Version.",
  "3"
};
void fatalerror(int msgid,...)
{
  va_list ap;
  va_start(ap, msgid);
  vfprintf(stderr, errmsgs[msgid], ap);
  va_end(ap);
  exit(msgid);
}
