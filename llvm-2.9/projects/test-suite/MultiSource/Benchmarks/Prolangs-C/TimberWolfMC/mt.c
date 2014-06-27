/*
Fast 2D Manhattan transformation package including
transformation inversion.
See Newman and Sproull's Principles of Interactive Computer
Graphics to understand it.
Employed heavily by the Squid DBMS.

Copyright Ken Keller 1981
*/

#include "port.h"
#include "kenk.h"
#include "lists.h"
#include "mt.h"

typedef enum {false,true} Bool;

void MTIdentity(MT *t);
void MTTranslate(MT *t,int x,int y);
static void MTInvert(MT *t);

MT *MTBegin(void)
/*
Returns pointer to transformation structure
that will be passed as an argument
to all of the package's routines.
If pointer is NULL, the structure can't be allocated.
MTBegin may be invoked any number of times.
*/
{
  MT *t;

  if((t = ALLOCNODE(MT)) == NULL)
    return(NULL);
  t->sp = 0;
  MTIdentity(t);
  return(t);
}

void MTEnd(MT *t)
/*
Deallocates transformation structure.
*/
{
  FREENODE(MT,t);
}

void MTIdentity(MT *t)
/*
Make current transformation the identity transformation.
*/
{
  t->ti[0][0] = t->ti[1][1] = t->ti[2][2] = 
    t->t[0][0] = t->t[1][1] = t->t[2][2] = 1;
  t->t[0][1] = t->t[1][0] = t->t[0][2] = t->t[1][2] = 
  t->t[2][0] = t->t[2][1] =
    t->ti[0][1] = t->ti[1][0] = t->ti[0][2] = t->ti[1][2] = 
    t->ti[2][0] = t->ti[2][1] = 0;
}

void MTTranslate(MT *t,int x,int y)
/*
Translate by (x,y). 
*/
{
  t->t[2][0] = t->t[2][0]+x;
  t->t[2][1] = t->t[2][1]+y;
  MTInvert(t);
}

void MTMY(MT *t)
/*
Mirror y coordinates. 
*/
{
  t->t[0][1] = -t->t[0][1];
  t->t[1][1] = -t->t[1][1];
  t->t[2][1] = -t->t[2][1];
  MTInvert(t);
}

void MTMX(MT *t)
/*
Mirror x coordinates.
*/
{
  t->t[0][0] = -t->t[0][0];
  t->t[1][0] = -t->t[1][0];
  t->t[2][0] = -t->t[2][0];
  MTInvert(t);
}

void MTRotate(MT *t,int x,int y)
/*
Rotate counter clockwise by direction vector (x,y).
Only 0, 90, 180, and 270 degrees have an effect.
*/
{
  register int i1;

  if(x == 0) {
    if(ABS(y) > 1)
      if(y < 0)
	y = -1;
      else y = 1; }
  ELIF(y == 0) {
    if(ABS(x) > 1)
      if(x < 0)
	x = -1;
      else x = 1; }
  if(x == 1 AND y == 0) /*Don't rotate at all.*/
    return; 
  ELIF(x == 0 AND y == -1) /*Rotate ccw by 270 degrees.*/ {
    i1 = t->t[0][0];
    t->t[0][0] = t->t[0][1];
    t->t[0][1] = -i1;
    i1 = t->t[1][0];
    t->t[1][0] = t->t[1][1];
    t->t[1][1] = -i1;
    i1 = t->t[2][0];
    t->t[2][0] = t->t[2][1];
    t->t[2][1] = -i1; }
  ELIF(x == 0 AND y == 1) /*Rotate ccw by 90 degrees.*/ {
    i1 = t->t[0][0];
    t->t[0][0] = -t->t[0][1];
    t->t[0][1] = i1;
    i1 = t->t[1][0];
    t->t[1][0] = -t->t[1][1];
    t->t[1][1] = i1;
    i1 = t->t[2][0];
    t->t[2][0] = -t->t[2][1];
    t->t[2][1] = i1; }
  ELIF(x == -1 AND y == 0) /*Rotate ccw by 180 degrees.*/ {
    register int i,j;

    for(i = 0;i < 3;++i)
      for(j = 0;j < 2;++j)
        t->t[i][j] = -t->t[i][j]; }
  MTInvert(t);
}

void MTConcat(MT *t,int a[3][3])
/*
Make current transformation the product of the
current transformation and a.
*/
{
  register int i1,i2,i3,i4,i5,i6;

  i1 = t->t[0][0]*a[0][0]+
    t->t[0][1]*a[1][0];
  i2 = t->t[0][0]*a[0][1]+
    t->t[0][1]*a[1][1];
  i3 = t->t[1][0]*a[0][0]+
    t->t[1][1]*a[1][0];
  i4 = t->t[1][0]*a[0][1]+
    t->t[1][1]*a[1][1];
  i5 = t->t[2][0]*a[0][0]+
    t->t[2][1]*a[1][0]+
    a[2][0];
  i6 = t->t[2][0]*a[0][1]+
    t->t[2][1]*a[1][1]+
    a[2][1];
  t->t[0][0] = i1;
  t->t[0][1] = i2;
  t->t[1][0] = i3;
  t->t[1][1] = i4;
  t->t[2][0] = i5;
  t->t[2][1] = i6;
  MTInvert(t);
}

