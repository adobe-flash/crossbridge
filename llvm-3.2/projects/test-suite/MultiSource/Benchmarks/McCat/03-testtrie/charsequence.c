
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
#include <string.h>
#include "charsequence.h"

/* Defines: */

#define DEBUG
#define TRUE   1
#define FALSE  0
#define TRIE_SIZE (sizeof(struct trie_s))
#define ERROR(s)  (fprintf(stderr,s))

void  charsequence_reset(charsequence *cs)
{
  if(cs->buf!=NULL)free(cs->buf);
  cs->buf=malloc(CHARSTREAM_STARTSIZE);
  if(cs->buf==NULL)ERROR("OUT OF MEMORY");
  cs->size=CHARSTREAM_STARTSIZE;
  cs->pos=0;
}

void  charsequence_push(charsequence *cs,char c)
{
  if(cs->size==0)
    {
      charsequence_reset(cs);
    }
  if(cs->pos==cs->size)
    {
      cs->size<<=1;
      cs->buf=realloc(cs->buf,cs->size);
      if(cs->buf==NULL)ERROR("OUT OF MEMORY");
    }
  cs->buf[cs->pos]=c;
  cs->pos++;
}
  
char charsequence_pop(charsequence *cs)
{
  if(cs->pos==0)
    {
      ERROR("ATTEMPTED POP ON EMPTY SEQUENCE");
    }
  return cs->buf[(cs->pos)--];
}

char *charsequence_val(charsequence *cs)
{
  char *ret;
  ret=calloc(cs->pos+1,sizeof(char));
  if(ret==NULL)ERROR("OUT OF MEMORY");/* check for allocation error */
  strncpy(ret,cs->buf,cs->pos);
  return ret;
}



