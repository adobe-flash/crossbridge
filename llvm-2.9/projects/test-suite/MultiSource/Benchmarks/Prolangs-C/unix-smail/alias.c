#ifndef lint
static char *sccsid = "@(#)alias.c	2.5 (smail) 9/15/87";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "defs.h"
#include <ctype.h>

extern enum edebug debug;	/* verbose and debug modes		*/
extern char hostdomain[];
extern char hostname[];
extern char *aliasfile;

/*
**
** Picture of the alias graph structure
**
**	head
**       |
**       v
**	maps -> mark -> gjm -> mel -> NNULL
**       |
**       v
**	sys ->  root -> ron -> NNULL
**       |
**       v
**	root -> mark -> chris -> lda -> NNULL
**       |
**       v
**      NNULL
*/

typedef struct alias_node node;

static struct alias_node {
	char *string;
	node *horz;
	node *vert;
};

#ifndef SENDMAIL
static node aliases = {"", 0, 0}; /* this is the 'dummy header' */
#endif /* not SENDMAIL */

/*
** lint free forms of NULL
*/

#define NNULL	((node   *) 0)
#define CNULL	('\0')

/*
** string parsing macros
*/
#define SKIPWORD(Z)  while(*Z!=' ' && *Z!='\t' && *Z!='\n' && *Z!=',') Z++;
#define SKIPSPACE(Z) while(*Z==' ' || *Z=='\t' || *Z=='\n' || *Z==',') Z++;

static int nargc = 0;
static char *nargv[MAXARGS];

void	add_horz();
void	load_alias(), strip_comments();
int	recipients();
node	*pop();
#ifndef SENDMAIL
node	*v_search(), *h_search();
char	*tilde();
#endif	/* not SENDMAIL */

/* our horizontal linked list looks like a stack */
#define push		add_horz

#define escape(s)	((*s != '\\') ? (s) : (s+1))

