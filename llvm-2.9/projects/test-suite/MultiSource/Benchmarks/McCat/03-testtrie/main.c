
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

/* dOPC - Assignment #1.
   Lasse R. Nielsen (920666) and Ren\'{e} R. Hansen (920319) */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "charsequence.h"
#include "trie.h"

#define FALSE 0
#define TRUE (!FALSE)

#define ISLOWER(c) (c >= 'a' && c <= 'z')

trie t;

void printit(int i, char *str)
{
  printf("%5d : '%s'\n",i,str);
}

void addfile(trie t, FILE *f)
{
  char c;
  int wstate=FALSE; /* NOT reading word now */
  charsequence cs=CHARSTREAM_INIT;

  while(!feof(f))
    {
      c=tolower(getc(f));
      if(wstate)
	{
	  if(ISLOWER(c))
	    charsequence_push(&cs,c);
	  else /* stop reading word */
	    {
	      char *str;
	      str=charsequence_val(&cs);
	      trie_insert(t,str);
	      free(str);
	      
	      wstate=FALSE;
	    }
	}
      else /* !wstate */
	if(ISLOWER(c)) /* start reading word */
	  {
	    charsequence_reset(&cs);
	    charsequence_push(&cs,c);
	    wstate=TRUE;
	  }
      /* else, ignore it */
    }
}



int main(int argc, char *argv[])
{
  FILE *input;

  t=trie_init();
  if (argc==1)
    addfile(t,stdin);
  else
    while(argc>1)
      {
	input=fopen(argv[1],"rb");
	if(input==NULL)
	  fprintf(stderr,"unable to open file '%s'\n",argv[1]);
	else
	  addfile(t,input);
	argc--;argv++;
      }
  trie_scan(t,printit);
  return 0; 
}
  
  
  
  
  
  
  
  

