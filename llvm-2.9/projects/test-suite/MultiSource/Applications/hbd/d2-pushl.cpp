/* d2-pushl.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include "general.h"
#include "exp.h"
#include "decomp.h"
#include "method.h"

int pushlocal(Classfile *c) /* push value from local (or params) */
{
  int pcval = currpc - 1;
  int val;
  Type idtype = VOID;
  if (ch < 0x1A) {
    val = JDNEXT8S();
    switch (ch) {
      case 0x15: idtype = INT; break;
      case 0x16: idtype = LONG; break;
      case 0x17: idtype = FLOAT; break;
      case 0x18: idtype = DOUBLE; break;
      case 0x19: idtype = OBJECT; break;
    }
  } else if (ch < 0x1E) {
    val = ch - 0x1A;
    idtype = INT;
  } else if (ch < 0x22) {
    val = ch - 0x1E;
    idtype = LONG;
  } else if (ch < 0x26) {
    val = ch - 0x22;
    idtype = FLOAT;
  } else if (ch < 0x2A) {
    val = ch - 0x26;
    idtype = DOUBLE;
  } else /* if (ch < 0x2E) */ {
    val = ch - 0x2A;
    idtype = OBJECT;
  }
  char *tmpstr = miptr->local_names[val], *idname;
  if (tmpstr) {
    idname = new char[strlen(tmpstr) + 1];
    strcpy(idname, tmpstr);
  } else {
    fprintf(stderr, "Error in code: local used before defined.\n");
    return 1;
  }
  Exp *e = new Exp(pcval, idname, idtype, LO, val);
//  if((lastaction == 16)&&(!strcmp((*(donestkptr-1))->exp1->e->id->name,id->name))) {
//    *stkptr++ = *(--donestkptr);
    *stkptr++ = e;
//  } else {
//    *stkptr++ = e;
//  }
  return 0;
}
