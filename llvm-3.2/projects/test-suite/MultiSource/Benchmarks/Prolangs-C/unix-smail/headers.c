/*
**  message spooing, header and address parsing and completion
**  functions for smail/rmail
*/

#ifndef lint
static char 	*sccsid="@(#)headers.c	2.5 (smail) 9/15/87";
#endif

# include	<stdio.h>
# include	<sys/types.h>
# include	<time.h>
# include	<ctype.h>
# include	<pwd.h>
# include	"defs.h"

extern enum edebug debug;	/* how verbose we are 		*/ 
extern char hostname[];		/* */
extern char hostdomain[];	/* */
extern char *spoolfile;		/* file name of spooled message */
extern FILE *spoolfp;		/* file ptr  to spooled message */
extern int spoolmaster;		/* set if creator of spoolfile  */
extern time_t now;		/* time				*/
extern char nows[], arpanows[];	/* time strings			*/
extern struct tm *gmt, *loc;	/* time structs			*/
extern char *from_addr;		/* replacement fromaddr with -F */

static char toline[SMLBUF];
static char fromline[SMLBUF];
static char dateline[SMLBUF];
static char midline[SMLBUF];
static char *ieof = "NOTNULL";

struct reqheaders {
	char *name;
	char *field;
	char have;
};

static struct reqheaders reqtab[] = {
	"Message-Id:"	,	midline		,	'N'	,
	"Date:"		,	dateline	,	'N'	,
	"From:"		,	fromline	,	'N'	,
	"To:"		,	toline		,	'N'	,
	NULL 		,	NULL		,	'N'
};

int ssplit(register char *buf,char c,char **ptr);
void build(char *domain,char *user,enum eform form,char *result);
void haveheaders(char *s);
void def_headers(int argc,char **argv,char *from);
void def_date(void);
void def_mid(void);
void def_from(char *from);
void def_to(int argc,char **argv);

/*
**
** parse(): parse <address> into <domain, user, form>.
**
** 	input		form
**	-----		----
**	user		LOCAL
**	domain!user	DOMAIN
**	user@domain	DOMAIN
**	@domain,address	LOCAL	(just for sendmail)
**	host!address	UUCP
**
*/

enum eform parse(char *address,char *domain,char *user)
{
	int parts;
	char *partv[MAXPATH];				/* to crack address */

/*
**  If this is route address form @domain_a,@domain_b:user@domain_c, ...
*/
	if(*address == '@')
#ifdef SENDMAIL
/*
**  hand it to sendmail
*/
	{
		goto local;
	}
#else
/*
**  no sendmail, convert it into a bang path: domain_a!domain_b!domain_c!user
*/
	{
		char buf[SMLBUF], *p;
		char t_dom[SMLBUF], t_user[SMLBUF];

		(void) strcpy(buf, address+1);		/* elide leading '@' */

		for(p=buf; *p != '\0' ; p++) {	/* search for ',' or ':' */
			if(*p == ':') {		/* reached end of route */
				break;
			}
			if(*p == ',') {		/* elide ','s */
				(void) strcpy(p, p+1);
			}
			if(*p == '@') {		/* convert '@' to '!' */
				*p = '!';
			}
		}

		if(*p != ':') {	/* bad syntax - punt */
			goto local;
		}
		*p = '\0';

		if(parse(p+1, t_dom, t_user) != LOCAL) {
			(void) strcat(buf, "!");
			(void) strcat(buf, t_dom);
		}
		(void) strcat(buf, "!");
		(void) strcat(buf, t_user);

		/* munge the address (yuk)
		** it's OK to copy into 'address', because the machinations
		** above don't increase the string length of the address.
		*/

		(void) strcpy(address, buf);

		/* re-parse the address */
		return(parse(address, domain, user));
	}
#endif
/*
**  Try splitting at @.  If it works, this is user@domain, form DOMAIN.
**  Prefer the righthand @ in a@b@c.
*/
	if ((parts = ssplit(address, '@', partv)) >= 2) {
	        int tmp;
		(void) strcpy(domain, partv[parts-1]);
		(void) strncpy(user, partv[0], (tmp = (partv[parts-1]-partv[0]))-1);
		/* user[partv[parts-1]-partv[0]-1] = '\0'; */
		user[tmp-1] = '\0'; 
		return (DOMAIN);
	} 
/*
**  Try splitting at !. If it works, see if the piece before the ! has
**  a . in it (domain!user, form DOMAIN) or not (host!user, form UUCP).
*/
	if (ssplit(address, '!', partv) > 1) {
	        int tmp;
		(void) strcpy(user, partv[1]);
		(void) strncpy(domain, partv[0], (tmp = (partv[1]-partv[0]))-1);
		/* domain[partv[1]-partv[0]-1] = '\0'; */
		domain[tmp-1] = '\0'; 

		if((parts = ssplit(domain, '.', partv)) < 2) {
			return(UUCP);
		}

		if(partv[parts-1][0] == '\0') {
			partv[parts-1][-1] = '\0'; /* strip trailing . */
		}
		return (DOMAIN);
	}
/* 
**  Done trying.  This must be just a user name, form LOCAL.
*/
local:
	(void) strcpy(user, address);
	(void) strcpy(domain, "");
	return(LOCAL);				/* user */
}

