/* $RCSfile$$Revision: 15076 $$Date: 2004-07-21 07:09:51 -0700 (Wed, 21 Jul 2004) $
 *
 *    Copyright (c) 1989, Larry Wall
 *
 *    You may distribute under the terms of the GNU General Public License
 *    as specified in the README file that comes with the perl 3.0 kit.
 *
 * $Log$
 * Revision 1.2  2004/07/21 14:09:51  brukman
 * Add missing function declarations.
 *
 * Revision 1.1  2004/02/17 22:21:16  criswell
 * Initial commit of the perl Malloc Benchmark.  I've cheated a little by
 * generating the yacc output files and committing them directly, but it was
 * easier than disabling the Bison Voodoo that gets executed by default.
 *
 * Revision 4.0.1.1  91/04/12  09:16:12  lwall
 * patch1: you may now use "die" and "caller" in a signal handler
 * 
 * Revision 4.0  91/03/20  01:40:04  lwall
 * 4.0 baseline.
 * 
 */

struct string {
    char *	str_ptr;	/* pointer to malloced string */
    STRLEN	str_len;	/* allocated size */
    union {
	double	str_nval;	/* numeric value, if any */
	STAB	*str_stab;	/* magic stab for magic "key" string */
	long	str_useful;	/* is this search optimization effective? */
	ARG	*str_args;	/* list of args for interpreted string */
	HASH	*str_hash;	/* string represents an assoc array (stab?) */
	ARRAY	*str_array;	/* string represents an array */
	CMD	*str_cmd;	/* command for this source line */
    } str_u;
    STRLEN	str_cur;	/* length of str_ptr as a C string */
    STR		*str_magic;	/* while free, link to next free str */
				/* while in use, ptr to "key" for magic items */
    char	str_pok;	/* state of str_ptr */
    char	str_nok;	/* state of str_nval */
    unsigned char str_rare;	/* used by search strings */
    unsigned char str_state;	/* one of SS_* below */
				/* also used by search strings for backoff */
#ifdef TAINT
    bool	str_tainted;	/* 1 if possibly under control of $< */
#endif
};

struct stab {	/* should be identical, except for str_ptr */
    STBP *	str_ptr;	/* pointer to malloced string */
    STRLEN	str_len;	/* allocated size */
    union {
	double	str_nval;	/* numeric value, if any */
	STAB	*str_stab;	/* magic stab for magic "key" string */
	long	str_useful;	/* is this search optimization effective? */
	ARG	*str_args;	/* list of args for interpreted string */
	HASH	*str_hash;	/* string represents an assoc array (stab?) */
	ARRAY	*str_array;	/* string represents an array */
	CMD	*str_cmd;	/* command for this source line */
    } str_u;
    STRLEN	str_cur;	/* length of str_ptr as a C string */
    STR		*str_magic;	/* while free, link to next free str */
				/* while in use, ptr to "key" for magic items */
    char	str_pok;	/* state of str_ptr */
    char	str_nok;	/* state of str_nval */
    unsigned char str_rare;	/* used by search strings */
    unsigned char str_state;	/* one of SS_* below */
				/* also used by search strings for backoff */
#ifdef TAINT
    bool	str_tainted;	/* 1 if possibly under control of $< */
#endif
};

/* some extra info tacked to some lvalue strings */

struct lstring {
    struct string lstr;
    STRLEN	lstr_offset;
    STRLEN	lstr_len;
};

/* These are the values of str_pok:		*/
#define SP_VALID	1	/* str_ptr is valid */
#define SP_FBM		2	/* string was compiled for fbm search */
#define SP_STUDIED	4	/* string was studied */
#define SP_CASEFOLD	8	/* case insensitive fbm search */
#define SP_INTRP	16	/* string was compiled for interping */
#define SP_TAIL		32	/* fbm string is tail anchored: /foo$/  */
#define SP_MULTI	64	/* symbol table entry probably isn't a typo */
#define SP_TEMP		128	/* string slated to die, so can be plundered */

#define Nullstr Null(STR*)

/* These are the values of str_state:		*/
#define SS_NORM		0	/* normal string */
#define SS_INCR		1	/* normal string, incremented ptr */
#define SS_SARY		2	/* array on save stack */
#define SS_SHASH	3	/* associative array on save stack */
#define SS_SINT		4	/* integer on save stack */
#define SS_SLONG	5	/* long on save stack */
#define SS_SSTRP	6	/* STR* on save stack */
#define SS_SHPTR	7	/* HASH* on save stack */
#define SS_SNSTAB	8	/* non-stab on save stack */
#define SS_SCSV		9	/* callsave structure on save stack */
#define SS_SAPTR	10	/* ARRAY* on save stack */
#define SS_HASH		253	/* carrying an hash */
#define SS_ARY		254	/* carrying an array */
#define SS_FREE		255	/* in free list */
/* str_state may have any value 0-255 when used to hold fbm pattern, in which */
/* case it indicates offset to rarest character in screaminstr key */

/* the following macro updates any magic values this str is associated with */

#ifdef TAINT
#define STABSET(x) \
    (x)->str_tainted |= tainted; \
    if ((x)->str_magic) \
	stabset((x)->str_magic,(x))
#else
#define STABSET(x) \
    if ((x)->str_magic) \
	stabset((x)->str_magic,(x))
#endif

str_sset(STR *dstr, register STR *sstr);
#define STR_SSET(dst,src) if (dst != src) str_sset(dst,src)

EXT STR **tmps_list;
EXT int tmps_max INIT(-1);
EXT int tmps_base INIT(-1);

char *str_2ptr();
double str_2num();
STR *str_mortal();
STR *str_2mortal();
STR *str_make();
STR *str_nmake();
STR *str_smake();
int str_cmp();
int str_eq();
void str_magic();
void str_insert();
STRLEN str_len();