void MTPoint(MT *t,int *x,int *y)
/*
MT (x,y) by current transformation.
*/
{
  int i1;

  i1 = *x*t->t[0][0]+*y*t->t[1][0]+
    t->t[2][0];
  *y = *x*t->t[0][1]+*y*t->t[1][1]+
    t->t[2][1];
  *x = i1;
}

void MTIPoint(MT *t,int *x,int *y)
/*
Invert (x,y).
*/
{
  int i1;

  i1 = *x*t->ti[0][0]+*y*t->ti[1][0]+
    t->ti[2][0];
  *y = *x*t->ti[0][1]+*y*t->ti[1][1]+
    t->ti[2][1];
  *x = i1;
}

Bool MTPushP(MT *t)
/*
Pushes current transformation on the transformation stack.
Current transformation is not changed.
Returns false if stack is full, else true.
*/
{
  register int i,j;

  if(t->sp == TSTKSIZE)
    return(false);
  for(i = 0;i < 3;++i)
    for(j = 0;j < 2;++j)
      t->stk[t->sp][i][j] = 
        t->t[i][j];
  ++t->sp;
  return(true);
}

Bool MTPopP(MT *t)
/*
Makes the top of the transformation stack the current transformation and
pops the stack.
Returns false if stack is empty, else true.
*/
{
  register int i,j;

  if(t->sp == 0)
    return(false);
  --t->sp;
  for(i = 0;i < 3;++i)
    for(j = 0;j < 2;++j)
      t->t[i][j] = t->stk[t->sp]
        [i][j];
  MTInvert(t);
  return(true);
}

Bool MTPremultiplyP(MT *t)
/*
Make the current transformation the matrix product of the
the current transformation and the top of the transformation stack.
Returns false if stack is empty.
*/
{
  register int i1,i2,i3,i4,i5,i6,sp;

  if(t->sp == 0)
    return(false);
  sp = t->sp-1;
  i1 = t->t[0][0]*t->stk[sp][0][0]+
    t->t[0][1]*t->stk[sp][1][0];
  i2 = t->t[0][0]*t->stk[sp][0][1]+
    t->t[0][1]*t->stk[sp][1][1];
  i3 = t->t[1][0]*t->stk[sp][0][0]+
    t->t[1][1]*t->stk[sp][1][0];
  i4 = t->t[1][0]*t->stk[sp][0][1]+
    t->t[1][1]*t->stk[sp][1][1];
  i5 = t->t[2][0]*t->stk[sp][0][0]+
    t->t[2][1]*t->stk[sp][1][0]+
    t->stk[sp][2][0];
  i6 = t->t[2][0]*t->stk[sp][0][1]+
    t->t[2][1]*t->stk[sp][1][1]+
    t->stk[sp][2][1];
  t->t[0][0] = i1;
  t->t[0][1] = i2;
  t->t[1][0] = i3;
  t->t[1][1] = i4;
  t->t[2][0] = i5;
  t->t[2][1] = i6;
  MTInvert(t);
  return(true);
}

Bool MTDecodeP(MT *t,char **s)
/*
Decodes current transformation into its "CIF equivalent" and
returns it in the string s.
Returns false if can't decode.
*/
{
  register int a,b,c,d,tx,ty;
  static char cif[81];

  if(NOT MTPushP(t))
    return(false);
  /*
  Let
  a c
  b d
  be the upper left corner of t->t.
  */
  a = t->t[0][0];
  b = t->t[1][0];
  c = t->t[0][1];
  d = t->t[1][1];
  tx = t->t[2][0];
  ty = t->t[2][1];
  MTIdentity(t);
  if(a == 0 AND b == 1 AND c == 1 AND d == 0) {
    MTMX(t);
    MTRotate(t,0,-1);
    MTTranslate(t,tx,ty);
    if(tx != 0 OR ty != 0) 
      sprintf(cif,"MX R 0 -1 T %d %d",tx,ty);
    else sprintf(cif,"MX R 0 -1"); }
  ELIF(a == 0 AND b == -1 AND c == -1 AND d == 0) {
    MTMX(t);
    MTRotate(t,0,1);
    MTTranslate(t,tx,ty);
    if(tx != 0 OR ty != 0) 
      sprintf(cif,"MX R 0 1 T %d %d",tx,ty);
    else sprintf(cif,"MX R 0 1"); }
  ELIF(a == 0 AND b == 1 AND c == -1 AND d == 0) {
    MTRotate(t,0,-1);
    MTTranslate(t,tx,ty);
    if(tx != 0 OR ty != 0) 
      sprintf(cif,"R 0 -1 T %d %d",tx,ty);
    else sprintf(cif,"R 0 -1"); }
  ELIF(a == 0 AND b == -1 AND c == 1 AND d == 0) {
    MTRotate(t,0,1);
    MTTranslate(t,tx,ty);
    if(tx != 0 OR ty != 0) 
      sprintf(cif,"R 0 1 T %d %d",tx,ty);
    else sprintf(cif,"R 0 1"); }
  ELIF(a == 1 AND b == 0 AND c == 0 AND d == 1) {
    MTTranslate(t,tx,ty);
    if(tx != 0 OR ty != 0) 
      sprintf(cif,"T %d %d",tx,ty); 
    else cif[0] = EOS;}
  ELIF(a == -1 AND b == 0 AND c == 0 AND d == -1) {
    MTRotate(t,-1,0);
    MTTranslate(t,tx,ty);
    if(tx != 0 OR ty != 0) 
      sprintf(cif,"R -1 0 T %d %d",tx,ty);
    else sprintf(cif,"R -1 0"); }
  ELIF(a == -1 AND b == 0 AND c == 0 AND d == 1) {
    MTMX(t);
    MTTranslate(t,tx,ty);
    if(tx != 0 OR ty != 0) 
      sprintf(cif,"MX T %d %d",tx,ty);
    else sprintf(cif,"MX"); }
  ELIF(a == 1 AND b == 0 AND c == 0 AND d == -1) {
    MTMY(t);
    MTTranslate(t,tx,ty);
    if(tx != 0 OR ty != 0) 
      sprintf(cif,"MY T %d %d",tx,ty);
    else sprintf(cif,"MY"); }
  else {
    MTPopP(t);
    return(false); }
  if(t->t[0][0] == a AND t->t[0][1] == c AND t->t[1][0] == b AND 
		t->t[1][1] == d) {
    MTPopP(t);
    *s = cif;
    return(true); }
  else {
    MTPopP(t);
    return(false); }
}