void build(char *domain,char *user,enum eform form,char *result)
{
	switch((int) form) {
	case LOCAL:
		(void) sprintf(result, "%s", user); 
		break;
	case UUCP:
		(void) sprintf(result, "%s!%s", domain, user);
		break;
	case DOMAIN:
		(void) sprintf(result, "%s@%s", user, domain);
		break;
	}
}

/*
**  ssplit(): split a line into array pointers.
**
**  Each pointer wordv[i] points to the first character after the i'th 
**  occurence of c in buf.  Note that each wordv[i] includes wordv[i+1].
**
*/

int ssplit(register char *buf,char c,char **ptr)
{
        int count = 0;
        int wasword = 0;

        for(; *buf; buf++) {
		if (!wasword) {
			count++;
			*ptr++ = buf;
		}
		wasword = (c != *buf);
        }
	if (!wasword) {
		count++;
		*ptr++ = buf;
	}
        *ptr = NULL;
        return(count);
}

/*
** Determine whether an address is a local address
*/

int islocal(char *addr,char *domain,char *user)
{
		enum eform form, parse();
		extern char hostuucp[];

		/*
		** parse the address
		*/

		form = parse(addr, domain, user);

		if((form == LOCAL)			/* user */
		||(strcmpic(domain, hostdomain) == 0)	/* user@hostdomain */
		||(strcmpic(domain, hostname)   == 0)	/* user@hostname */
#ifdef DOMGATE
		||(strcmpic(domain, &MYDOM[0]) == 0)	/* user@MYDOM w/ dot */
		||(strcmpic(domain, &MYDOM[1]) == 0)	/* user@MYDOM no dot */
#endif
		||(strcmpic(domain, hostuucp)   == 0)) {/* user@hostuucp */
			return(1);
		}
		return(0);
}

/*
** spool - message spooling module
**
** (1) get dates for headers, etc.
** (2) if the message is on the standard input (no '-f')
**     (a) create a temp file for spooling the message.
**     (b) collapse the From_ headers into a path.
**     (c) if the mail originated locally, then
**	     (i) establish default headers
**	    (ii) scan the message headers for required header fields
**	   (iii) add any required message headers that are absent
**     (d) copy rest of the message to the spool file
**     (e) close the spool file
** (3) open the spool file for reading
*/

