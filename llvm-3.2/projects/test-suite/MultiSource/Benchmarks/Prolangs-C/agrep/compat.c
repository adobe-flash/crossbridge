/* test the conflicts between options */
#include <stdio.h>

extern int FILENAMEONLY, APPROX, PAT_FILE, COUNT, INVERSE, BESTMATCH;
extern FILEOUT;
extern REGEX;
extern DELIMITER;
extern WHOLELINE;
extern LINENUM;
extern I, S, DD;
extern JUMP;
extern char Progname[32];

void compat(void)
{
	if(BESTMATCH)  if(COUNT || FILENAMEONLY || APPROX || PAT_FILE) {
		BESTMATCH = 0;
		fprintf(stderr, "%s: WARNING!!! -B option ignored when -c, -l, -f, or -# is on\n", Progname);
	}
	if(PAT_FILE)   {
		if(APPROX)  {
			fprintf(stderr, "WARNING!!!  approximate matching is not supported with -f option\n");
		}
/*
		if(INVERSE) {
			fprintf(stderr, "%s: -f and -v are not compatible\n", Progname);
			exit(2);
		}
*/
		if(LINENUM) {
			fprintf(stderr, "%s: -f and -n are not compatible\n", Progname);
			exit(2);
		}
		if(DELIMITER) {
			fprintf(stderr, "%s: -f and -d are not compatible\n", Progname);
			exit(2);
		}
	}
	if(JUMP) {
		if(REGEX) {
			fprintf(stderr, "WARNING!!! -D#, -I#, or -S# option is ignored for regular expression pattern\n");
			JUMP = 0;
		}
		if(I == 0 || S == 0 || DD == 0) {
			fprintf(stderr, "%s: the error cost cannot be 0\n", Progname);
			exit(2);
		}
	}
	if(DELIMITER) {
		if(WHOLELINE) {
			fprintf(stderr, "%s: -d and -x is not compatible\n", Progname);
			exit(2);
		}
	}

}


