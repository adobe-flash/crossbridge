
/*
**  Miscellaneous support functions for smail/rmail
*/

#ifndef lint
static char 	*sccsid="@(#)misc.c	2.5 (smail) 9/15/87";
#endif

# include	<stdio.h>
# include	<sys/types.h>
# include	<ctype.h>
# include	"defs.h"
#ifdef BSD
# include	<sys/time.h>
# include	<sys/timeb.h>
#else
# include	<time.h>
# include	<sys/utsname.h>
#endif

extern int  exitstat;		/* set if a forked mailer fails */
extern enum edebug debug;	/* how verbose we are 		*/ 
extern enum ehandle handle;	/* what we handle		*/
extern char *uuxargs;		/* arguments given to uux       */
extern int  queuecost;		/* threshold for queueing mail  */
extern int  maxnoqueue;		/* max number of uucico's       */
extern enum erouting routing;	/* when to route addresses	*/
extern char hostdomain[];	/* */
extern char hostname[];		/* */
extern char hostuucp[];		/* */
extern char *pathfile;		/* location of path database	*/
extern char *spoolfile;		/* file name of spooled message */
extern FILE *spoolfp;		/* file ptr  to spooled message */
extern int spoolmaster;		/* set if creator of spoolfile  */

extern struct tm *localtime();

struct tm *gmt, *loc;		/* GMT and local time structure	*/
time_t now;			/* current system time		*/
char nows[50];			/* time in ctime format		*/
char arpanows[50];		/* time in arpa format		*/

# ifdef LOG
void log(char *command,char *from,long size)
{
	FILE *fd;
	char *logtime, tbuf[50];
	int cmask;

	logtime = strcpy(tbuf, nows);
	logtime[16] = '\0';
	logtime += 4;

	cmask = umask(0);
	fd = fopen(LOG, "a");
	(void) umask(cmask);

	if (fd != NULL) {
		(void) fprintf(fd, "%s\t%ld\t%s\t%s\n",
			logtime, size, from, command);
		(void) fclose(fd);
	}
}
# endif

# ifdef RECORD
FILE *record(char *command,char *from,long size)
{
	FILE *fd;
	char *logtime, buf[SMLBUF];
	int cmask;

	logtime = strcpy(buf, nows);
	logtime[16] = 0;
	logtime += 4;

	cmask = umask(0);
	fd = fopen(RECORD, "a");
	(void) umask(cmask);

	if (fd != NULL) {
		(void) fprintf(fd, "%s: %s, from %s, %ld bytes\n", 
			logtime, command, from, size);
	}
	while(fgets(buf, sizeof(buf), spoolfp) != NULL) {
		(void) fputs(buf, fd);
	}
	(void) fclose(fd);
}
# endif


void setdates(void)
{
	time_t time();
	struct tm *gmtime();
	char *ctime(), *arpadate();

	(void) time(&now);
	(void) strcpy(nows, ctime(&now));
	gmt = gmtime(&now);
	loc = localtime(&now);
	(void) strcpy(arpanows, arpadate(nows));
}

/*
**  Note: This routine was taken from sendmail
**
**  ARPADATE -- Create date in ARPANET format
**
**	Parameters:
**		ud -- unix style date string.  if NULL, one is created.
**
**	Returns:
**		pointer to an ARPANET date field
**
**	Side Effects:
**		none
**
**	WARNING:
**		date is stored in a local buffer -- subsequent
**		calls will overwrite.
**
**	Bugs:
**		Timezone is computed from local time, rather than
**		from whereever (and whenever) the message was sent.
**		To do better is very hard.
**
**		Some sites are now inserting the timezone into the
**		local date.  This routine should figure out what
**		the format is and work appropriately.
*/