char **alias(int *pargc,char **argv)
{
/*
**  alias the addresses
*/
	int	i;
	char	domain[SMLBUF], ubuf[SMLBUF], *user;
	node	*addr, addrstk;
	node	*flist,  fliststk, *u;

#ifndef SENDMAIL
	FILE	*fp;
	node	*a;
	char	*home, buf[SMLBUF];
	int	aliased;
	struct	stat st;
#endif /* not SENDMAIL */

#ifdef FULLNAME
	char *res_fname();	/* Does fullname processing */
#endif

	addr  = &addrstk;
	flist = &fliststk;
	user  = ubuf;

	addr->horz = NNULL;
	flist->horz  = NNULL;

	/*
	** push all of the addresses onto a stack
	*/
	for(i=0; i < *pargc; i++) {
		push(addr, argv[i]);
	}

	/*
	** for each adress, check for included files, aliases,
	** full name mapping, and .forward files
	*/

	while((nargc < MAXARGS) && ((u = pop(addr)) != NNULL)) {
#ifndef SENDMAIL
		if(strncmpic(u->string, ":include:", 9) == 0) {
			/*
			** make sure it's a full path name
			** don't allow multiple sourcing
			** of a given include file
			*/
			char *p = u->string + 9;

			if((*p == '/')
			&& (h_search(flist, p) == NULL)) {
				push(flist, p);
				if((stat(p, &st) >= 0)
				&&((st.st_mode & S_IFMT) == S_IFREG)
				&&((fp = fopen(p, "r")) != NULL)) {
					while(fgets(buf, sizeof buf, fp)) {
						(void) recipients(addr, buf);
					}
					(void) fclose(fp);
				}
			}
			continue;
		}
#endif /* not SENDMAIL */
		/*
		** parse the arg to see if it's to be aliased
		*/

		if(islocal(u->string, domain, ubuf) == 0) {
			goto aliasing_complete;
		}

		/*
		** local form - try to alias user
		** aliases file takes precedence over ~user/.forward
		** since that's the way that sendmail does it.
		*/

#ifdef LOWERLOGNAME
		/* squish 'user' into lower case */
		for(user = ubuf; *user ; user++) {
			*user = lower(*user);
		}
#endif
		user = escape(ubuf);

		(void) strcpy(u->string, user);	/* local => elide domain */
#ifndef SENDMAIL
		/*
		** check for alias - all this complication is necessary
		** to handle perverted aliases like these:
		** # mail to 's' resolves to 't' 'm' and 'rmt!j'
		** s	t,g,j,m
		** g	j,m
		** j	rmt!j
		** # mail to 'a' resolves to 'rmt!d'
		** a	b c
		** b	c
		** c	rmt!d
		** # mail to x resolves to 'x'
		** x	local!x
		** # mail to 'y' resolves to 'y' and 'z'
		** y	\y z
		*/
		if(((a = v_search(user)) != NNULL)) {
			char dtmpb[SMLBUF], utmpb[SMLBUF], *ut;
			int user_inalias = 0;
			node *t = a;

			for(a = a->horz; a != NNULL; a=a->horz) {
				if(islocal(a->string, dtmpb, utmpb)) {
#ifdef LOWERLOGNAME
					/* squish 'utmpb' into lower case */
					for(ut = utmpb; *ut ; ut++) {
						*ut = lower(*ut);
					}
#endif

					ut = escape(utmpb);
#ifdef CASEALIAS
					if(strcmp(ut, user) == 0)
#else
					if(strcmpic(ut, user) == 0)
#endif
					{
						user_inalias = 1;
					} else {
						push(addr, a->string);
					}
				} else {
					push(addr, a->string);
				}
			}
			t->horz = NNULL; /* truncate horz list of aliases */
			if(user_inalias == 0) {
				continue;
			}
		}

		if((home = tilde(user)) != NULL) {
			/* don't allow multiple sourcing
			** of a given .forward file
			*/

			if((h_search(flist, home) != NULL)) {
				continue;
			}
			push(flist, home);

			/*
			** check for ~user/.forward file
			** must be a regular, readable file
			*/

			(void) sprintf(buf, "%s/%s", home, ".forward");
			if((stat(buf, &st) >= 0)
			&&((st.st_mode & S_IFMT) == S_IFREG)
			&&((st.st_mode & 0444)   == 0444)
			&&((fp = fopen(buf, "r")) != NULL)) {
				aliased = 0;
				while(fgets(buf, sizeof buf, fp)) {
					aliased |= recipients(addr, buf);
				}
				(void) fclose(fp);
				if(aliased) {
					continue;
				}
			}
		}
#endif /* not SENDMAIL */

#ifdef FULLNAME
		/*
		** Do possible fullname substitution.
		*/
#ifdef DOT_REQD
		if (index(user, '.') != NULL)
#endif
		{
			static char t_dom[SMLBUF], t_unam[SMLBUF];
			char *t_user = res_fname(user);
			if (t_user != NULL) {
				if(islocal(t_user, t_dom, t_unam) == 0) {
					/* aliased to non-local address */
					push(addr, t_user);
					continue;
				}
				if(strcmp(t_unam, user) != 0) {
					/* aliased to different local address */
					push(addr, t_unam);
					continue;
				}
			}
		}
#endif

aliasing_complete:
		user = escape(u->string);
		for(i=0; i < nargc; i++) {
			if(strcmpic(nargv[i], user) == 0) {
				break;
			}
		}

		if(i == nargc) {
			nargv[nargc++] = user;
		}
	}
	*pargc     = nargc;
	return(nargv);
}

#ifndef SENDMAIL
/*
** v_search
**	given an string, look for its alias in
**	the 'vertical' linked list of aliases.
*/
node *v_search(char *user)
{
	node *head;
	node *a;
	static int loaded = 0;

	head = &aliases;
	if(loaded == 0) {
		load_alias(head, aliasfile);
		loaded = 1;
	}

	for(a = head->vert; a != NNULL; a = a->vert) {
#ifdef CASEALIAS
		if(strcmp(a->string, user) == 0)
#else
		if(strcmpic(a->string, user) == 0)
#endif
		{
			break;
		}
	}
	if(a == NNULL) {		/* not in graph */
		return(NNULL);
	}
	return(a);
}

/*
** h_search
**	given an string, look for it in
**	a 'horizontal' linked list of strings.
*/
node *h_search(node *head,char *str)
{
	node *a;
	for(a = head->horz; a != NNULL; a = a->horz) {
#ifdef CASEALIAS
		if(strcmp(a->string, str) == 0)
#else
		if(strcmpic(a->string, str) == 0)
#endif
		{
			break;
		}
	}
	return(a);
}
#endif /* not SENDMAIL */

/*
** load_alias
**	parse an 'aliases' file and add the aliases to the alias graph.
**	Handle inclusion of other 'aliases' files.
*/

