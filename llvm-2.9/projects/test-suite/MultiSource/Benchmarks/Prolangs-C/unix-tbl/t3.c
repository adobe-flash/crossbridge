#ifndef lint
static char sccsid[] = "@(#)t3.c	4.2 8/11/83";
#endif

/* for solaris */
#define index  strchr

 /* t3.c: interpret commands affecting whole table */
#include <string.h>
# include "t..c"
#if 0
struct optstr {char *optnam; int *optadd;} options [] = {
	"expand", &expflg,
	"EXPAND", &expflg,
	"center", &ctrflg,
	"CENTER", &ctrflg,
	"box", &boxflg,
	"BOX", &boxflg,
	"allbox", &allflg,
	"ALLBOX", &allflg,
	"doublebox", &dboxflg,
	"DOUBLEBOX", &dboxflg,
	"frame", &boxflg,
	"FRAME", &boxflg,
	"doubleframe", &dboxflg,
	"DOUBLEFRAME", &dboxflg,
	"tab", &tab,
	"TAB", &tab,
	"linesize", &linsize,
	"LINESIZE", &linsize,
	"delim", &delim1,
	"DELIM", &delim1,
	0,0};
#endif

struct optstr {char *optnam; int *optadd;} options [21];

extern char *gets1(char *s);
extern int letter(int ch);
extern int prefix(char *small,char *big);
extern void error(char *s);
extern void un1getc(int c);

void backrest(char *cp);

void init_options(void)
{
  options[0].optnam =
    "expand";
  options[0].optadd =
    &expflg,
  options[1].optnam =
    "EXPAND";
  options[1].optadd =  
    &expflg,
  options[2].optnam =
    "center";
  options[2].optadd = 
    &ctrflg,
  options[3].optnam =
    "CENTER";
  options[3].optadd = 
    &ctrflg,
  options[4].optnam =
    "box";
  options[4].optadd = 
    &boxflg,
  options[5].optnam =
    "BOX";
  options[5].optadd = 
    &boxflg,
  options[6].optnam =
    "allbox";
  options[6].optadd = 
    &allflg,
  options[7].optnam =    
    "ALLBOX";
  options[7].optadd = 
    &allflg,
  options[8].optnam =
    "doublebox";
  options[8].optadd = 
    &dboxflg,
  options[9].optnam =
    "DOUBLEBOX";
  options[9].optadd =
    &dboxflg,
  options[10].optnam =
    "frame";
  options[10].optadd =
    &boxflg,
  options[11].optnam =
    "FRAME";
  options[11].optadd =
    &boxflg,
  options[12].optnam =
    "doubleframe";
  options[12].optadd =
    &dboxflg,
  options[13].optnam =
    "DOUBLEFRAME";
  options[13].optadd =
    &dboxflg,
  options[14].optnam =
    "tab";
  options[14].optadd =
    &tab,
  options[15].optnam =
    "TAB";
  options[15].optadd =
    &tab,
  options[16].optnam =
    "linesize";
  options[16].optadd = 
    &linsize,
  options[17].optnam =
    "LINESIZE";
  options[17].optadd =
    &linsize,
  options[18].optnam =
    "delim";
  options[18].optadd =
    &delim1,
  options[19].optnam =
    "DELIM";
  options[19].optadd =
    &delim1,
  options[20].optnam =
    0;
  options[20].optadd =
    0;
}

void getcomm(void)
{
char line[200], *cp, nb[25], *t;
struct optstr *lp;
int c, ci, found;
for(lp= options; lp->optnam; lp++)
	*(lp->optadd) = 0;
texname = texstr[texct=0];
tab = '\t';
printf(".nr %d \\n(.s\n", LSIZE);
gets1(line);
/* see if this is a command line */
if (index(line,';') == NULL)
	{
	backrest(line);
	return;
	}
for(cp=line; (c = *cp) != ';'; cp++)
	{
	if (!letter(c)) continue;
	found=0;
	for(lp= options; lp->optadd; lp++)
		{
		if (prefix(lp->optnam, cp))
			{
			*(lp->optadd) = 1;
			cp += strlen(lp->optnam);
			if (letter(*cp))
				error("Misspelled global option");
			while (*cp==' ')cp++;
			t=nb;
			if ( *cp == '(')
				while ((ci= *++cp) != ')')
					*t++ = ci;
			else cp--;
			*t++ = 0; *t=0;
			if (lp->optadd == &tab)
				{
				if (nb[0])
					*(lp->optadd) = nb[0];
				}
			if (lp->optadd == &linsize)
				printf(".nr %d %s\n", LSIZE, nb);
			if (lp->optadd == &delim1)
				{
				delim1 = nb[0];
				delim2 = nb[1];
				}
			found=1;
			break;
			}
		}
	if (!found)
		error("Illegal option");
	}
cp++;
backrest(cp);
return;
}

void backrest(char *cp)
{
char *s;
for(s=cp; *s; s++);
un1getc('\n');
while (s>cp)
	un1getc(*--s);
return;
}
