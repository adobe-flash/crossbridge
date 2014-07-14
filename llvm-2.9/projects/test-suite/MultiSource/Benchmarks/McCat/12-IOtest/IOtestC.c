
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

#include <stdio.h>
#include "IOtest.h"

/* slow version */

static void initminB(char *res)
{
  setac(0);
  *res=255;
}

static void initmaxB(char *res)
{
  setac(0);
  *res=0;
}

static void initaddB(char *res)
{
  setac(0);
  *res=0;
}

static void initmultB(char *res)
{
  setac(0);
  *res=1;
}


static void stepminB(char *res)
/* {{{  */

{
  register char t;
  unsigned long i;
  t=array(getac());
  *res=min(*res,t);
  i=getac()+4097;
  setac(i);
}

/* }}} */

static void stepmaxB(char *res)
/* {{{  */
{
  register char t;
  unsigned long i;
  t=array(getac());
  *res=max(*res,t);
  i=getac()+4097 ;
  setac(i);
}
/* }}} */

static void stepaddB(char *res)
/* {{{  */
{
  register char t;
  unsigned long i;
  t=array(getac());
  *res=add(*res,t);
  i=getac()+4097;
  setac(i);
}
/* }}} */

static void stepmultB(char *res)
/* {{{  */

{
  register char t;
  unsigned long i;
  t=array(getac());
  *res=mult(*res,t);
  i=getac()+4097;
  setac(i);
}

/* }}} */


void testC()
{
  struct global_result res;

  initarray();

  loop((void (*)(void *))&initminB,(void (*)(void *))&stepminB,&(res.min));

  loop((void (*)(void *))&initmaxB,(void (*)(void *))&stepmaxB,&(res.max));

  loop((void (*)(void *))&initaddB,(void (*)(void *))&stepaddB,&(res.add));

  loop((void (*)(void *))&initmultB,(void (*)(void *))&stepmultB,&(res.mult));

  printf("C %d min %d max %d add %d mult \n",res.min,res.max,res.add,res.mult);

}
