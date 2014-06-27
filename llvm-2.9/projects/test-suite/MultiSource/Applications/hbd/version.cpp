/* version.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <stdio.h>
#include "general.h"
#include "version.h"
#include "file.h"
#include "err.h"
#include "class.h"

void ClassVersion::read(Classfile *c) {
  minor_version = get16(c->infile, &c->infile_pos);
  if ((major_version = get16(c->infile, &c->infile_pos)) != 45)
    fatalerror(BAD_VERSION_ERR);
  else if (minor_version != 3) {
    fprintf(stderr, "Warning: Class Version 45.%d. (Program designed for ver 45.3)\n", minor_version);
  }
}
