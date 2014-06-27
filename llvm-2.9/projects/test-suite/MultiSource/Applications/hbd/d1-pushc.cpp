/* d1-pushc.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <stdio.h>
#include "exp.h"
#include "class.h"
#include "decomp.h"
#include "cp.h"
#include "consts.h"

int pushimm(Classfile *c) /* push immediate value e.g. bipush 34 */
{
  int pcval = currpc - 1;
  int val = JDNEXT8S();
  if (ch == 0x11) { val<<=8; val+=JDNEXT8U(); }
  char *temp_str = new char[32];
  sprintf(temp_str, "%i", val);
  *stkptr++ = new Exp(pcval, temp_str, INT, IM); /* id->linfo = val */
  return 0;
}

int pushconst(Classfile *c) /* push value from cp e.g. ldc1 #3 */
{
  int pcval = currpc - 1;
  int val = JDNEXT8U();
  if (ch != 0x12) { val<<=8; val+=JDNEXT8U(); }
  Type idtype;
  char tmpstr[1024];
  cp_info *cpi = c->cp[val];
  switch (cpi->tag) {
    case CONSTANT_Integer:
      sprintf(tmpstr, "0x%lX", cpi->i);
      idtype = INT;
//      id->linfo = cpi->i;
      break;
    case CONSTANT_Long:
      if (cpi->i)
	sprintf(tmpstr, "0x%lX%08lXL", cpi->i, (cpi + 1)->i);
      else
	sprintf(tmpstr, "0x%lXL", (cpi + 1)->i);
      idtype = LONG;
//      id->linfo = (cpi + 1)->i;
//      id->llinfo[2] = cpi->i;
      break;
    case CONSTANT_Float:
      sprintf(tmpstr, "%.25Gf", cpi->f);
      idtype = FLOAT;
//      id->dinfo = cpi->f;
      break;
    case CONSTANT_Double:
      sprintf(tmpstr, "%.25Gd", *(double *)&cpi->i);
      idtype = DOUBLE;
//      id->dinfo = *(double*)&cpi->i;
      break;
    case CONSTANT_String:
      sprintf(tmpstr, "\"%s\"", c->cp[cpi->i]->chp);
      idtype = OBJECT; /* java.lang.String */
//      id->dinfo = (int)c->cp[cpi->i]->chp;
      break;
    default:
      fprintf(stderr, "Unkown tag %d on constant\n", cpi->tag);
      return -1;
  }
  char *idname = new char[strlen(tmpstr) + 1];
  strcpy(idname, tmpstr);
  *stkptr++ = new Exp(pcval, idname, idtype, CP, val);
  return 0;
}

int pushimp(Classfile *c) /* push implied immediate value e.g. iconst_m1 */
{
  *stkptr++ = new Exp(currpc - 1, ch - 1);
  return 0;
}

