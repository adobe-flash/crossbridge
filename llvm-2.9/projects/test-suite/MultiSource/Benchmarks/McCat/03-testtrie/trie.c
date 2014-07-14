
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
#include "trie.h"
#include "charsequence.h"

/* Defines: */

#define DEBUG
#define TRUE   1
#define FALSE  0
#define TRIE_SIZE (sizeof(struct trie_s))
#define ERROR(s)  (fprintf(stderr,s))

#define index2char(i) (i+'a')
/*
char index2char(int i)
{
  return (i+'a');
}
*/

#define char2index(c) (tolower(c)-'a')
/*
int char2index(char c)
{
  return (tolower(c)-'a');
}
*/

/* Initialize an empty trie */
trie  trie_init(void)
{
  trie  t;

  if((t = calloc(1,TRIE_SIZE)) == NULL)
    {
      ERROR("Out of memory");
      exit(1);
    }

  return t;
}


/* Insert a string, s, in a trie, t, and return the updated trie */
trie  trie_insert(trie t, string s)
{
  if(t == NULL)
    t = trie_init();
  
  if(s[0] == '\0')			/* We've found the end */
    t->number++;
  else
    t->next[char2index(s[0])] = trie_insert(t->next[char2index(s[0])], s+1);

  return t;
}


/* Looks up a string, s, in a trie, t, and returns the number of occ. */
int   trie_lookup(trie t, string s)
{
  if(t == NULL)
    return 0;				/* Didn't find it */
  
  if(s[0] == '\0')			/* At the end */
    return (t->next[char2index(s[0])])->number;
  else
    return (trie_lookup(t->next[char2index(s[0])], s+1));
}


charsequence trie_scan_buffer=CHARSTREAM_INIT; /* reset,push,pop,val */

/* performs function f on all words in trie */
void trie_scan(trie t,void f(int,char*))
{
 char *str;
 int i;
  if (t!=NULL)
    {
      if (t->number!=0)
	{
	  str=charsequence_val(&trie_scan_buffer);
	  f(t->number,str );
	  free(str);
	}
      for(i=0;i<TRIEWIDTH;i++)
	if(t->next[i]!=NULL)
	  { 
	    charsequence_push(&trie_scan_buffer,index2char(i));
	    trie_scan(t->next[i],f);
	    charsequence_pop(&trie_scan_buffer);
	  }
    }
}




