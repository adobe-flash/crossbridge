#ifndef lint
static char sccsid[] = "@(#)tc.c	4.2 8/11/83";
#endif

 /* tc.c: find character not in table to delimit fields */
# include "t..c"

extern int ctype(int il,int ic);
extern void error(char *s);

int point(int s);

void choochar(void)
{
/* choose funny characters to delimit fields */
int had[128], ilin,icol, k;
char *s;
for(icol=0; icol<128; icol++)
	had[icol]=0;
F1 = F2 = 0;
for(ilin=0;ilin<nlin;ilin++)
	{
	if (instead[ilin]) continue;
	if (fullbot[ilin]) continue;
	for(icol=0; icol<ncol; icol++)
		{
		k = ctype(ilin, icol);
		if (k==0 || k == '-' || k == '=')
			continue;
		s = table[ilin][icol].col;
		if (point((int)s))
		while (*s)
			had[*s++]=1;
		s=table[ilin][icol].rcol;
		if (point((int)s))
		while (*s)
			had[*s++]=1;
		}
	}
/* choose first funny character */
for(
	s="\002\003\005\006\007!%&#/?,:;<=>@`^~_{}+-*ABCDEFGHIJKMNOPQRSTUVWXYZabcdefgjkoqrstwxyz";
		*s; s++)
	{
	if (had[*s]==0)
		{
		F1= *s;
		had[F1]=1;
		break;
		}
	}
/* choose second funny character */
for(
	s="\002\003\005\006\007:_~^`@;,<=>#%&!/?{}+-*ABCDEFGHIJKMNOPQRSTUVWXZabcdefgjkoqrstuwxyz";
		*s; s++)
	{
	if (had[*s]==0)
		{
		F2= *s;
		break;
		}
	}
if (F1==0 || F2==0)
	error("couldn't find characters to use for delimiters");
return;
}

int point(int s)
{
return(s>= 128 || s<0);
}
