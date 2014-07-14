/* access.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef ACCESS_H
#define ACCESS_H

#include "general.h"

/* The various types of Access flags Java uses */
enum Access {
  ACC_PUBLIC = 0x0001, /* visible to everyone */
  ACC_PRIVATE = 0x0002, /* visible only to defining class */
  ACC_PROTECTED = 0x0004, /* visible to subclasses */
  ACC_STATIC = 0x0008, /* instance variable is static */
  ACC_FINAL = 0x0010, /* no further subclassing, overriding */
  ACC_SYNCHRONIZED = 0x0020, /* wrap method call */
                              /* in monitor lock */
  ACC_THREADSAFE = 0x0040, /* can cache in registers */
  ACC_TRANSIENT = 0x0080, /* not persistant */
  ACC_NATIVE = 0x0100, /* implemented in C */
  ACC_INTERFACE = 0x0200, /* class is an interface */
  ACC_ABSTRACT = 0x0400 /* no definition provided */
};

/*
   A class representing a set of access flags with
   various useful methods.
*/

struct AccessFlags {
  u16 flags;

  AccessFlags() {
    flags = 0;
  }

  AccessFlags(u16 inflags) {
    flags = inflags;
  }

  void operator =(u16 inflags) {
    flags = inflags;
  }

  void operator +=(Access a) {
    flags |= (u16)a;
  }

  void operator -=(Access a) {
    flags &= ~(u16)a;
  }

  int operator &(u16 intflags) {
    return flags & intflags;
  }

  char *toString(char *buffer);
  u16 strlen();
};

#endif
