/* class.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef CLASS_H
#define CLASS_H

#include <stdio.h>
#include "options.h"
#include "version.h"
#include "cp.h"
#include "access.h"
#include "general.h"
#include "field.h"
#include "method.h"

/* The master classfile structure. */
struct Classfile {
  /* The input and output files */
  FILE *infile, *outfile;

  /* These keeps a tab on where in the files we are */
  int infile_pos, outfile_pos;

  /* The command-line options we are using */
  CL_Options options;
  
  /* The basic class file format starts from here */

  /* The class version - should always be 45.3 */
  ClassVersion version;

  /* The Constant Pool */
  ConstPool cp;

  /* The access modifiers for this class */
  AccessFlags access_flags;

  /* An index into the constant pool of the name
     of this class. */
  u16 this_class;

  /* This contains the name of the package for this class,
     resolved from the above constant-pool reference */
  char *package_name;

  /* This contains the name of the actual class,
     resolved from the above constant-pool reference */
  char *this_class_name;

  /* This is an index into the constant pool for the name
     of the parent class */
  u16 super_class;

  /* The resolved string */
  char *super_class_name;

  /* This table contains indexes into the constant pool
     for the interfaces that this class implements */
  u16 interfaces_count;
  u16 *interfaces;

  /* This table contains the fields in this class */
  u16 fields_count;
  field_info_ptr *fields;

  /* This table contains the methods in this class */
  u16 methods_count;
  method_info_ptr *methods;

  /* The name of the source file from which this
     class was compiled.  This is determined from
     the SourceFile attribute. */
  char *source_name;

  /* This table holds the import statements that this
     class would have needed to enable it to refer to
     all the classes it uses without referring to
     their package name.  It is determined from clues
     the parser finds along the way. */
  u16 imports_count;
  char **imports;

  /* The function that the translator will be inserting
     the code for */
  char *functoinsert;

  /* The constructor, which will parse the command-line
     arguments */
  Classfile(int argc, char **argv);

  /* The read() and print() methods are only used by the
     decompiler */
  /* The read() method parses the input file */
  void read();
  /* print() decompiles and prints the decompilation
     of the class */
  void print();
};

#endif
