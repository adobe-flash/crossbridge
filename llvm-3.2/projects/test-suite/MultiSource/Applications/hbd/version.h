/* version.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef VERSION_H
#define VERSION_H

#include "general.h"

/* Forward declaration */
struct Classfile;

/* This struct is for the class file version */
struct ClassVersion {
  u16 minor_version;
  u16 major_version;

  /* read() is used by the decompiler to read
     the version of the input class file and
     make sure it is a version that we support */
  void read(Classfile *c);
};

#endif
