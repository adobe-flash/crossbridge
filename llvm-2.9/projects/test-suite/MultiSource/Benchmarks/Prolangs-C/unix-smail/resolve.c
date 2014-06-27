/*
**
**  Resolve.c
**
**  Routes then resolves addresses into UUCP or LOCAL.
**
*/
#ifndef lint
static char 	*sccsid="@(#)resolve.c	2.5 (smail) 9/15/87";
#endif

#include	<ctype.h>
#include	<stdio.h>
#include	"defs.h"

extern int exitstat;		/* set if address doesn't resolve 	*/
extern enum ehandle handle;	/* what mail we can handle		*/
extern enum edebug debug;	/* verbose and debug modes		*/
extern enum erouting routing;	/* when to route addresses		*/
extern char hostdomain[];	/* */
extern char hostname[];		/* */
extern char *pathfile;		/* location of path database		*/
extern int getcost;		/* get path cost even if not routing    */

char *sform();

/*
**
**  rsvp(): how to resolve addresses.
**
**  After parsing an address into <form>, the resolved form will be
**  rsvp( form ).  If == ROUTE, we route the parsed address and parse again.
**
*/

# define rsvp(a) table[(int)a][(int)handle]

enum eform table[5][3] = {
/*	all		justuucp	none */
{	ERROR, 		ERROR, 		ERROR }, 	/* error */
{	LOCAL, 		LOCAL,	 	LOCAL }, 	/* local */
{	ROUTE, 		LOCAL, 		LOCAL }, 	/* domain */
{	UUCP, 		UUCP, 		LOCAL }, 	/* uucp */
{	ERROR, 		ERROR, 		ERROR }};	/* route */

/*
**
**  resolve(): resolve addresses to <host, user, form>.
**
**  This is a gnarly piece of code, but it does it all.  Each section 
**  is documented.
**
*/

enum eform resolve(char *address,char *domain,char *user ,int *cost)
{
	enum eform form;		/* the returned form	*/ 
	enum eform parse();		/* to crack addresses	*/
	int parts;			/* to ssplit addresses	*/
	char *partv[MAXPATH];		/* "  "      "		*/
	char temp[SMLBUF];		/* "  "      "		*/
	int i;
		

/*
**  If we set REROUTE and are prepared to deliver UUCP mail, we split the 
**  address apart at !'s and try to resolve successively larger righthand 
**  substrings until we succeed.  Otherwise, we just resolve the whole thing 
**  once.
*/
	if ((routing == REROUTE) && (rsvp( UUCP ) == UUCP)) {
		parts = ssplit( address, '!', partv );
	} else {
		parts = 1;
		partv[0] = address;
	}
/*
**  This for(i) loop selects successively larger
**  righthand substrings of the address.
*/
	for( i = parts - 1; i >= 0; i-- ) {
/*
**  Parse the address.
*/
		(void) strcpy( temp, partv[i] );
		form = parse( temp, domain, user );

DEBUG("resolve: parse address '%s' = '%s' @ '%s' (%s)\n",
	temp,user,domain,sform(form));

/*
**  If we are looking at a substring (that's not the entire string)
**  which parses to a LOCAL address, we skip to the next larger substring.
*/
		if((i != 0) && (form == LOCAL))
			continue;
/*
**  Routing, when required, is the next step.
**  We route the address if we have a ROUTE form
**  or if we have a UUCP form and we are told to
**  route ALWAYS or REROUTE (i.e., routing != JUSTDOMAIN)
*/
		if((rsvp( form ) == ROUTE)
		 ||((rsvp( form ) == UUCP) && (routing != JUSTDOMAIN ))) {

			int look_smart = 0;

			if((routing == REROUTE) && (i == 0)) {
				look_smart = 1; /* last chance */
			}

			/* route() puts the new route in 'temp' */
			if(route(domain,user,look_smart,temp,cost) != EX_OK) {
				continue;	/* If routing fails, try
						/* next larger substring.
						/* */
			}
/*
**  After routing, reparse the new route into domain and user. 
*/
			form = parse( temp, domain, user );

DEBUG("resolve: parse route '%s' = '%s' @ '%s' (%s)\n",
	temp,user,domain,sform(form));

		} else if((getcost) && (rsvp(form) == UUCP)) {
			/* get the cost of the route
			** even if we're not going route the mail.
			** this allows smart decisions about using
			** the -r flag to uux when we're not routing.
			*/
			char junk[SMLBUF];
			if(route(domain,user,0,junk,cost) != EX_OK) {
				continue;	/* If routing fails, try
						/* next larger substring.
						/* */
			}
		}
		break;	/* route is resolved */
	}
/*
**  For LOCAL mail in non-local format, we rewrite the full address into 
**  <user> and leave <domain> blank.
*/
	if ((rsvp( form ) == LOCAL) && (form != LOCAL )) {
		build( domain, user, form, temp );
		(void) strcpy( user, temp );
		(void) strcpy( domain, "" );
		form = LOCAL;
	}
/*
**  If we were supposed to route an address but failed (form == ERROR), 
**  or after routing we are left with an address that still needs to
**  be routed (rsvp( form ) == ROUTE), complain.
*/
	if ((form == ERROR) || (rsvp( form ) == ROUTE )) {
		exitstat = EX_NOHOST;
		ADVISE("resolve failed '%s' = '%s' @ '%s' (%s)\n",
			address, user, domain, sform(form));
		form = ERROR;
	} else {
		ADVISE("resolve '%s' = '%s' @ '%s' (%s)\n",
			address, user, domain, sform(form));
	}
	return ( form );
}

