/* access.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <string.h>
#include "general.h"
#include "access.h"

char *flag2str[] = {
  "public ", "private ", "protected ", "static ",
  "final ", "synchronized ", "threadsafe ", "transient ",
  "native ", "interface ", "abstract "
};

int flag2strlen[] = {
  7, 8, 10, 7, 6, 13, 11, 10, 7, 10, 9
};

char *AccessFlags::toString(char *buffer)
{
  u16 f, i;

  *buffer = '\0';

  for (f = flags, i = 0; f; f>>=1, i++) {
    if (f & 1) {
      strcat(buffer, flag2str[i]);
    }
  }
  return buffer;
}

u16 AccessFlags::strlen()
{
  u16 buffsize = 0, f, i;

  for (f = flags, i = 0; f; f>>=1, i++) {
    if (f & 1) {
      buffsize += flag2strlen[i];
    }
  }

  return buffsize;
}