static void MTInvert(MT *t)
/*
From CSVAX:ouster  Thu Jun 25 15:26:13 1981
To: ESVAX:keller
Subject: MT ti

Caesar uses the following mechanism for inverting a transform, which
I have verified exhaustively to be correct for transforms with no
scaling and rotations that are multiples of 90 degrees.

If the original transform is
a d 0
b e 0
c f 1

then the inverse is

 1  0  0	 a b 0
 0  1  0  times  d e 0
-c -f  0	 0 0 0

Try this for all 8 possible combinations of mirroring and rotation to
convince yourself that it works.
					-John-

So, the inverse is
a b 0
d e 0
-ca-fd -cb-fe

KK 7/81
*/
{
  t->ti[0][0] = t->t[0][0]; /*a*/
  t->ti[0][1] = t->t[1][0]; /*d*/
  t->ti[1][0] = t->t[0][1]; /*b*/
  t->ti[1][1] = t->t[1][1]; /*e*/
  /*-ca-fd*/
  t->ti[2][0] = -t->t[2][0]*t->t[0][0]-t->t[2][1]*t->t[0][1];
  /*-cb-fe*/
  t->ti[2][1] = -t->t[2][0]*t->t[1][0]-t->t[2][1]*t->t[1][1];
  t->ti[0][2] = t->ti[1][2] = 0;
  t->ti[2][2] = 1;
}

#ifdef TEST1
int main(void)
{
  int selection;
  MT *t;
  char *cif;

  t = MTBegin();
  MTIdentity(t);
  LOOP {
    printf("\n");
    printf("1 Mirror in x.\n");
    printf("2 Mirror in y.\n");
    printf("3 Rotate by 90.\n");
    printf("4 Rotate by 180.\n");
    printf("5 Rotate by 270.\n");
    printf("6 Translate by a random amount.\n");
    printf("7 Decode.\n"); 
    printf("8 Is inverse correct?\n"); 
    printf("9 Current transform?\n"); 
    scanf("%d",&selection);
    printf("\n");
    switch(selection) {
      case 1:
	MTMX(t);
	break;
      case 2:
	MTMY(t);
	break;
      case 3:
	MTRotate(t,0,1);
	break;
      case 4:
	MTRotate(t,-1,0);
	break;
      case 5:
	MTRotate(t,0,-1);
	break;
      case 6:
	MTTranslate(t,33,17);
	break;
      case 7:
	if(MTDecodeP(t,&cif))
	  printf("Decoding is %s.\n",cif); 
	else printf("Couldn't decode.\n");
	break;
      case 8:
	MTPushP(t);
	/*Multiply t->t by t->ti.*/
	MTConcat(t,t->ti);
	/*Is the product the identity matrix?*/
	if(t->t[0][0] == 1 AND t->t[1][1] == 1 AND 
	  t->t[2][2] == 1 AND t->t[0][1] == 0 AND t->t[0][2] == 0
	  AND t->t[1][0] == 0 AND t->t[1][2] == 0 AND 
	  t->t[2][0] == 0 AND t->t[2][1] == 0)
	  printf("Yes.\n");
        else printf("No.\n");
	MTPopP(t);
	break;
      case 9: {
	int i,j;

	for(i = 0;i < 3;++i) {
	  for(j = 0;j < 3;++j)
	    printf("%d ",t->t[i][j]);
	  printf("\n"); } }
	break;
      default:
	printf("What?\n");
	break; } }
  return 0;
}
#endif
