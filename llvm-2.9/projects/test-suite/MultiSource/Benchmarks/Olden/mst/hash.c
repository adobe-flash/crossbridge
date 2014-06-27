/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdlib.h>
#include "hash.h"

#define assert(num,a) if (!(a)) {printf("Assertion failure:%d in hash\n",num); exit(-1);}

static int remaining = 0;
static char *temp;

static char *localmalloc(int size) 
{
  char *blah;
  
  if (size>remaining) 
    {
      temp = (char *) malloc(32768);
      if (!temp) printf("Error! malloc returns null\n");
      remaining = 32768;
    }
  blah = temp;
  temp += size;
  remaining -= size;
  return blah;
}

#define localfree(sz)

Hash MakeHash(int size, int (*map)(unsigned int)) 
{
  Hash retval;
  int i;

  retval = (Hash) localmalloc(sizeof(*retval));
  retval->array = (HashEntry *) localmalloc(size*sizeof(HashEntry));
  for (i=0; i<size; i++)
    retval->array[i]=NULL;
  retval->mapfunc = map;
  retval->size = size;
  return retval;
}

void *HashLookup(unsigned int key, Hash hash)
{
  int j;
  HashEntry ent;

  j = (hash->mapfunc)(key);        /* 14% miss in hash->mapfunc */  
  assert(1,j>=0);
  assert(2,j<hash->size);
  for (ent = hash->array[j];       /* 17% miss in hash->array[j] */ /* adt_pf can't detect :( */
       ent &&                      /* 47% miss in ent->key */       /* adt_pf can detect :) */
           ent->key!=key; 
       ent=ent->next);             /* 8% miss in ent->next */       /* adt_pf can detect :) */
  if (ent) return ent->entry;
  return NULL;
}

void HashInsert(void *entry,unsigned int key,Hash hash) 
{
  HashEntry ent;
  int j;
  
  assert(3,!HashLookup(key,hash));
  
  j = (hash->mapfunc)(key);
  ent = (HashEntry) localmalloc(sizeof(*ent));
  ent->next = hash->array[j];
  hash->array[j]=ent;
  ent->key = key;
  ent->entry = entry;
}

void HashDelete(unsigned key, Hash hash) {
  HashEntry tmp;
  int j = (hash->mapfunc)(key);
  HashEntry *ent = &hash->array[j];

  while (*ent && (*ent)->key != key) {
    ent = &(*ent)->next;
  }

  assert(4, *ent);

  tmp = *ent;
  *ent = (*ent)->next;
  localfree(tmp);
}

  
  


