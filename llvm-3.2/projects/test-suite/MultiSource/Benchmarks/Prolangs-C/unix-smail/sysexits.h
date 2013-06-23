/*
**	@(#)sysexits.h	2.5 (smail) 9/15/87
*/

# define EX_OK		0	/* successful termination */
# define EX_USAGE	64	/* command line usage error */
# define EX_NOHOST	68	/* host name unknown */
# define EX_UNAVAILABLE	69	/* service unavailable */
# define EX_SOFTWARE	70	/* internal software error */
# define EX_OSFILE	72	/* critical OS file missing */
# define EX_CANTCREAT	73	/* can't create (user) output file */
# define EX_TEMPFAIL	75	/* temp failure; user is invited to retry */