void load_alias(node *head,char *filename)
{
	FILE *fp;
	node *v, *h, *add_vert();
	char domain[SMLBUF], user[SMLBUF];
	char *p, *b, buf[SMLBUF];

	if((fp = fopen(filename,"r")) == NULL) {
DEBUG("load_alias open('%s') failed\n", filename);
		return;
	}

	while(fgets(buf, sizeof buf, fp) != NULL) {
		p = buf;
		if((*p == '#') || (*p == '\n')) {
			continue;
		}

		/*
		** include another file of aliases
		*/

		if(strncmp(p, ":include:", 9) == 0) {
			char *nl;
			p += 9;
			if((nl = index(p, '\n')) != NULL) {
				*nl = CNULL;
			}
DEBUG("load_alias '%s' includes file '%s'\n", filename, p);
			load_alias(head, p);
			continue;
		}

		/*
		**  if the first char on the line is a space or tab
		**  then it's a continuation line.  Otherwise,
		**  we start a new alias.
		*/
		if(*p != ' ' && *p != '\t') {
			b = p;
			SKIPWORD(p);
			*p++ = CNULL;
			/*
			** be sure that the alias is in local form
			*/
			if(islocal(b, domain, user) == 0) {
				/*
				** non-local alias format - skip it
				*/
				continue;
			}
			/*
			** add the alias to the (vertical) list of aliases
			*/
			if((h = add_vert(head, user)) == NNULL) {
DEBUG("load_alias for '%s' failed\n", b);
				return;
			}
		}
		/*
		**  Next on the line is the list of recipents.
		**  Strip out each word and add it to the
		**  horizontal linked list.
		*/
		(void) recipients(h, p);
	}
	(void) fclose(fp);
	/*
	** strip out aliases which have no members
	*/
	for(v = head; v->vert != NNULL; ) {
		if(v->vert->horz == NNULL) {
			v->vert = v->vert->vert;
		} else {
			v = v->vert;
		}
	}
}

/*
** add each word in a string (*p) of recipients
** to the (horizontal) linked list associated with 'h'
*/

int recipients(node *h,char *p)
{

	char *b, d[SMLBUF], u[SMLBUF];
	int ret = 0;

	strip_comments(p);	/* strip out stuff in ()'s */

	SKIPSPACE(p);		/* skip leading whitespace on line */

	while((*p != NULL) && (*p != '#')) {
		b = p;
		if(*b == '"') {
			if((p = index(++b, '"')) == NULL) {
				/* syntax error - no matching quote */
				/* skip the rest of the line */
				return(ret);
			}
		} else {
			SKIPWORD(p);
		}

		if(*p != CNULL) {
			*p++ = CNULL;
		}

		/* don't allow aliases of the form
		** a	a
		*/
		if((islocal(b, d, u) == 0)
		|| (strcmpic(h->string, u) != 0)) {
			add_horz(h, b);
			ret = 1;
		}
		SKIPSPACE(p);
	}
	return(ret);
}

/*
** some aliases may have comments on the line like:
**
** moderators	moderator@somehost.domain	(Moderator's Name)
**		moderator@anotherhost.domain	(Another Moderator's Name)
**
** strip out the stuff in ()'s
**
*/

void strip_comments(char *p)
{
	char *b;
	while((p = index(p, '(')) != NULL) {
		b = p++;	/*
				** save pointer to open parenthesis
				*/
		if((p = index(p, ')')) != NULL) {/* look for close paren */
			(void) strcpy(b, ++p);	 /* slide string left    */
		} else {
			*b = CNULL;	/* no paren, skip rest of line  */
			break;
		}
	}
}

/*
** add_vert - add a (vertical) link to the chain of aliases.
*/

node *add_vert(node *head,char *str)
{
	char *p;
/*	void free();         */
	node *new;

	/*
	** strip colons off the end of alias names
	*/
	if((p = index(str, ':')) != NULL) {
		*p = CNULL;
	}
	if((new = (node *) malloc(sizeof(node))) != NNULL) {
		if((new->string = malloc((unsigned) strlen(str)+1)) == NULL) {
			free(new);
			new = NNULL;
		} else {
			(void) strcpy(new->string, str);
			new->vert   = head->vert;
			new->horz   = NNULL;
			head->vert  = new;
/*DEBUG("add_vert %s->%s\n", head->string, new->string);/* */
		}
	}
	return(new);
}

/*
** add_horz - add a (horizontal) link to the chain of recipients.
*/

void add_horz(node *head,char *str)
{
	node *new;

	if((new = (node *) malloc(sizeof(node))) != NNULL) {
		if((new->string = malloc((unsigned) strlen(str)+1)) == NULL) {
			free(new);
			new = NNULL;
		} else {
			(void) strcpy(new->string, str);
			new->horz  = head->horz;
			new->vert  = NNULL;
			head->horz = new;
		}
/*DEBUG("add_horz %s->%s\n", head->string, new->string);/* */
	}
}

node *pop(node *head)
{
	node *ret = NNULL;


	if(head != NNULL) {
		ret = head->horz;
		if(ret != NNULL) {
			head->horz = ret->horz;
		}
	}
	return(ret);
}
