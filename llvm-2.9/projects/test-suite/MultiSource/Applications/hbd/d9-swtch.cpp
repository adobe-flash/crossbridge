/* d9-swtch.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include "exp.h"
#include "class.h"
#include "decomp.h"
#include "cp.h"

int dotableswitch(Classfile *c)
{
  unsigned pcval = currpc - 1;
  while(currpc%4) JDNEXT8U();
  Exp *e1 = *(--stkptr);
  unsigned defaultpc = JDNEXT32S();
  unsigned low = JDNEXT32S(), high = JDNEXT32S(), numcases = high - low + 1;
  Case *tcase = new Case[numcases];
  *donestkptr++ = new Exp(pcval, e1->minpc, SWITCH, VOID, ID, e1, defaultpc, numcases, tcase);
  for (unsigned m = low; m <= high;) {
     tcase->caseval = m++;
     (tcase++)->branch_pc = JDNEXT32S();
   }
  return 0;
}

int doluswitch(Classfile *c)
{
  unsigned pcval = currpc - 1;
  while(currpc%4) JDNEXT8U();
  Exp *e1 = *(--stkptr);
  unsigned defaultpc = JDNEXT32U(), numcases = JDNEXT32U();
  Case *tcase = new Case[numcases];
  *donestkptr++ = new Exp(pcval, e1->minpc, SWITCH, VOID, ID, e1, defaultpc, numcases, tcase);
  for (unsigned m = numcases; m--;) {
    tcase->caseval = JDNEXT32U();
    (tcase++)->branch_pc = JDNEXT32U();
  }
  return 0;
}
