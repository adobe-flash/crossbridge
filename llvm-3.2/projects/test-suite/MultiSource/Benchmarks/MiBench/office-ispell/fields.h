/*
 * $Id: fields.h 33046 2007-01-09 23:57:19Z lattner $
 *
 * $Log$
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:58:50  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.4  1994/01/05  20:13:43  geoff
 * Add the maxf parameter
 *
 * Revision 1.3  1994/01/04  02:40:22  geoff
 * Add field_line_inc, field_field_inc, and the FLD_NOSHRINK flag.
 *
 * Revision 1.2  1993/09/09  01:11:12  geoff
 * Add a return value to fieldwrite and support for backquotes.
 *
 * Revision 1.1  1993/08/25  21:32:05  geoff
 * Initial revision
 *
 */

/*
 * Structures used by the field-access package.
 */

typedef struct
    {
    int		nfields;	/* Number of fields in the line */
    int		hadnl;		/* NZ if line ended with a newline */
    char *	linebuf;	/* Malloc'ed buffer containing the line */
    char **	fields;		/* Malloc'ed array of pointers to fields */
    }
		field_t;

/*
 * Flags to fieldread and fieldmake
 */
#define FLD_RUNS	0x0001	/* Consider runs of delimiters same as one */
#define FLD_SNGLQUOTES	0x0002	/* Accept single-quoted fields */
#define FLD_BACKQUOTES	0x0004	/* Accept back-quoted fields */
#define FLD_DBLQUOTES	0x0008	/* Accept double-quoted fields */
#define FLD_SHQUOTES	0x0010	/* Use shell-style (embedded) quoting rules */
#define FLD_STRIPQUOTES	0x0020	/* Strip quotes from fields */
#define FLD_BACKSLASH	0x0040	/* Process C-style backslashes */
#define FLD_NOSHRINK	0x0080	/* Don't shrink memory before return */

#undef P
#ifdef __STDC__
#define P(x)	x
#else /* __STDC__ */
#define P(x)	()
#endif /* __STDC__ */

extern field_t *	fieldread P ((FILE * file, char * delims,
			  int flags, int maxf));
extern field_t *	fieldmake P ((char * line, int allocated,
			  char * delims, int flags, int maxf));
extern int		fieldwrite P ((FILE * file, field_t * fieldp,
			  int delim));
extern void		fieldfree P ((field_t * fieldp));

extern unsigned int	field_field_inc;
				/* Increment for expanding fields */
extern unsigned int	field_line_inc;
				/* Increment for expanding lines */