void spool(int argc,char **argv)
{
	static char *tmpf = "/tmp/rmXXXXXX";	/* temp file name */
	char *mktemp();
	char buf[SMLBUF];
	static char splbuf[SMLBUF];
	char from[SMLBUF], domain[SMLBUF], user[SMLBUF];
	void rline(), scanheaders(), compheaders();

	/*
	** if the mail has already been spooled by
	** a previous invocation of smail don't respool.
	** check the file name to prevent things like
	** rmail -f /etc/passwd badguy@dreadfuldomain
	*/

	if((spoolfile != NULL)
	&& (strncmp(spoolfile, tmpf, strlen(tmpf) - 6) != 0)) {
		error(EX_TEMPFAIL, "spool: bad file name '%s'\n", spoolfile);
	}

	/*
	** set dates in local, arpa, and gmt forms
	*/
	setdates();

	/*
	** If necessary, copy stdin to a temp file.
	*/

	if(spoolfile == NULL) {
		spoolfile = strcpy(splbuf, tmpf);
		(void) mktemp(spoolfile);

		if((spoolfp = fopen(spoolfile, "w")) == NULL) {
			error(EX_CANTCREAT, "can't create %s.\n", spoolfile);
		}

		spoolmaster = 1;

		/*
		** rline reads the standard input,
		** collapsing the From_ and >From_
		** lines into a single uucp path.
		** first non-from_ line is in buf[];
		*/

		rline(from, buf);

		/*
		** if the mail originated here, we parse the header
		** and add any required headers that are missing.
		*/

		if(islocal(from, domain, user) || (from_addr != NULL)) {
			/*
			** initialize default headers
			*/
			def_headers(argc, argv, from);

			/*
			** buf has first, non-from_  line
			*/
			scanheaders(buf);
			/*
			** buf has first, non-header line,
			*/

			compheaders();

			if(buf[0] != '\n') {
				(void) fputs("\n", spoolfp);
			}
		}

		/*
		** now, copy the rest of the letter into the spool file
		** terminate on either EOF or '^.$'
		*/

		while(ieof != NULL) {
			(void) fputs(buf, spoolfp);
			if((fgets(buf, SMLBUF, stdin) == NULL)
			|| (buf[0] == '.' && buf[1] == '\n')) {
				ieof = NULL;
			}
		}

		/*
		** close the spool file, and the standard input.
		*/

		(void) fclose(spoolfp);
		(void) fclose(stdin);	/* you don't see this too often! */
	}

	if((spoolfp = fopen(spoolfile, "r")) == NULL) {
		error(EX_TEMPFAIL, "can't open %s.\n", spoolfile);
	}
}

/*
**
**  rline(): collapse From_ and >From_ lines.
**
**  Same idea as the old rmail, but also turns user@domain to domain!user. 
**
*/

