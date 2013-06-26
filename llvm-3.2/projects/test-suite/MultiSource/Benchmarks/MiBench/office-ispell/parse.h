typedef union
    {
    int			simple;		/* Simple char or lval from yylex */
    struct
	{
	char *		set;		/* Character set */
	int		complement;	/* NZ if it is a complement set */
	}
			charset;
    unsigned char *	string;		/* String */
    ichar_t *		istr;		/* Internal string */
    struct flagent *	entry;		/* Flag entry */
    } YYSTYPE;
#define	ALLAFFIXES	257
#define	ALTSTRINGCHAR	258
#define	ALTSTRINGTYPE	259
#define	BOUNDARYCHARS	260
#define	COMPOUNDMIN	261
#define	COMPOUNDWORDS	262
#define	CONTROLLED	263
#define	DEFSTRINGTYPE	264
#define	FLAG	265
#define	FLAGMARKER	266
#define	NROFFCHARS	267
#define	OFF	268
#define	ON	269
#define	PREFIXES	270
#define	RANGE	271
#define	SUFFIXES	272
#define	STRING	273
#define	STRINGCHAR	274
#define	TEXCHARS	275
#define	WORDCHARS	276


extern YYSTYPE parse.ylval;
