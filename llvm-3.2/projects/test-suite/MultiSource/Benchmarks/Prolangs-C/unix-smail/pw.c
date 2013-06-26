#ifndef lint
static char *sccsid = "@(#)pw.c	2.5 (smail) 9/15/87";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "defs.h"
#include <ctype.h>
#include <stdlib.h>

typedef struct pw_node pwlist;

struct pw_node {
	char *lname;			/* login name */
	char *fname;			/* full name  */
	int  uid;			/* user-id    */
	char *home;			/* login name */
	pwlist *vlink;			/* link to next item */
};

pwlist *pwhead;		/* head of linked list */
pwlist *pwparse();	/* head of linked list */

#define PNULL	((pwlist *) 0)

char *pwfnam(char *user)
{
	pwlist *f;

	/*
	** check for previously cached user
	*/

	for(f=pwhead; f != NULL; f=f->vlink) {
		if(strcmp(user, f->lname) == 0) {
			return(f->fname);
		}
	}
	/*
	** not found parse the password file
	*/

	while((f=pwparse()) != PNULL) {
		if(strcmp(user, f->lname) == 0) {
			return(f->fname);
		}
	}
	return(NULL);
}

char *pwuid(int uid)
{
	pwlist *f;

	/*
	** check for previously cached user
	*/

	for(f=pwhead; f != NULL; f=f->vlink) {
		if(uid == f->uid) {
			return(f->lname);
		}
	}
	/*
	** not found parse the password file
	*/

	while((f=pwparse()) != PNULL) {
		if(uid == f->uid) {
			return(f->lname);
		}
	}
	return(NULL);
}

#ifndef SENDMAIL
char *tilde(char *user)
{
	pwlist *f;

	/*
	** check for previously cached user
	*/

	for(f=pwhead; f != NULL; f=f->vlink) {
		if(strcmp(user, f->lname) == 0) {
			return(f->home);
		}
	}
	/*
	** not found parse the password file
	*/

	while((f=pwparse()) != PNULL) {
		if(strcmp(user, f->lname) == 0) {
			return(f->home);
		}
	}
	return(NULL);
}
#endif /* not SENDMAIL */

char *fullname(char *gecos)
{
	static char fname[SMLBUF];
	register char *cend;

	(void) strcpy(fname, gecos);
	if (cend = index(fname, ','))
		*cend = '\0';
	if (cend = index(fname, '('))
		*cend = '\0';
	/*
	** Skip USG-style 0000-Name nonsense if necessary.
	*/
	if (isdigit(*(cend = fname))) {
		if ((cend = index(fname, '-')) != NULL)
			cend++;
		else
			/*
			** There was no `-' following digits.
			*/
			cend = fname;
	}
	return (cend);
}

pwlist *pwparse(void)
{
	pwlist *f;
	char *p, *name;
	struct passwd *pwent, *getpwent();
	unsigned int i;
	static int pw_eof = 0;

/*
** Performance measurements indicate that the majority of time this
** application spends is in getpwent(). As getpwent just tests how 
** many authorized users there are, don't do the call.
*/

/*	if((pw_eof == 1)
**	|| ((pwent = getpwent()) == (struct passwd *) NULL)) {
*/
		pw_eof = 1;
		return(PNULL);
/*
**	}
*/
	/*
	** Get an entry from the password file.
	** Parse relevant strings.
	*/
	f = (pwlist *) malloc(sizeof(pwlist));
	if(f == PNULL) return(PNULL);

	f->vlink = pwhead;
	pwhead   = f;
	f->uid   = pwent->pw_uid;

	i=strlen(pwent->pw_name)+1;
	p = malloc(i);
	if(p == NULL) return(PNULL);
	f->lname = strcpy(p, pwent->pw_name);

	i=strlen(pwent->pw_dir)+1;
	p = malloc(i);
	if(p == NULL) return(PNULL);
	f->home  = strcpy(p, pwent->pw_dir);

	name = fullname(pwent->pw_gecos);
	i=strlen(name)+1;
	p = malloc(i);
	if(p == NULL) return(PNULL);
	f->fname = strcpy(p, name);
	return(f);
}

#ifdef FULLNAME
/*
** Resolve a full name to a login name.
** Not too much smarts here.
*/

char *res_fname(register char *user)
{
	long pos, middle, hi, lo;
	static long pathlength = 0;
	register char *s;
	int c;
	static FILE *file;
	int flag;
	char namebuf[SMLBUF], *path;
	extern enum edebug debug;
	extern char *fnlist;



DEBUG("res_fname: looking for '%s'\n", user);

	if(pathlength == 0) {	/* open file on first use */
		if((file=fopen(fnlist, "r")) == NULL) {
			DEBUG( "can't access %s.\n", fnlist);
			pathlength = -1;
		} else {
			(void) fseek(file, 0L, 2); 	/* find length */
			pathlength = ftell(file);
		}
	}

	if(pathlength == -1 ) return(NULL);

	lo = 0;
	hi = pathlength;
	path = namebuf;

	(void) strcpy( path, user );
	(void) strcat( path, "\t" );

	for( ;; ) {
		pos = middle = ( hi+lo+1 )/2;
		(void) fseek( file, pos, 0 );	/* find midpoint */
		if (pos != 0)		/* to beginning of next line */
			while( ( c=getc( file ) ) != EOF && c != '\n' );
		for( flag = 0, s = path; flag == 0; s++ ) { /* match??? */
			if ( *s == '\0' ) {
				goto solved;
			}
			c = getc( file );
			flag = lower( c ) - lower( *s );
		} 
		if (lo >= middle)		/* failure? */
			return(NULL);

		if(c != EOF && flag < 0)	/* close window */
			lo = middle;
		else 
			hi = middle - 1;
	}
/* 
** Now just copy the result.
*/
solved:
	while(((c  = getc(file)) != EOF) && (c != '\t') && (c != '\n')) {
		*path++ = c;
	}

	if(path == namebuf) {	/* NULL alias field */
		return(NULL);
	}

	*path = '\0';
	if((path = malloc((unsigned) strlen(namebuf)+1)) == NULL) {
		return(NULL);	/* sorry, no memory */
	}

	(void) strcpy(path, namebuf);
	return(path);

}
#endif	/* FULLNAME */
