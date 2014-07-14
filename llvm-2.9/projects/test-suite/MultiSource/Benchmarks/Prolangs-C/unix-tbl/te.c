#ifndef lint
static char sccsid[] = "@(#)te.c	4.2 8/11/83";
#endif

 /* te.c: error message control, input line count */
# include "t..c"

extern int swapin(void);

void error(char *s)
{
fprintf(stderr, "\n%s: line %d: %s\n", ifile, iline, s);
# ifdef unix
fprintf(stderr, "tbl quits\n");
exit(1);
# endif
# ifdef gcos
fprintf(stderr, "run terminated due to error condition detected by tbl preprocessor\n");
exit(0);
# endif
}

char *gets1(char *s)
{
char *p;
int nbl = 0;
iline++;
p=fgets(s,BUFSIZ,tabin);
while (p==0)
	{
	if (swapin()==0)
		return(0);
	p = fgets(s,BUFSIZ,tabin);
	}

while (*s) s++;
s--;
if (*s == '\n') *s-- =0;
for(nbl=0; *s == '\\' && s>p; s--)
	nbl++;
if (linstart && nbl % 2) /* fold escaped nl if in table */
	gets1(s+1);

return(p);
}
# define BACKMAX 500
char backup[BACKMAX];
char *backp = backup;

void un1getc(int c)
{
if (c=='\n')
	iline--;
*backp++ = c;
if (backp >= backup+BACKMAX)
	error("too much backup");
}

int get1char(void)
{
int c;
if (backp>backup)
	c = *--backp;
else
	c=getc(tabin);
if (c== EOF) /* EOF */
	{
	if (swapin() ==0)
		error("unexpected EOF");
	c = getc(tabin);
	}
if (c== '\n')
	iline++;
return(c);
}