char *arpadate(register char *ud)
{
	register char *p;
	register char *q;
	static char b[40];
	extern char *ctime();
	register int i;
#ifndef BSD
	extern char *tzname[];
	time_t t, time();
#else
	/* V7 and 4BSD */
	struct timeb t;
	extern struct timeb *ftime();
	extern char *timezone();
#endif

	/*
	**  Get current time.
	**	This will be used if a null argument is passed and
	**	to resolve the timezone.
	*/

#ifndef BSD
	(void) time(&t);
	if (ud == NULL)
		ud = ctime(&t);
#else
	/* V7 or 4BSD */
	ftime(&t);
	if (ud == NULL)
		ud = ctime(&t.time);
#endif

	/*
	**  Crack the UNIX date line in a singularly unoriginal way.
	*/

	q = b;

	p = &ud[8];		/* 16 */
	if (*p == ' ')
		p++;
	else
		*q++ = *p++;
	*q++ = *p++;
	*q++ = ' ';

	p = &ud[4];		/* Sep */
	*q++ = *p++;
	*q++ = *p++;
	*q++ = *p++;
	*q++ = ' ';

	p = &ud[22];		/* 1979 */
	*q++ = *p++;
	*q++ = *p++;
	*q++ = ' ';

	p = &ud[11];		/* 01:03:52 */
	for (i = 8; i > 0; i--)
		*q++ = *p++;

				/* -PST or -PDT */
#ifndef BSD
	p = tzname[localtime(&t)->tm_isdst];
#else
	p = timezone(t.timezone, localtime(&t.time)->tm_isdst);
#endif
	if (p[3] != '\0')
	{
		/* hours from GMT */
		p += 3;
		*q++ = *p++;
		if (p[1] == ':')
			*q++ = '0';
		else
			*q++ = *p++;
		*q++ = *p++;
		p++;		/* skip ``:'' */
		*q++ = *p++;
		*q++ = *p++;
	}
	else
	{
		*q++ = ' ';
		*q++ = *p++;
		*q++ = *p++;
		*q++ = *p++;
	}

	p = &ud[0];		/* Mon */
	*q++ = ' ';
	*q++ = '(';
	*q++ = *p++;
	*q++ = *p++;
	*q++ = *p++;
	*q++ = ')';

	*q = '\0';
	return (b);
}

/*
 *	The user name "postmaster" must be accepted regardless of what
 *	combination of upper and lower case is used.  This function is
 *	used to convert all case variants of "postmaster" to all lower
 *	case.  If the user name passed in is not "postmaster", it is
 *	returned unchanged.
 */
char *postmaster(char *user)
{
	static char *pm = "postmaster";

	if(strcmpic(user, pm) == 0) {
		return(pm);
	} else {
		return(user);
	}
}

/*
 * Return 1 iff the string is "UUCP" (ignore case).
 */
int isuucp(char *str)
{
	if(strcmpic(str, "UUCP") == 0) {
		return(1);
	} else {
		return(0);
	}
}

/*
** sform(form) returns a pointer to a string that tells what 'form' means
*/

char *sform(enum eform form)
{
	if(form == ERROR)  return("ERROR");
	if(form == LOCAL)  return("LOCAL");
	if(form == DOMAIN) return("DOMAIN");
	if(form == UUCP)   return("UUCP");
	if(form == ROUTE)  return("ROUTE");
	return("UNKNOWN");
}

/*
**
**  getmynames(): what is my host name and host domain?
**
**  Hostname set by -h, failing that by #define HOSTNAME, failing
**  that by gethostname() or uname().
**  
**  Hostdomain set by -h, failing that by #define HOSTDOMAIN,
**  failing that as hostname.MYDOM, or as just hostname.
**
**  See defs.h for the inside story.
**
*/

void getmynames(void)
{
#ifdef HOSTNAME
	if (!*hostname)
		(void) strcpy(hostname, HOSTNAME);
#endif
#ifdef GETHOSTNAME
	if (!*hostname)
		gethostname(hostname, SMLBUF - 1);
#endif
#ifdef UNAME
	if (!*hostname) {
		struct utsname site;

		if (uname(&site) < 0)
			error(EX_SOFTWARE, "uname() call failed", 0);
		(void) strcpy(hostname, site.nodename);
	}
#endif
	if (!*hostname)
		error(EX_SOFTWARE, "can't determine hostname.\n", 0);
#ifdef HOSTDOMAIN
	if (!*hostdomain)
		(void) strcpy(hostdomain, HOSTDOMAIN);
#endif
#ifdef MYDOM
	if (!*hostdomain)
		(void) strcat(strcpy(hostdomain, hostname), MYDOM);
#endif
	if (!*hostdomain)
		(void) strcpy(hostdomain, hostname);

	(void) strcat(strcpy(hostuucp, hostname), ".UUCP");
}
