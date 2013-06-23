#ifndef lint
static char sccsid[] = "@(#)t1.c	4.2 8/11/83";
#endif

 /* t1.c: main control and input switching */
#
# include "t..c"
#include <signal.h>
# ifdef gcos
/* required by GCOS because file is passed to "tbl" by troff preprocessor */
# define _f1 _f
extern FILE *_f[];
# endif

# define MACROS "/usr/lib/tmac.s"
# define PYMACS "/usr/lib/tmac.m"

# ifdef gcos
# define MACROS "cc/troff/smac"
# define PYMACS "cc/troff/mmac"
# endif

# define ever (;;)

extern char *gets1(char *s);
extern int prefix(char *small,char *big);
extern void tableput(void);
extern int match(char *s1,char *s2);
extern void error(char *s);

int tbl(int argc,char *argv[]);
void setinp(int argc,char **argv);
int swapin(void);


int main(int argc,char *argv[])
{
extern init_options();
# ifdef unix
void badsig(void);
signal(SIGPIPE, badsig);
# endif
# ifdef gcos
if(!intss()) tabout = fopen("qq", "w"); /* default media code is type 5 */
# endif
init_options();
exit(tbl(argc,argv));
return 0;
}


int tbl(int argc,char *argv[])
{
char line[BUFSIZ];
/* required by GCOS because "stdout" is set by troff preprocessor */
tabin=stdin; tabout=stdout;
setinp(argc,argv);
while (gets1(line))
	{
	fprintf(tabout, "%s\n",line);
	if (prefix(".TS", line))
		tableput();
	}
fclose(tabin);
return(0);
}
int sargc;
char **sargv;

void setinp(int argc,char **argv)
{
	sargc = argc;
	sargv = argv;
	sargc--; sargv++;
	if (sargc>0)
		swapin();
}

int swapin(void)
{
	while (sargc>0 && **sargv=='-') /* Mem fault if no test on sargc */
		{
		if (sargc<=0) return(0);
		if (match("-ms", *sargv))
			{
			*sargv = MACROS;
			break;
			}
		if (match("-mm", *sargv))
			{
			*sargv = PYMACS;
			break;
			}
		if (match("-TX", *sargv))
			pr1403=1;
		sargc--; sargv++;
		}
	if (sargc<=0) return(0);
# ifdef unix
/* file closing is done by GCOS troff preprocessor */
	if (tabin!=stdin) fclose(tabin);
# endif
	tabin = fopen(ifile= *sargv, "r");
	iline=1;
# ifdef unix
/* file names are all put into f. by the GCOS troff preprocessor */
	fprintf(tabout, ".ds f. %s\n",ifile);
# endif
	if (tabin==NULL)
		error("Can't open file");
	sargc--;
	sargv++;
	return(1);
}
# ifdef unix

void badsig(void)
{
signal(SIGPIPE, SIG_IGN);
 exit(0);
}
# endif
