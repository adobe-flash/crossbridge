/* decomp.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef DECOMP_H
#define DECOMP_H

#include "general.h"

/* These all need to be global for various reasons */
extern int ch;
extern char actions[];
extern int (*actiontable[])(Classfile *c);
extern unsigned char *inbuff;
extern int bufflength;
extern unsigned currpc;

/* Various macros to read the bytecodes and keep track of
   where we are in the code */
#define JDNEXT8S() (i8)(currpc++,bufflength--,*inbuff++)
#define JDNEXT8U() (u8)(currpc++,bufflength--,*inbuff++)
#define JDNEXT16S() (i16)(currpc+=2,bufflength-=2,inbuff+=2, \
                   (((u16)*(inbuff-2))<<8)+((u16)*(inbuff-1)))
#define JDNEXT16U() (u16)(currpc+=2,bufflength-=2,inbuff+=2, \
                   (((u16)*(inbuff-2))<<8)+((u16)*(inbuff-1)))
#define JDNEXT32S() (i32)(currpc+=4,bufflength-=4,inbuff+=4, \
           (((u32)*(inbuff-4))<<24)+(((u32)*(inbuff-3))<<16) \
           +(((u32)*(inbuff-2))<<8)+((u32)*(inbuff-1)))
#define JDNEXT32U() (u32)(currpc+=4,bufflength-=4,inbuff+=4, \
           (((u32)*(inbuff-4))<<24)+(((u32)*(inbuff-3))<<16) \
           +(((u32)*(inbuff-2))<<8)+((u32)*(inbuff-1)))
//#define JDLAST(num) (*(inbuff-(num)-1))
#define JDLAST8S() (i8)(*(inbuff-1))
#define JDLAST8U() (u8)(*(inbuff-1))
#define JDPEEK8S() (i8)(*inbuff)
#define JDPEEK8U() (u8)(*inbuff)

/* These are all the prototypes for the actions that are used
   by the decompiler */
int pushimp(Classfile *c);
int pushimm(Classfile *c);
int pushconst(Classfile *c);
int pushlocal(Classfile *c);
int storelocal(Classfile *c);
int pushbinop(Classfile *c);
int pushunop(Classfile *c);
int finishconditional(Classfile *c);
int doget(Classfile *c);
int doput(Classfile *c);
int doreturn(Classfile *c);
int invokefunc(Classfile *c);
int doif1(Classfile *c);
int dodup(Classfile *c);
int doarraylength(Classfile *c);
int doarrayget(Classfile *c);
int iinclocal(Classfile *c);
int docheckcast(Classfile *c);
int doarrayput(Classfile *c);
int anewarray(Classfile *c);
int multianewarray(Classfile *c);
int dopop(Classfile *c);
int docmp(Classfile *c);
int doif2(Classfile *c);
int doluswitch(Classfile *c);
int dodup_x1(Classfile *c);
int doinstanceof(Classfile *c);
int dotableswitch(Classfile *c);

/* These are the globals which contain the
   various stacks used by the decompiler. */
extern Exp *stack[];
extern Exp **stkptr;
extern Exp *donestack[];
extern Exp **donestkptr;
extern int lastaction;
extern int cond_pcend;
extern Exp *cond_e1;
extern Exp *cond_e1;
extern Exp **cond_donestkptr;

#endif