/*
**
**  route(): route domain, plug in user.
**
**  Less complicated than it looks.  Each section is documented.
**
*/

int route(char *domain,char *user,int look_smart,char *result,int *cost)
{
	int	uucpdom = 0;
	int	domains, step;			/* to split domain	*/
	char	*domainv[MAXDOMS];		/* "  "     "		*/
	char	temp[SMLBUF], path[SMLBUF];

/*
**  Fully qualify the domain, and then strip the last (top level domain) 
**  component off, so that we look it up separately.
*/
	temp[0] = '.';
	(void) strcpy(temp+1, domain );

	domains = ssplit( temp+1, '.', domainv );

/*
** check target domain for the local host name and host domain.
** if it matches, then skip the lookup in the database.
** this prevents mail loops for cases where SMARTHOST is defined
** in the routing table, but the local host is not.  It also is
** a little faster when the local host is the target domain.
*/
	if((strcmpic(domain, hostname) == 0)
	|| (strcmpic(domain, hostdomain) == 0)) {
		step = 0;
		*cost = 0;
		(void) strcpy(path, "%s");
DEBUG("route: '%s' is local\n", domain);
		goto route_complete;
	}

	/* If the domain ends in .UUCP, trim that off. */
	if((domains > 0) && isuucp(domainv[domains-1])) {
		domains--;
		domainv[domains][-1] = '\0';
		uucpdom = 1;
	}
/*
**  Try to get the path for successive components of the domain.  
**  Example for osgd.cb.att.uucp:
**	osgd.cb.att
**	cb.att
**	att
**	uucp ( remember stripping top level? )
**	SMARTHOST
**  Returns with error if we find no path.
*/
	for(step = 0; (step < domains); step++) {
		if((getpath(domainv[step]-1, path, cost) == EX_OK) /* w/ dot */
		|| (getpath(domainv[step]  , path, cost) == EX_OK))/* no dot */
			break;
	}

	if(step == domains) {
	/*
	** we've looked at each component of the domain without success
	*/
		/*
		** If domain is a UUCP address, look for a UUCP gateway.
		*/
		if((uucpdom == 0) || (getpath(".UUCP", path, cost) != EX_OK)) {
			/*
			** The domain not is a UUCP address, or we can't
			** find a UUCP gateway.  If this is our last chance,
			** look for a smarter host to deliver the mail.
			*/
			if((look_smart == 0)
			|| (getpath(SMARTHOST, path, cost) != EX_OK)) {
				/*
				** All our efforts have been in vain.
				** Tell them the bad news.
				*/
				DEBUG("route '%s' failed\n", domain);
				return( EX_NOHOST );
			}
		}
	}

route_complete:

DEBUG("route:  '%s' (%s) = '%s' (%d)\n", domain, domainv[step]?domainv[step]:"NULL", path, *cost);

/*
**  If we matched on the entire domain name, this address is fully resolved, 
**  and we plug <user> into it.  If we matched on only part of the domain 
**  name, we plug <domain>!<user> in.  
*/
	build(domain, user, (step == 0) ? LOCAL : UUCP, temp);
	(void) sprintf(result, path, temp);
	return( EX_OK );
}