void rline(char *from,char *retbuf)
{
	int parts;			/* for cracking From_ lines ... */
	char *partv[16];		/* ... apart using ssplit() 	*/
	char user[SMLBUF];		/* for rewriting user@host	*/
	char domain[SMLBUF];		/* "   "         "          	*/
	char addr[SMLBUF];		/* "   "         "          	*/
	enum eform form, parse();	/* "   "         "          	*/
	char *c;
	int nhops, i;
	char buf[SMLBUF], tmp[SMLBUF], *hop[128], *e, *b;
	char *pwuid();

	if(spoolmaster == 0) return;

	buf[0] = from[0] = addr[0] = '\0';
/*
**  Read each line until we hit EOF or a line not beginning with "From "
**  or ">From " (called From_ lines), accumulating the new path in from
**  and stuffing the actual sending user (the user name on the last From_ 
**  line) in addr.
*/
	for(;;) {
		(void) strcpy(retbuf, buf);
		if(ieof == NULL) {
			break;
		}
		if((fgets(buf, sizeof(buf), stdin) == NULL)
		|| (buf[0] == '.' && buf[1] == '\n')) {
			ieof = NULL;
			break;
		}
		if (strncmp("From ", buf, 5) 
		    && strncmp(">From ", buf, 6)) {
			break;
		}
/*
**  Crack the line apart using ssplit.
*/
		if(c = index(buf, '\n')) {
			*c = '\0';
		}
		parts = ssplit(buf, ' ', partv);
/*
**  Tack host! onto the from argument if "remote from host" is present.
*/

		if((parts > 3)
		&& (strncmp("remote from ", partv[parts-3], 12) == 0)) {
			(void) strcat(from, partv[parts-1]);
			(void) strcat(from, "!");
		}
/*
**  Stuff user name into addr, overwriting the user name from previous 
**  From_ lines, since only the last one counts.  Then rewrite user@host 
**  into host!user, since @'s don't belong in the From_ argument.
*/
		if(parts < 2) {
			break;
		} else {
			char *x = partv[1];
			char *q = index(x, ' ');
			if(q != NULL) {
				*q = '\0';
			}
			(void) strcpy(addr, x);
		}

		(void) parse(addr, domain, user);
		if(*domain == '\0') {
			form = LOCAL;
		} else {
			form = UUCP;
		}

		build(domain, user, form, addr);
	}
/*
**  Now tack the user name onto the from argument.
*/
	(void) strcat(from, addr);
/*
**  If we still have no from argument, we have junk headers, but we try
**  to get the user's name using /etc/passwd.
*/

	if (from[0] == '\0') {
		char *login;
		if ((login = pwuid(getuid())) == NULL) {
			(void) strcpy(from, "nobody");	/* bad news */
		} else {
			(void) strcpy(from, login);
		}
	}

	/* split the from line on '!'s */
	nhops = ssplit(from, '!', hop);

	for(i = 0; i < (nhops - 1); i++) {
		b = hop[i];
		if(*b == '\0') {
			continue;
		}
		e = hop[i+1];
		e-- ;
		*e = '\0';	/* null terminate each path segment */
		e++;

#ifdef HIDDENHOSTS
/*
**  Strip hidden hosts:  anything.hostname.MYDOM -> hostname.MYDOM
*/
		for(p = b;(p = index(p, '.')) != NULL; p++) {
			if(strcmpic(hostdomain, p+1) == 0) {
				(void) strcpy(b, hostdomain);
				break;
			}
		}
#endif

/*
**  Strip useless MYDOM: hostname.MYDOM -> hostname
*/
		if(strcmpic(hop[i], hostdomain) == 0) {
			(void) strcpy(hop[i], hostname);
		}
	}

/*
**  Now strip out any redundant information in the From_ line
**  a!b!c!c!d	=> a!b!c!d
*/

	for(i = 0; i < (nhops - 2); i++) {
		b = hop[i];
		e = hop[i+1];
		if(strcmpic(b, e) == 0) {
			*b = '\0';
		}
	}
/*
**  Reconstruct the From_ line
*/
	tmp[0] = '\0';			/* empty the tmp buffer */

	for(i = 0; i < (nhops - 1); i++) {
		if((hop[i][0] == '\0')	/* deleted this hop */
		 ||((tmp[0] == '\0')	/* first hop == hostname */
		  &&(strcmpic(hop[i], hostname) == 0))) {
			continue;
		}
		(void) strcat(tmp, hop[i]);
		(void) strcat(tmp, "!");
	}
	(void) strcat(tmp, hop[i]);
	(void) strcpy(from, tmp);
	(void) strcpy(retbuf, buf);
	(void) fprintf(spoolfp, "%s\n", from);
}

void scanheaders(char *buf)
{
	int inheader = 0;

	while(ieof != NULL) {
		if(buf[0] == '\n') {
			break; /* end of headers */
		}

		/*
		** header lines which begin with whitespace
		** are continuation lines
		*/
		if((inheader == 0)
		|| ((buf[0] != ' ' && buf[0] != '\t'))) {
			/* not a continuation line
			** check for header
			*/
			if(isheader(buf) == 0) {
				/*
				** not a header
				*/
				break;
			}
			inheader = 1;
			haveheaders(buf);
		}
		(void) fputs(buf, spoolfp);
		if((fgets(buf, SMLBUF, stdin) == NULL)
		|| (buf[0] == '.' && buf[1] == '\n')) {
			ieof = NULL;
		}
	}

	if(isheader(buf)) {
		buf[0] = '\0';
	}
}

/*
** complete headers - add any required headers that are not in the message
*/
void compheaders(void)
{
	struct reqheaders *i;

	/*
	** look at the table of required headers and
	** add those that are missing to the spooled message.
	*/
	for(i = reqtab; i->name != NULL; i++) {
		if(i->have != 'Y') {
			(void) fprintf(spoolfp, "%s\n", i->field);
		}
	}
}

