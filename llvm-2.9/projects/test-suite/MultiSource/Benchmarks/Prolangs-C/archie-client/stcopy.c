/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * v1.2.1 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 * v1.2.0 - 09/17/91 (bpk) - added BULL & USG stuff, thanks to Jim Sillas
 */

#include <copyright.h>
#include <stdio.h>
#include <stdlib.h>
#include <pmachine.h>

#ifdef NEED_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if defined(MSDOS)
# include <stdlib.h>
#endif

char	*stcopyr();

int	string_count = 0;
int	string_max = 0;

/*
 * stcopy - allocate space for and copy a string
 *
 *     STCOPY takes a string as an argument, allocates space for
 *     a copy of the string, copies the string to the allocated space,
 *     and returns a pointer to the copy.
 */

char *stcopy(char *st)
    {
      if (!st) return(NULL);
      if (string_max < ++string_count) string_max = string_count;

      return strcpy((char *)malloc(strlen(st) + 1), st);
    }

/*
 * stcopyr - copy a string allocating space if necessary
 *
 *     STCOPYR takes a string, S, as an argument, and a pointer to a second
 *     string, R, which is to be replaced by S.  If R is long enough to
 *     hold S, S is copied.  Otherwise, new space is allocated, and R is
 *     freed.  S is then copied to the newly allocated space.  If S is
 *     NULL, then R is freed and NULL is returned.
 *
 *     In any event, STCOPYR returns a pointer to the new copy of S,
 *     or a NULL pointer.
 */

char *stcopyr(char *s,char *r)
    {
	int	sl;

	if(!s && r) {
	    free(r);
	    string_count--;
	    return(NULL);
	}
	else if (!s) return(NULL);

	sl = strlen(s) + 1;

	if(r) {
	    if ((strlen(r) + 1) < sl) {
		free(r);
		r = (char *) malloc(sl);
	    }
	}
	else {
	    r = (char *) malloc(sl);
	    string_count++;
	    if(string_max < string_count) string_max = string_count;
	}
	    
	return strcpy(r,s);
    }

/*
 * stfree - free space allocated by stcopy or stalloc
 *
 *     STFREE takes a string that was returned by stcopy or stalloc 
 *     and frees the space that was allocated for the string.
 */

void stfree(char *st)
    {
	if(st) {
	    free(st);
	    string_count--;
	}
    }


