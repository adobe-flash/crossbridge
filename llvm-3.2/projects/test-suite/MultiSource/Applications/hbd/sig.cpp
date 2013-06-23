/* sig.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <stdio.h>
#include <string.h>
#include "general.h"
#include "class.h"
#include "method.h"
#include "sig.h"
#include "err.h"

char *type2str[] = {
  "void", "byte", "char", "short", "int", "long", "float", "double", "object"
};

Type sig2type(char* sig)
{
  switch (*sig) {
    case SIGNATURE_BYTE: return BYTE;
    case SIGNATURE_CHAR: return CHAR;
    case SIGNATURE_DOUBLE: return DOUBLE;
    case SIGNATURE_FLOAT: return FLOAT;
    case SIGNATURE_INT: return INT;
    case SIGNATURE_LONG: return LONG;
    case SIGNATURE_CLASS: return OBJECT;
    case SIGNATURE_SHORT: return SHORT;
    case SIGNATURE_BOOLEAN: return BOOLEAN;
    case SIGNATURE_ARRAY: return OBJECT;
    case SIGNATURE_FUNC: return FUNC;
    case SIGNATURE_VOID: return VOID;
    default:
      fprintf(stderr, "Error converting signature to a type.\n");
      exit(1);
  }
  return VOID;
}

int printsigname(Classfile *c, FILE* outfile, char *&sig, char *name, void *mip)
{
  method_info_ptr mi = (method_info_ptr)mip;
  int i;
  char *t, *t2;
  switch(*sig++) {
    case SIGNATURE_BYTE: fprintf(outfile, "byte %s", name); return 0;
    case SIGNATURE_CHAR: fprintf(outfile, "char %s", name); return 0;
    case SIGNATURE_DOUBLE: fprintf(outfile, "double %s", name); return 0;
    case SIGNATURE_FLOAT: fprintf(outfile, "float %s", name); return 0;
    case SIGNATURE_INT: fprintf(outfile, "int %s", name); return 0;
    case SIGNATURE_LONG: fprintf(outfile, "long %s", name); return 0;
    case SIGNATURE_CLASS:
      t = sig;
      while (*sig++ != ';') ;
      if ((t2 = new char[sig - t]) == 0) memerr();
      strncpy(t2, t, sig - t - 1);
      t2[sig - t - 1] = '\0';
      t = t2;
      if (!strncmp(t, "java/lang/", 10)) t += 10;
      else while ((t2 = strchr(t2, '/')) != 0) *t2 = '.';
      i = c->package_name?strlen(c->package_name):0;
      if (c->package_name && !strncmp(t, c->package_name, i)) t += i + 1;
      fprintf(outfile, "%s %s", t, name);
      return 0;
    case SIGNATURE_SHORT: fprintf(outfile, "short %s", name); return 0;
    case SIGNATURE_BOOLEAN: fprintf(outfile, "boolean %s", name); return 0;
    case SIGNATURE_ARRAY:
      i = 0;
      while ((*sig >= '0') && (*sig <= '9')) i = (i * 10) + *sig++ - '0';
      printsigname(c, outfile, sig, name, mi);
      if (i) fprintf(outfile, "[%d]", i); else fprintf(outfile, "[]");
      return 0;
    case SIGNATURE_FUNC:
      if (!mi) {
        fprintf(stderr, "Non-function with function sig!\n");
        return 0;
      }
      t = sig;
      while (*sig++ != SIGNATURE_ENDFUNC) /* skip for now */;
      if (!strcmp(name, "<clinit>")) {
//      fprintf(outfile, "\b");
        return 0;
      }
      if ((mi->ret_sig = new char[strlen(sig) + 1]) == 0) memerr();
      strcpy(mi->ret_sig, sig);
      mi->ret_type = sig2type(mi->ret_sig);
      if (strcmp(name, "<init>"))
        printsigname(c, outfile, sig, name, mi); /* return type and name */
      else
        fprintf(outfile, "%s", c->this_class_name);
      fprintf(outfile, "(");
      mi->max_locals++;
      if (!mi->local_variable_table_length) {
        if (((mi->local_names = new char_ptr[mi->max_locals]) == 0) ||
            ((mi->local_sigs = new char_ptr[mi->max_locals]) == 0) ||
            ((mi->local_types = new Type[mi->max_locals]) == 0) ||
            ((mi->local_firstuses = new unsigned[mi->max_locals]) == 0)) memerr();
        for (int it = mi->max_locals; it--; ) {
          mi->local_firstuses[it] = 0;
          mi->local_names[it] = mi->local_sigs[it] = 0;
          mi->local_types[it] = VOID;
        }
        if ((mi->access_flags & ACC_STATIC) == 0) {
          mi->local_names[0] = "this";
          mi->local_sigs[0] = "L";
          mi->local_types[0] = OBJECT;
          mi->local_firstuses[0] = 0;
        }
      }
      i = ((mi->access_flags & ACC_STATIC) == 0) ? 1 : 0;
      while (*t != SIGNATURE_ENDFUNC) {
        if (mi->local_variable_table_length) {
          if (strcmp(t,mi->local_sigs[i])) {
            fprintf(stderr, "Function Parameter type mismatch\n");
            return 1;
          }
          printsigname(c, outfile, t,mi->local_names[i],mi);
        } else {
          if ((mi->local_names[i] = new char[6]) == 0) memerr();
          sprintf(mi->local_names[i], "var%d", i);
          char *t2 = t;
          printsigname(c, outfile, t,mi->local_names[i],mi);
          if ((mi->local_sigs[i] = new char[t - t2 + 1]) == 0) memerr();
          strncpy(mi->local_sigs[i], t2, t-t2);
          mi->local_sigs[i][t-t2] = '\0';
          mi->local_types[i] = sig2type(mi->local_sigs[i]);
          mi->local_firstuses[i] = 0;
        }
        if ((*(t-1) == 'D') || (*(t-1) == 'J')) i++;
        i++;
        if (*t != SIGNATURE_ENDFUNC) fprintf(outfile, ", ");
      }
      fprintf(outfile, ")");
      return 0;
    case SIGNATURE_VOID: fprintf(outfile, "void %s", name); return 0;
//    case 0: fprintf(outfile, "%s", name); return 0;
    default:
      fprintf(stderr, "Error reading type signature!\n");
      return 1;
  }
}

