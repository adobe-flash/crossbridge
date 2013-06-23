/* Symbol table manager for Bison,
   Copyright (C) 1984 Bob Corbett and Free Software Foundation, Inc.

BISON is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the BISON General Public License for full details.

Everyone is granted permission to copy, modify and redistribute BISON,
but only under the conditions described in the BISON General Public
License.  A copy of this license is supposed to have been given to you
along with BISON so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

#include <stdio.h>
#ifdef USG
#include <string.h>
#else /* NOT USG */
#include <strings.h>
#endif /* NOT USG */

#include "new.h"
#include "symtab.h"
#include "gram.h"


bucket **symtab;
bucket *firstsymbol;
bucket *lastsymbol;



int hash(char *key)
{
  register char *cp;
  register int k;

  cp = key;
  k = 0;
  while (*cp)
    k = ((k << 1) ^ (*cp++)) & 0x3fff;

  return (k % TABSIZE);
}



char *copys(char *s)
{
  register int i;
  register char *cp;
  register char *result;

  i = 1;
  for (cp = s; *cp; cp++)
    i++;

  result =  mallocate((unsigned int)i);
  strcpy(result, s);
  return (result);
}

void tabinit(void)
{
/*   register int i; JF unused */

  symtab = NEW2(TABSIZE, bucket *);

  firstsymbol = NULL;
  lastsymbol = NULL;
}

bucket *getsym(char *key)
{
  register int hashval;
  register bucket *bp;
  register int found;

  hashval = hash(key);
  bp = symtab[hashval];

  found = 0;
  while (bp != NULL && found == 0)
    {
      if (strcmp(key, bp->tag) == 0)
	found = 1;
      else
	bp = bp->link;
    }

  if (found == 0)
    {
      nsyms++;

      bp = NEW(bucket);
      bp->link = symtab[hashval];
      bp->next = NULL;
      bp->tag = copys(key);
      bp->class = SUNKNOWN;

      if (firstsymbol == NULL)
	{
	  firstsymbol = bp;
	  lastsymbol = bp;
	}
      else
	{
	  lastsymbol->next = bp;
	  lastsymbol = bp;
	}

      symtab[hashval] = bp;
    }

  return (bp);
}

void free_symtab(void)
{
  register int i;
  register bucket *bp,*bptmp;/* JF don't use ptr after free */

  for (i = 0; i < TABSIZE; i++)
    {
      bp = symtab[i];
      while (bp)
	{
	  bptmp=bp->link;
	  FREE(bp);
	  bp = bptmp;
	}
    }
}
