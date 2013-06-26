#ifndef lint
static char sccsid[] = "@(#)t2.c	4.2 8/11/83";
#endif

 /* t2.c:  subroutine sequencing for one table */
# include "t..c"

extern void saveline(void);
extern void savefill(void);
extern void ifdivert(void);
extern void cleanfc(void);
extern void getcomm(void);
extern void getspec(void);
extern void gettbl(void);
extern void getstop(void);
extern void checkuse(void);
extern void choochar(void);
extern void maktab(void);
extern void runout(void);
extern void release(void);
extern void rstofill(void);
extern void endoff(void);
extern void restline(void);

void tableput(void)
{
saveline();
savefill();
ifdivert();
cleanfc();
getcomm();
getspec();
gettbl();
getstop();
checkuse();
choochar();
maktab();
runout();
release();
rstofill();
endoff();
restline();
}