/*
** look at a string and determine
** whether or not it is a valid header.
*/
int isheader(char *s)
{
	char *p;

	/*
	** header field names must terminate with a colon
	** and may not be null.
	*/
	if(((p = index(s, ':')) == NULL) || (s == p)) {
		return(0);

	}
	/*
	** header field names must consist entirely of
	** printable ascii characters.
	*/
	while(s != p) {
		if((*s < '!') || (*s > '~')) {
			return(0);
		}
		s++;
	}
	/*
	** we hit the ':', so the field may be a header
	*/
	return(1);
}

/*
** compare the header field to those in the required header table.
** if it matches, then mark the required header as being present
** in the message.
*/
void haveheaders(char *s)
{
	struct reqheaders *i;

	for(i = reqtab; i->name != NULL; i++) {
		if(strncmpic(i->name, s, strlen(i->name)) == 0) {
			if((strncmpic("From:", s, 5) == 0)
			&& (from_addr != NULL)) {
				(void) sprintf(s, "From: %s\n", from_addr);
			}
			i->have = 'Y';
			break;
		}
	}
}

/*
** create default headers for the message.
*/
void def_headers(int argc,char **argv,char *from)
{
	def_to(argc, argv);	/* default To:		*/
	def_date();		/* default Date:	*/
	def_from(from);		/* default From: 	*/
	def_mid();		/* default Message-Id:	*/
}

/*
** default Date: in arpa format
*/
void def_date(void)
{
	(void) strcpy(dateline, "Date: ");
	(void) strcat(dateline, arpanows);
}

/*
** default Message-Id
**  Message-Id: <yymmddhhmm.AAppppp@hostdomain>
**
**	yy	 year
**	mm	 month
**	dd	 day
**	hh	 hour
**	mm	 minute
**	ppppp	process-id
**
** date and time are set by GMT
*/

void def_mid(void)
{
	(void) sprintf(midline, "Message-Id: <%02d%02d%02d%02d%02d.AA%05d@%s>",
		gmt->tm_year,
		gmt->tm_mon+1,
		gmt->tm_mday,
		gmt->tm_hour,
		gmt->tm_min,
		getpid(),
		hostdomain);
}

/*
** default From:
**  From: user@hostdomain (Full Name)
*/

void def_from(char *from)
{

	char *nameptr;
	char name[SMLBUF];
	char *getenv(), *login;
	char *pwfnam(), *pwuid();

	if (from_addr != NULL) {
		(void) sprintf(fromline, "From: %s", from_addr);
		return;
	}

	name[0] = '\0';
	if((nameptr = getenv("NAME")) != NULL) {
		(void) strcpy(name, nameptr);
	} else if((login = pwuid(getuid())) != NULL) {
		if((nameptr = pwfnam(login)) != NULL) {
			(void) strcpy(name, nameptr);
		}
	}
	if(name[0] != '\0') {
		(void) sprintf(fromline,
			"From: %s@%s (%s)", from, hostdomain, name);
	} else {
		(void) sprintf(fromline,
			"From: %s@%s", from, hostdomain);
	}
}

/*
** default To:
**  To: recip1, recip2, ...
**
** lines longer than 50 chars are continued on another line.
*/

void def_to(int argc,char **argv)
{
	int i, n;
	char *bol;

	bol = toline;
	(void) strcpy(bol, "To: ");
	for(n = i = 0; i < argc; i++) {
		(void) strcat(bol, argv[i]);

		if((index(argv[i], '!') == NULL)
		&& (index(argv[i], '@') == NULL)) {
			(void) strcat(bol, "@");
			(void) strcat(bol, hostdomain);
		}
		if(i+1 < argc) {
			n = strlen(bol);
			if(n > 50) {
				(void) strcat(bol, ",\n\t");
				bol = bol + strlen(bol);
				*bol = '\0';
				n = 8;
			} else {
				(void) strcat(bol, ", ");
			}
		}
	}
}
