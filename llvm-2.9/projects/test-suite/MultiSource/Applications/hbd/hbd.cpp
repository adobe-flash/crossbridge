/* hbd.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <stdio.h>
#include "class.h"

int debugon = 0;

int main(int argc, char **argv)
{
  fprintf(stderr, "HomeBrew Decompiler.  Copyright (c) 1994-2003 Pete Ryland.\n");
  Classfile c(argc, argv);
  c.read();
  c.print();
  return 0;
}
