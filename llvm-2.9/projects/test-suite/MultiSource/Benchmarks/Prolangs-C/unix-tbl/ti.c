#ifndef lint
static char sccsid[] = "@(#)ti.c	4.2 8/11/83";
#endif

 /* ti.c: classify line intersections */
# include "t..c"
/* determine local environment for intersections */

extern int lefdata(int i,int c);
extern int allh(int i);
extern int thish(int i,int c);

int up1(int i);

int interv(int i,int c)
{
int ku, kl;
if (c>=ncol || c == 0)
	{
	if (dboxflg)
		{
		if (i==0) return(BOT);
		if (i>=nlin) return(TOP);
		return(THRU);
		}
	if (c>=ncol)
		return(0);
	}
ku = i>0 ? lefdata(i-1,c) : 0;
if (i+1 >= nlin)
	kl=0;
else
kl = lefdata(allh(i) ? i+1 : i, c);
if (ku==2 && kl==2) return(THRU);
if (ku ==2) return(TOP);
if (kl==BOT) return(2);
return(0);
}

int interh(int i,int c)
{
int kl, kr;
if (fullbot[i]== '=' || (dboxflg && (i==0 || i>= nlin-1)))
	{
	if (c==ncol)
		return(LEFT);
	if (c==0)
		return(RIGHT);
	return(THRU);
	}
if (i>=nlin) return(0);
kl = c>0 ? thish (i,c-1) : 0;
if (kl<=1 && i>0 && allh(up1(i)))
	kl = c>0 ? thish(up1(i),c-1) : 0;
kr = thish(i,c);
if (kr<=1 && i>0 && allh(up1(i)))
	kr = c>0 ? thish(up1(i), c) : 0;
if (kl== '=' && kr ==  '=') return(THRU);
if (kl== '=') return(LEFT);
if (kr== '=') return(RIGHT);
return(0);
}

int up1(int i)
{
i--;
while (instead[i] && i>0) i--;
return(i);
}
