/* d3-popl.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <stdlib.h>
#include "exp.h"
#include "class.h"
#include "decomp.h"
#include "cp.h"

int storelocal(Classfile *c) /* pop value to local (including params) */
{
  unsigned pcval = currpc - 1;
  int val;
  Type idtype = VOID;
  if (ch < 0x3B) {
    switch (ch) {
      case 0x36: idtype = INT; break;
      case 0x37: idtype = LONG; break;
      case 0x38: idtype = FLOAT; break;
      case 0x39: idtype = DOUBLE; break;
      case 0x3A: idtype = OBJECT; break;
    }
    val = JDNEXT8S();
  } else if (ch < 0x3F) {
    val = ch - 0x3B;
    idtype = INT;
  } else if (ch < 0x43) {
    val = ch - 0x3F;
    idtype = LONG;
  } else if (ch < 0x47) {
    val = ch - 0x43;
    idtype = FLOAT;
  } else if (ch < 0x4B) {
    val = ch - 0x47;
    idtype = DOUBLE;
  } else {
    val = ch - 0x4B;
    idtype = OBJECT;
  }
//  if (val) {
    char *tmpstr = miptr->local_names[val], *idname;
    if (tmpstr) {
      idname = new char[strlen(tmpstr) + 1];
      strcpy(idname, tmpstr);
      idtype = miptr->local_types[val];
    } else {
      idname = miptr->local_names[val] = new char[7];
      sprintf(idname, "var%d", val);
      if ((miptr->local_types[val] == VOID)||(miptr->local_types[val] == UNKNOWN)) {
	if (idtype != INT)
	  miptr->local_types[val] = idtype;
	else
	  miptr->local_types[val] = UNKNOWN;
      }
    }
  Exp *e1 = new Exp(pcval, idname, idtype, LO, val);
  Exp *e2 = *(--stkptr);
  Exp *e = new Exp(pcval, min(pcval, e2->minpc), BINARY, idtype, ASSIGN, e1, e2);
  if (!tmpstr) miptr->local_firstuses[val] = e->minpc;
  if ((e2->e->type == INT) && (e1->e->type == BOOLEAN))
    if ((e2->e == std_exps + 2)||(e2->e == std_exps + 3)) /* 0 or 1 */
      e2->e += 13; /* false or true */
    else
      /* CMPEQ */;
  *donestkptr++ = e;
  return 0;
}

int iinclocal(Classfile *c) /* increment local by value */
{
  int pcval = currpc - 1;
  int val;
  val = JDNEXT8S();
  char *id1name;
  char *tmpstr = miptr->local_names[val];
  if (tmpstr) {
//    id1name = new char[strlen(tmpstr) + 1];
//    strcpy(id1name, tmpstr);
    id1name = tmpstr;
  } else {
    printf("Local int used before defined.\n");
    return 1;
  }
  if (miptr->local_types[val] == UNKNOWN) miptr->local_types[val] = INT;
//  if (miptr->local_types[val] == UNKNOWN) miptr->local_types[val] = INT;
//  if (miptr->local_types[val] == VOID) miptr->local_types[val] = INT;
  if ((miptr->local_types[val] != INT)&&(miptr->local_types[val] != SHORT)) {
    printf("Incrementation of local var%d of type %d i.e. %s.\n", val, miptr->local_types[val], type2str[miptr->local_types[val]]);
    return 1;
  }
  int incnum = JDNEXT8S();
//  id2->linfo = incnum;
  Exp *e;
  if ((incnum!=1)&&(incnum!=-1)) {
    Exp *e1 = new Exp(pcval, id1name, INT, LO, val);
    char *id2name = new char[5]; sprintf(id2name, "%ld", labs(incnum));
    Exp *e2 = new Exp(pcval, id2name, INT, IM);
    e = new Exp(pcval, BINARY, INT, (incnum<0)?SUBASSIGN:ADDASSIGN, e1, e2);
  } else {
    Exp *e1 = new Exp(pcval, id1name, INT, LO, val);
    e = new Exp(pcval, PREUNARY, INT, (incnum<0)?DEC:INC, e1);
  }

  if ((lastaction == 4)&&(!strcmp((*(stkptr-1))->e->id->name,id1name))) {
    e->e->et = POSTUNARY; e->minpc = min(e->minpc, (*(stkptr-1))->minpc);
    *(stkptr-1) = e;
  } else {
    *donestkptr++ = e;
  }
  return 0;
}
