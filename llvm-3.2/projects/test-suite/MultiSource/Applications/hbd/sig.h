/* sig.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef SIG_H
#define SIG_H

#include <stdio.h>

/* The Java types */
typedef enum {
  VOID, BYTE, CHAR, SHORT,
  INT, LONG, FLOAT, DOUBLE,
  OBJECT, ARRAY, BOOLEAN, FUNC,
  CMPTYPE, /* -1, 0, or 1 */
  UNKNOWN, UNKNOWN_1, UNKNOWN_4,
  UNKNOWN_8
} Type;

/* This converts a signature string to a type */
Type sig2type(char* sig);

/* This contains string representations of the types */
extern char *type2str[];

/* These defines are used for converting sigs */
#define SIGNATURE_ANY           'A'
#define SIGNATURE_ARRAY         '['
#define SIGNATURE_BYTE          'B'
#define SIGNATURE_CHAR          'C'
#define SIGNATURE_CLASS         'L'
#define SIGNATURE_ENDCLASS      ';'
#define SIGNATURE_ENUM          'E'
#define SIGNATURE_FLOAT         'F'
#define SIGNATURE_DOUBLE        'D'
#define SIGNATURE_FUNC          '('
#define SIGNATURE_ENDFUNC       ')'
#define SIGNATURE_INT           'I'
#define SIGNATURE_LONG          'J'
#define SIGNATURE_SHORT         'S'
#define SIGNATURE_VOID          'V'
#define SIGNATURE_BOOLEAN       'Z'

#define SIGNATURE_ANY_STRING            "A"
#define SIGNATURE_ARRAY_STRING          "["
#define SIGNATURE_BYTE_STRING           "B"
#define SIGNATURE_CHAR_STRING           "C"
#define SIGNATURE_CLASS_STRING          "L"
#define SIGNATURE_ENDCLASS_STRING       ";"
#define SIGNATURE_ENUM_STRING           "E"
#define SIGNATURE_FLOAT_STRING          "F"
#define SIGNATURE_DOUBLE_STRING         "D"
#define SIGNATURE_FUNC_STRING           "("
#define SIGNATURE_ENDFUNC_STRING        ")"
#define SIGNATURE_INT_STRING            "I"
#define SIGNATURE_LONG_STRING           "J"
#define SIGNATURE_SHORT_STRING          "S"
#define SIGNATURE_VOID_STRING           "V"
#define SIGNATURE_BOOLEAN_STRING        "Z"

/* Forward declaration */
struct Classfile;

/* This prints the method name with return type
   and parameters in the format used in Java source files */
int printsigname(Classfile *c, FILE *outfile,
                 char *&sig, char *name, void *mi);

#endif
