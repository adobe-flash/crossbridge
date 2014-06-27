#ifndef lint
static char 	*sccsid="@(#)str.c	2.5 (smail) 9/15/87";
#endif

#include "defs.h"
#include <ctype.h>

/*
**	strncmpic: string compare, ignore case, stop after 'n' chars
*/

int strncmpic(char *s1,char *s2,int n)
{
	register char *u = s1;
	register char *p = s2;

	while((n > 0) && (*p != '\0')) {
		/* chars match or only case different */
		if(lower(*u) == lower(*p)) {
			p++;	/* examine next char */
			u++;
		} else {
			break;	/* no match - stop comparison */
		}
		n--;
	}
	if(n > 0) {
		return(lower(*u) - lower(*p)); /* return "difference" */
	} else {
		return(0);
	}
}

/*
**	strcmpic: string compare, ignore case
*/

int strcmpic(char *s1,char *s2)
{
	register char *u = s1;
	register char *p = s2;

	while(*p != '\0') {
		/* chars match or only case different */
		if(lower(*u) == lower(*p)) {
			p++;	/* examine next char */
			u++;
		} else {
			break;	/* no match - stop comparison */
		}
	}

	return(lower(*u) - lower(*p)); /* return "difference" */
}
