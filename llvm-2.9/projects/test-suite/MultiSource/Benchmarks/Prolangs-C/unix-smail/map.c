#ifndef lint
static char 	*sccsid="@(#)map.c	2.5 (smail) 9/15/87";
#endif

# include	<stdio.h>
# include	<sys/types.h>
# include	"defs.h"

extern int queuecost;

/*
**
**  map(): map addresses into <host, user, form, cost> sets.
**
**  Calls resolve() for each address of argv.  The result is hostv and 
**  userv arrays (pointing into buffers userz and hostz), and formv array.
**
*/

void map(int argc,char **argv,char *hostv[],char *userv[],enum eform formv[],
         int costv[])
{
	int i, cost;
	enum eform resolve();
	char *c;
	static char userbuf[BIGBUF], *userz;
	static char hostbuf[BIGBUF], *hostz;

	userz = userbuf;
	hostz = hostbuf;

	for( i=0; i<argc; i++ ) {
#ifdef DEFQUEUE
		cost = queuecost+1;		/* default is queueing */
#else
		cost = queuecost-1;		/* default is no queueing */
#endif
		userv[i] = userz;		/* put results here */
		hostv[i] = hostz;
		if ( **argv == '(' ) {		/* strip () */
			++*argv;
			c = index( *argv, ')' );
			if (c)
				*c = '\0';
		}
						/* here it comes! */
		formv[i] = resolve(*argv++, hostz, userz, &cost);
		costv[i] = cost;
		userz += strlen( userz ) + 1;	/* skip past \0 */
		hostz += strlen( hostz ) + 1;
	}
}
