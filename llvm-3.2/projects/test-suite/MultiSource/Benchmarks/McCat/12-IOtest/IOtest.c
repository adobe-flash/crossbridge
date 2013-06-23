
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

#include "IOtest.h"
#include <stdlib.h>

#define IOTEST_SIZE (53681*50) /* prime number */ /* 53681 19170419 67108933 */

char *testarray;
unsigned long array_count;

unsigned long getac(void)
/* {{{  */

{
  return array_count;
}

/* }}} */

void setac(unsigned long i)
{
  array_count=(i % IOTEST_SIZE);
}

void initarray(void)
{
  unsigned long i;
  i=0; 
  while(i<IOTEST_SIZE)
    {
      testarray[i]= i % 255;
      i++;
    }
}

char array(unsigned long i)
{
  return testarray[i];
}

char min(char a, char b)
{
  return (a>b)? a : b ;
}

char max(char a, char b)
{
  return (a>b) ? b : a ;
}

char add(char a, char b)
{
  return a+b;
}

char mult(char a, char b)
{
  return a*b;
}

void loop( void (*init)(void *) , void (*step)(void *) ,void *result) 
{
  unsigned long i;
  i=0;
  init(result);
  
  while(i<IOTEST_SIZE)
    {
      step(result);
      i++;
    }
}

extern void testA(void), testB(void), testC(void);
int main() {
	testarray = (char*)malloc(IOTEST_SIZE);
	testA(); testB(); testC();
	return 0;
}
   
