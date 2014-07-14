
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

/* Implementation of the datastructure Trie
   ) September 1996
     Lars Hjorth Nielsen and Jacob Winstrup Schmidt */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define True -1
#define False 0 
#define or    ||
#define and   &&
#define MAXSIZE   500
#define LINESIZE 1000

typedef struct trie *Trie;
typedef struct TrieRoot * TrieRoot;
typedef struct SString * SString;

typedef int bool;
typedef char * String;

void insertWord(TrieRoot tr,String w);
void insertW(Trie tr,String w);
Trie insertRestOfWord(String w);
Trie deleteWord(Trie tr,String w);
bool deleteW(Trie tr,String w);
void deleteNode(Trie tr);
Trie tNew(char c);
Trie tAlloc(void);
void insertChar(SString word, char c);
void deleteChar(SString word);
void printT(TrieRoot tr);
void printTheRest(Trie t, SString word);
SString ssInit(void);
TrieRoot trInit(void);


struct trie
{
  char val;
  bool word;
  void * data;
  Trie postfix;
  Trie otherChar;
};

struct TrieRoot
{
  Trie t;
};

struct SString
{
  String s;
  int max;
  int len;
};


Trie tAlloc(void)
{
  Trie t;

  t = (Trie) calloc(1, sizeof(struct trie));
  if (t == NULL)
     {
       abort();
     }
  return t;
};

Trie tNew(char c)
{
  Trie t;
  t = tAlloc();
  t->val = c;
  t->word = False;
  t->data = (void *) NULL;
  t->postfix = t->otherChar = (Trie) NULL;
  return t;
};

/*Insert functions */
/*******************/

void insertWord(TrieRoot r,String w)
{
  if (*w == '\0')
    {
    return;
    }
  else
    {
      if (r->t == (Trie) NULL)
	{
	  r->t = insertRestOfWord(w);
	}
      else
	{
	  insertW(r->t, w);
	}
    }
}


void insertW(Trie tr,String w)
     /* Insert a word in a NON-EMPTY trie */
{
  if (tr == (Trie) NULL)
    {
      abort();
      /* error("Cannot insert in an empty Trie") */ ;
    }
  else
    {
      if (*w == '\0')
	/* Should not happen but does not harm. */
	{
	  return;
	}
      else
	{
	  if (*w == tr->val)
	    {
	      if (*(w+1) == '\0')
		{
		  tr->word = True;
		  return;
		}
	      else
		{
		  if (tr->postfix == (Trie) NULL)
		    {
		      tr->postfix = insertRestOfWord(++w);
		      return;
		    }
		  else
		    {
		      insertW(tr->postfix, ++w);
		      return;
		    }
		}
	    }
	  else
	    {
	      if (tr->otherChar == (Trie) NULL)
		{
		  tr->otherChar = insertRestOfWord(w);
		  return;
		}
	      else
		{
		  insertW(tr->otherChar, w);
		  return;
		}
	    }
	}
    }
}

Trie insertRestOfWord(String w)
     /* Insert a NON-EMPTY word in a new trie */
{
  Trie t;
  if (*w == '\0')
    /* error("Cannot insert the empty string. \0") */ ;
  else
    {
      t = tNew(*w);
      w++;
      if (*w == '\0')
	{
	  t->word = True;
	  return t;
	}
      else
	{
	  t->postfix = insertRestOfWord(w);
	  return t;
	}
    }
}

/*End of insert functions */

void insertChar(SString word, char c)
{
  if (word == NULL) 
    {
      abort();
    }
  else
    {
      if (word->s == NULL)
	 {
	   String sTemp;
	   sTemp = (String) calloc(1, sizeof(char)+1);
	   if (sTemp == NULL) abort();
	   else
	     {
	       word->s = sTemp;
	       word->max = 1;      /* don't count \0 */
	       word->len = 0;
	     }
	 }
      else
	{
	  if (word->len == word->max)
	    {
	      String tmp;
	      tmp = (String) calloc(1, 2 * word->max * sizeof(char) + 1);
	      if (tmp == NULL) abort();
	      else
		{
		  tmp = strcpy(tmp, word->s);
		  free(word->s);
		  word->s = tmp;
		  word->max *= 2;
		}
	    }
	}
      word->s[word->len++] = c;
      word->s[word->len] = '\0';
    }
}

void deleteChar(SString word)
     /* word must be NON-NULL */
{
  if (word == NULL) abort();
  else
    {
      if (word->s == NULL) abort();
      else
	{
	  if (word->len > 0)
	    {
	      word->s[--word->len] = '\0';
	    }
	  else /* OK */ ;
	}
    }
}

/*Print function */
/****************/

void printT(TrieRoot tr)
{
  SString word = ssInit();
  if (tr == NULL)
    abort();
  else
    {
      printTheRest(tr->t, word);
    }
}

void printTheRest(Trie t, SString word)
{
  if (t == NULL)
    {
      return;
    }
  else
    {
      insertChar(word, t->val);
      if (t->word == True)
	{
	  printf("%s\n", word->s);
	}
      else
	;
      printTheRest(t->postfix, word);
      deleteChar(word);
      printTheRest(t->otherChar, word);
    }
}

SString ssInit(void)
{
  SString word = (SString) calloc(1, sizeof(struct SString));
  if (word == NULL)
    {
      abort();
    }
  else
    {
      word->s = (String) NULL;
      word->max = 0;
      word->len = 0;
      return word;
    }
}

TrieRoot trInit(void)
{
  TrieRoot tr;

  tr = (TrieRoot) calloc(1, sizeof(struct TrieRoot));
  tr->t = (Trie) NULL;
  return tr;
}

int main(int argc, char **argv)
{
  FILE * file;
  char word[LINESIZE];
  char buffer[MAXSIZE];
  int pos = 0;
  TrieRoot tr;

  tr = trInit();
  if(argc != 2)
    {
    fprintf(stdout, "Usage: %s <sourcefile>\n",argv[0]);
    return 1;
    }
  if(!(file = fopen(argv[1], "r")))
    {
      perror(argv[1]);
      return 1;
    }
  while(!feof(file))
    {
      if (fscanf(file,"%s",word) < 0)
	{
	  break;
	}
      else 
	{
	  pos = strlen(word) + pos + 1;
	  fseek(file, pos, 0);
	  insertWord(tr, word);
	}
    }
  fclose(file);
  printT(tr);
  return 0;
};
