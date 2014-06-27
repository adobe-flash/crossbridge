
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

/* fast version */

void initA(struct global_result *res)
{
  setac(0);
  res->min=255;
  res->max=0;
  res->add=0;
  res->mult=1;
}

void stepA(struct global_result *res)
{
  register char t;
  unsigned long i;
  t=array(getac());
  res->min=min(res->min,t);
  res->max=max(res->max,t);
  res->add=add(res->add,t);
  res->mult=mult(res->mult,t);
  i=getac();
  i++;
  setac(i);
}

void testA()
{
  struct global_result res;

  initarray();

  loop((void (*)(void *))&initA,(void (*)(void *))&stepA,&res);

  printf("A %d min %d max %d add %d mult \n",res.min,res.max,res.add,res.mult);

}









