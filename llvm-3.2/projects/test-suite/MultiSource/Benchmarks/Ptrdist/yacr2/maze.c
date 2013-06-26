
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "channel.h"
#include "assign.h"

#define min(a,b)	((a<b) ? a : b)
#define max(a,b)	((a<b) ? b : a)


/*
 *	plane allocation structures and routines
 */

#define FROM_LEFT	0x01
#define FROM_RIGHT	0x02
#define FROM_TOP	0x04
#define FROM_BOT	0x08

/* generic r/lvalue allocation map access macro */
#define ACCESS_MAP(a, x, y)	a[(y)*channelColumns + (x)]

static char * horzPlane;	/* horizontal plane allocation map */

/* r/lvalue for accessing horizontal plane allocation map */
#define HORZ(x,y)	ACCESS_MAP(horzPlane, x, y)

static char * vertPlane;	/* vertical plane allocation map */

/* r/lvalue for accessing vertical plane allocation map */
#define VERT(x,y)	ACCESS_MAP(vertPlane, x, y)

static char * viaPlane;		/* via plane allocation map */

/* r/lvalue for accessing via plane allocation map */
#define VIA(x,y)	ACCESS_MAP(viaPlane, x, y)

static char * mazeRoute;	/* true if the col needs to be maze routed */


/*
 *	set up the plane allocation maps, note: the channel
 *	dimensions must be set correctly when this routine
 *	is called
 */
void
InitAllocMaps(void)
{
    /* allocate maps */
    horzPlane = (char *)malloc((channelColumns+1)*(channelTracks+3));
    vertPlane = (char *)malloc((channelColumns+1)*(channelTracks+3));
    viaPlane = (char *)malloc((channelColumns+1)*(channelTracks+3));
    mazeRoute = (char *)malloc((channelColumns+1));


    /* if (!horzPlane || !vertPlane || !viaPlane || !mazeRoute) { */
    if (horzPlane==NULL || vertPlane==NULL ||
	viaPlane==NULL || mazeRoute==NULL) {
	fprintf(stderr, "unable to allocate plane allocation maps\n");
	exit(1);
    }

}

void
FreeAllocMaps(void)
{
    free(horzPlane);
    free(vertPlane);
    free(viaPlane);
    free(mazeRoute);
}


/*
 *	draw a segment, horizontal or vertical, from (x1,y1) to
 * 	(x2,y2) in the passed channel plane allocation map,
 *	note: the map entries are overwritten irreguardless of
 *	previous allocation, so check that the space is empty first,
 *	note: no spatial order is required on the two points, as
 *	they are sorted as needed by the line drawer
 */
void
DrawSegment(char * plane,
	    unsigned long x1, unsigned long y1,
	    unsigned long x2, unsigned long y2)
{
    unsigned long x, y;

    /* only horz or vert segments allowed */
    assert((x1 == x2) || (y1 == y2));

    /* must be a line */
    assert((x1 != x2) || (y1 != y2));

    if (x1 == x2) {	/* vertical */
	/* FROM_BOT at top end */
	/* assert(ACCESS_MAP(plane, x1, min(y1, y2)) == 0); */
	ACCESS_MAP(plane, x1, min(y1, y2)) |= FROM_BOT;

	/* FROM_TOP|FROM_BOT in the middle */
	for (y=min(y1, y2)+1; y<max(y1, y2); y++) {
	    /* assert((ACCESS_MAP(plane, x1, y)&(FROM_TOP|FROM_BOT)) == 0); */
	    ACCESS_MAP(plane, x1, y) |= FROM_TOP|FROM_BOT;
	}

	/* FROM_TOP at the bottom end */
	/* assert((ACCESS_MAP(plane, x1, max(y1, y2))&FROM_TOP) == 0); */
	ACCESS_MAP(plane, x1, max(y1, y2)) |= FROM_TOP;
	       
    }
    else { /* (y1 == y2), horizontal */
	/* FROM_RIGHT at left end */
	/* assert((ACCESS_MAP(plane, min(x1, x2), y1)&FROM_RIGHT) == 0); */
	ACCESS_MAP(plane, min(x1, x2), y1) |= FROM_RIGHT;

	/* FROM_LEFT|FROM_RIGHT in the middle */
	for (x=min(x1, x2)+1; x<max(x1, x2); x++) {
	    /* assert((ACCESS_MAP(plane,x,y1)&(FROM_LEFT|FROM_RIGHT)) == 0); */
	    ACCESS_MAP(plane, x, y1) = FROM_LEFT|FROM_RIGHT;
	}

	/* FROM_LEFT at the right end */
	/* assert((ACCESS_MAP(plane, max(x1, x2), y1)&FROM_LEFT) == 0);	*/
	ACCESS_MAP(plane, max(x1, x2), y1) |= FROM_LEFT;
    }
}


/*
 *	draw a via at (x, y)
 */
void
DrawVia(unsigned long x, unsigned long y)
{
    /* assert(ACCESS_MAP(viaPlane, x, y) == 0); */
    ACCESS_MAP(viaPlane, x, y) = 1;
}


/*
 *	is there a via at (x, y) ?
 */
int
HasVia(unsigned long x, unsigned long y)
{
    return(ACCESS_MAP(viaPlane, x, y));
}


/*
 *	return non-zero if the segment from (x1,y1) to (x2,y2)
 * 	in the plane allocation map, plane, note: no spatial order
 *	is required on the two points, as they are sorted as
 *	they are sorted as needed by the line drawer
 */
int
SegmentFree(char * plane,
	    unsigned long x1, unsigned long y1,
	    unsigned long x2, unsigned long y2)
{
    unsigned long x, y;
    unsigned long index;

    /* only horz or vert segments allowed */
    assert((x1 == x2) || (y1 == y2));

    if (x1 == x2) {	/* vertical */
	index = min(y1, y2)*channelColumns + x1;
	for (y=min(y1, y2); y<=max(y1, y2); y++, index += channelColumns) {
	    if (plane[index])
		return 0;
	}
    }
    else { /* (y1 == y2), horizontal */
	index = y1*channelColumns + min(x1,x2); 
	for (x=min(x1,x2); x<=max(x1,x2); x++, index++) {
	    if (plane[index])
		return 0;
	}
    }
    return 1;
}


/*
 *	print the channel, note: via crossovers are denoted with
 * 	a "X" and non-via crossovers with a "+"
 */
void
PrintChannel(void)
{
    unsigned long x, y;

    /* ms digit */
    printf("           ");
    for (x=1; x<=channelColumns; x++)
	printf( " %d ", TOP[x]/100);
    printf("\n");

    /* next ms digit */
    printf("           ");
    for (x=1; x<=channelColumns; x++)
	printf(" %d ", (TOP[x] - (TOP[x]/100)*100)/10);
    printf("\n");

    /* next ms digit */
    printf("           ");
    for (x=1; x<=channelColumns; x++)
	printf(" %d ", TOP[x] % 10);
    printf("\n");

    printf("           ");
    for (x=1; x<=channelColumns; x++)
	printf("%%%c%%", VERT(x,0) ? '|' : ' ');
    printf("\n");

    for (y=1; y<=channelTracks; y++) {

	printf("           ");
	for (x=1; x<=channelColumns; x++) {
	    if (VERT(x,y)&FROM_TOP)
		printf(" | ");
	    else
		printf("   ");
	}
	printf("\n");

	printf("Track %3d: ", y);
	for (x=1; x<=channelColumns; x++) {
	    if ((HORZ(x,y)&FROM_LEFT) && (VERT(x,y)&FROM_LEFT))
		printf("=");
	    else if (HORZ(x,y)&FROM_LEFT)
		printf("-");
	    else if (VERT(x,y)&FROM_LEFT)
		printf("^");
	    else
		printf(" ");

	    if (VIA(x,y))
		printf("X");
	    else if (HORZ(x,y) && VERT(x,y))
		printf("+");
	    else if (HORZ(x,y))
		printf("-");
	    else if (VERT(x,y))
		printf("|");
	    else
		printf(" ");


	    if ((HORZ(x,y)&FROM_RIGHT) && (VERT(x,y)&FROM_RIGHT))
		printf("=");
	    else if (HORZ(x,y)&FROM_RIGHT)
		printf("-");
	    else if (VERT(x,y)&FROM_RIGHT)
		printf("^");
	    else
		printf(" ");
	}
	printf("\n");

	printf("           ");
	for (x=1; x<=channelColumns; x++) {
	    if (VERT(x,y)&FROM_BOT)
		printf(" | ");
	    else
		printf("   ");
	}
	printf("\n");
    }

    printf("           ");
    for (x=1; x<=channelColumns; x++)
	printf("%%%c%%", VERT(x,channelTracks+1) ? '|' : ' ');
    printf("\n");

    /* ms digit */
    printf("           ");
    for (x=1; x<=channelColumns; x++)
	printf(" %d ", BOT[x]/100);
    printf("\n");

    /* next ms digit */
    printf("           ");
    for (x=1; x<=channelColumns; x++)
	printf(" %d ", (BOT[x] - (BOT[x]/100)*100)/10);
    printf("\n");

    /* next ms digit */
    printf("           ");
    for (x=1; x<=channelColumns; x++)
	printf(" %d ", BOT[x] % 10);
    printf("\n");

}


/*
 *	draw the assigned nets, except for any segments that need to be
 *	maze routed, this should be called just before maze routing
 *	starts, DrawSegment() will catch any overlapp errors, vias
 * 	are also drawn in this routine, and the mazeRoute flags
 *	are set, returns the total columns that need to be maze routed
 */
int
DrawNets(void)
{
    unsigned long i;
    int numLeft = 0;

    /* initialize maps to empty */
    bzero(horzPlane,
	  (int)((channelColumns+1)*(channelTracks+2)));
    bzero(vertPlane,
	  (int)((channelColumns+1)*(channelTracks+2)));
    bzero(viaPlane,
	  (int)((channelColumns+1)*(channelTracks+2)));
    bzero(mazeRoute,
	  (int)(channelColumns+1));

    /* draw all horizontal segments */
    for (i=1; i<=channelNets; i++) {
	if (FIRST[i] != LAST[i])
	    DrawSegment(horzPlane,
			FIRST[i], netsAssign[i],
			LAST[i], netsAssign[i]);
#ifdef VERBOSE
	printf("Just routed net %d...\n", i);
	PrintChannel();
#endif
    }

    /* draw all vertical segments that do not require maze routing */
    for (i=1; i<=channelColumns; i++) {
	if ((BOT[i] == 0) && (TOP[i] == 0)) {
	    /* do nothing */
	}
	else if ((BOT[i] == 0) && (TOP[i] != 0)) {
	    /* only one segment, therefore no vertical constraint violation */
	    DrawSegment(vertPlane,
			i, 0,
			i, netsAssign[TOP[i]]);
	    DrawVia(i, netsAssign[TOP[i]]);
	}
	else if ((TOP[i] == 0) && (BOT[i] != 0)) {
	    /* only one segment, therefore no vertical constraint violation */
	    DrawSegment(vertPlane,
			i, netsAssign[BOT[i]],
			i, channelTracks+1);
	    DrawVia(i, netsAssign[BOT[i]]);
	}
	/* two segments to route */
	else if ((TOP[i] == BOT[i]) && (FIRST[TOP[i]] == LAST[TOP[i]])) {
	    /* same net, no track needed to route */
	    assert((FIRST[TOP[i]] == i) && (LAST[TOP[i]] == i));
	    DrawSegment(vertPlane,
			i, 0,
			i, channelTracks+1);
	}
	else if (TOP[i] == BOT[i]) {
	    /* connecting to same track, therefore no vcv */
	    DrawSegment(vertPlane,
			i, 0,
			i, channelTracks+1);
	    DrawVia(i, netsAssign[BOT[i]]);
	}
	/* two segments to route, going to different tracks */
	else if (netsAssign[TOP[i]] < netsAssign[BOT[i]]) {
	    /* no vertical constraint violation */
	    DrawSegment(vertPlane,
			i, 0,
			i, netsAssign[TOP[i]]);
	    DrawVia(i, netsAssign[TOP[i]]);
	    DrawSegment(vertPlane,
			i, netsAssign[BOT[i]],
			i, channelTracks+1);
	    DrawVia(i, netsAssign[BOT[i]]);
	}
	/* otherwise, maze routing is required */
	else {
	    assert(netsAssign[TOP[i]] > netsAssign[BOT[i]]);
	    mazeRoute[i] = 1;
	    numLeft++;
	}
#ifdef VERBOSE
	if (!mazeRoute[i])
	    printf("Just routed vertical column %d...\n", i);
	else
	    printf("VCV in vertical column %d... will maze later.\n", i);

	PrintChannel();
#endif
    }
    return numLeft;
}


/*
 * clean net "net", that is, check if it is complete, and if so
 * trim the ends of any un-needed horizontal net segments, this does
 * not need to be too efficient as it is only called when a colunm
 * is mazed
 */
static void
CleanNet(unsigned long net)
{
    unsigned long i;
    unsigned long firstVia, lastVia;
    unsigned long effFIRST, effLAST;
    unsigned long track;

    /* is this net finished */
    for (i=FIRST[net]; i<=LAST[net]; i++) {
	if (((TOP[i] == net) || (BOT[i] == net)) && (mazeRoute[i]))
	    return;	/* not done, leave it alone */
    }

    track = netsAssign[net];

    /* find effective FIRST and LAST */
    effFIRST = FIRST[net];
    while (ACCESS_MAP(horzPlane, effFIRST, track)&FROM_LEFT)
	effFIRST--;

    effLAST = LAST[net];
    while (ACCESS_MAP(horzPlane, effLAST, track)&FROM_RIGHT)
	effLAST++;

    /* net is finished */
    firstVia = 9999999;
    lastVia = 0;
    for (i=effFIRST; i<=effLAST; i++) {
	if (HasVia(i, track)) {
	    if (i < firstVia)
		firstVia = i;
	    if (i > lastVia)
		lastVia = i;
	}
    }
    if (effFIRST < firstVia) {
	/* clean up the segment */
	for (i=effFIRST; i<firstVia; i++) {
	    ACCESS_MAP(horzPlane, i, track) = 0;
	}
	/* and the left edge at the via */
	ACCESS_MAP(horzPlane, firstVia, track) &= ~FROM_LEFT;
    }
    if (lastVia < effLAST) {
	/* get the right edge */
	ACCESS_MAP(horzPlane, lastVia, track) &= ~FROM_RIGHT;
	/* clean up the segment */
	for (i=lastVia+1; i<= effLAST; i++) {
	    ACCESS_MAP(horzPlane, i, track) = 0;
	}
    }
}


/*
 * return non-zero if there is a vertical constraint violation in col "i"
 */
static int
HasVCV(unsigned long i)
{
    return ((TOP[i] != 0) &&
	    (BOT[i] != 0) &&
	    (TOP[i] != BOT[i]) &&
	    (netsAssign[TOP[i]] > netsAssign[BOT[i]]));
}


/*
 *	maze1 route the channel, return the number of channels,
 *	with vertical constraint violations, that could not be maze1
 *	routed
 */
static int
Maze1Mech(unsigned long i,		/* column */
	  unsigned long s1,		/* straight segment from s1 to s2 */
	  unsigned long s2,		/* bend is next to s2 */
	  unsigned long b1,		/* bent channel from b1 to b2 */
	  unsigned long b2,		/* s1, b1 are at the terminals */
	  int bXdelta, int bYdelta)	/* bend X, Y delta from s */
{
    if (SegmentFree(vertPlane,		/* straight vert seg in col i */
		    i, s1,
		    i, s2) &&
	SegmentFree(vertPlane,		/* bent vert seg in col i */
		    i, b1,
		    i, s2+bYdelta) &&
	SegmentFree(vertPlane,		/* bent jog in vert plane */
		    i, s2+bYdelta,
		    i+bXdelta, s2+bYdelta) &&
	SegmentFree(vertPlane,		/* run along segment in vert plane */
		    i+bXdelta, s2+bYdelta,
		    i+bXdelta, b2) &&
	!HasVCV(i+bXdelta)) {

	DrawSegment(vertPlane,		/* straight vert seg in col i */
		    i, s1,
		    i, s2);
	DrawVia(i, s2);			/* via down to horz plane */
	DrawSegment(vertPlane,		/* bent vert seg in col i */
		    i, b1,
		    i, s2+bYdelta);
	DrawSegment(vertPlane,		/* bent jog in vert plane */
		    i, s2+bYdelta,
		    i+bXdelta, s2+bYdelta);
	DrawSegment(vertPlane,		/* run along segment in vert plane */
		    i+bXdelta, s2+bYdelta,
		    i+bXdelta, b2);
	DrawVia(i+bXdelta, b2);		/* via down to horz plane */
	DrawSegment(horzPlane,		/* possibly extend horz seg */
		    i+bXdelta, b2,
		    i, b2);
	return 1;
    }
    return 0;
}


int
Maze1(void)
{
    int numLeft = 0;
    unsigned long p, s;
    unsigned long i;

    for (i=1; i<=channelColumns; i++) {
	if (mazeRoute[i]) {

	    s = netsAssign[TOP[i]];
	    p = netsAssign[BOT[i]];
	    assert(s > p);

#ifdef TODD
	    if ((i > 1) && (p > 1) &&
		Maze1Mech(i, 
			  channelTracks+1, p,			/* straight */
			  0, s,					/* bent */
			  -1, -1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else if ((i < channelColumns) && (p > 1) &&
		     Maze1Mech(i,
			       channelTracks+1, p,		/* straight */
			       0, s,				/* bent */
			       1, -1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
#else
	    if ((i < channelColumns) && (p > 1) &&
		     Maze1Mech(i,
			       channelTracks+1, p,		/* straight */
			       0, s,				/* bent */
			       1, -1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else if ((i > 1) && (p > 1) &&
		Maze1Mech(i, 
			  channelTracks+1, p,			/* straight */
			  0, s,					/* bent */
			  -1, -1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
#endif
#ifdef TODD
	    else if ((i > 1) && (s < channelTracks) &&
		     Maze1Mech(i,
			       0, s,				/* straight */
			       channelTracks+1, p,		/* bent */
			       -1, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else if ((i < channelColumns) && (s < channelTracks) &&
		     Maze1Mech(i,
			       0, s,				/* straight */
			       channelTracks+1, p,		/* bent */
			       1, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
#else
	    else if ((i < channelColumns) && (s < channelTracks) &&
		     Maze1Mech(i,
			       0, s,				/* straight */
			       channelTracks+1, p,		/* bent */
			       1, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else if ((i > 1) && (s < channelTracks) &&
		     Maze1Mech(i,
			       0, s,				/* straight */
			       channelTracks+1, p,		/* bent */
			       -1, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
#endif
	    else {
		/* could not maze1 route this column */
		numLeft++;
	    }
#ifdef VERBOSE
	    if (!mazeRoute[i]) {
		/* got one */
		printf("Maze1 routed vertical column %d...\n", i);
		PrintChannel();
	    }
	    else
		printf("Maze1 could not route vertical column %d...\n", i);
#endif
	}
    }
    return numLeft;
}


/*
 *	maze2 route the channel, return the number of channels,
 *	with vertical constraint violations, that could not be maze1
 *	routed
 */


/*
 * can this track be extended to the range specified, return result
 */
int
ExtendOK(unsigned long net, char * plane,
	 unsigned long _x1, unsigned long _y1,	/* start seg */
	 unsigned long _x2, unsigned long _y2)	/* end seg */
{
    unsigned long x1, y1, x2, y2;

    /* sort, (x1,y1) => (x2,y2) */
    x1 = min(_x1, _x2);
    y1 = min(_y1, _y2);
    x2 = max(_x1, _x2);
    y2 = max(_y1, _y2);

    assert((y1 == y2) && (netsAssign[net] == y1));

    if ((x1 >= FIRST[net]) && (x2 <= LAST[net]))
	return 1;	/* inside the net */
    if ((x1 < FIRST[net]) && (x2 > LAST[net])) {
	/* subsumes */
	return (SegmentFree(plane,
			    x1, y1,
			    FIRST[net]-1, y1) &&
		SegmentFree(plane,
			    LAST[net]+1, y1,
			    x2, y1));
    }
    else if (x1 < FIRST[net]) {
	/* to the left possibly overlapping */
	return SegmentFree(plane,
			   x1, y1,
			   FIRST[net]-1, y1);
    }
    else if (x2 > LAST[net]) {
	/* to the right possibly overlapping */
	return SegmentFree(plane,
			   LAST[net]+1, y1,
			   x2, y1);
    }
    /* should not get here */
    abort();
    return 0;
}


static int
Maze2Mech(unsigned long bentNet,	/* net to bend */
	  unsigned long i,		/* column */
	  unsigned long s1,		/* straight segment from s1 to s2 */
	  unsigned long s2,		/* bend is next to s2 */
	  unsigned long b1,		/* bent channel from b1 to b2 */
	  unsigned long b2,		/* s1, b1 are at the terminals */
	  unsigned long xStart,		/* cols to search across */
	  unsigned long xEnd,
	  int bXdelta,			/* direction of bend horz seg */
	  unsigned long yStart,         /* rows to search across */
          unsigned long yEnd,
          int bYdelta)                  /* direction of bend vert seg */
{
    unsigned long row, col;
    int colFree;

    xEnd += bXdelta;	/* so I can use != */
    yEnd += bYdelta;

    for (row = yStart; row != yEnd; row += bYdelta) {	/* search for row */
	colFree = 1;
	for (col = xStart;
	     colFree && (col != xEnd);
	     col += bXdelta) { /* search for col */
	    if ((colFree = SegmentFree(horzPlane,	/* bent horz seg */
			    i, row,
			    col, row)) &&
		SegmentFree(vertPlane,		/* straight seg in vert */
			    i, s1,
			    i, s2) &&
		SegmentFree(vertPlane,		/* bent to bend in vert */
			    i, b1,
			    i, row) &&
		SegmentFree(vertPlane,		/* bent to net */
			    col, row,
			    col, b2-1) &&
		!HasVCV(col) &&
		ExtendOK(bentNet, horzPlane,
			 col, b2,
			 i, b2)) {
		/* draw it! */
		DrawSegment(vertPlane,		/* straight seg in vert */
			    i, s1,
			    i, s2);
		DrawVia(i, s2);			/* conn to horz */

		DrawSegment(vertPlane,		/* bent to bend in vert */
			    i, b1,
			    i, row);
		DrawVia(i, row);		/* conn to horz plane */
		DrawSegment(horzPlane,		/* bent horz seg */
			    i, row,
			    col, row);
		DrawVia(col, row);		/* conn to vert plane */
		DrawSegment(vertPlane,		/* bent to net */
			    col, row,
			    col, b2);
		DrawVia(col, b2);		/* back to horz plane */
		DrawSegment(horzPlane,		/* possibly extend net */
			    col, b2,
			    i, b2);
		return 1;
	    }
	}
    }
    return 0;
}


int
Maze2(void)
{
    int numLeft = 0;
    unsigned long p, s;
    unsigned long i;

    for (i=1; i<=channelColumns; i++) {
	if (mazeRoute[i]) {

	    s = netsAssign[TOP[i]];
	    p = netsAssign[BOT[i]];
	    assert(s > p);

#ifdef TODD
	    if ((i > 1) && (p > 1) &&
		Maze2Mech(TOP[i], i, 
			  channelTracks+1, p,			/* straight */
			  0, s,					/* bent */
			  i-1, 1, -1,				/* left srch */
			  1, p-1, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else if ((i < channelColumns) && (p > 1) &&
		     Maze2Mech(TOP[i], i,
			       channelTracks+1, p,		/* straight */
			       0, s,				/* bent */
			       i+1, channelColumns, 1,		/* rght srch */
			       1, p-1, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
#else
	    if ((i < channelColumns) && (p > 1) &&
		     Maze2Mech(TOP[i], i,
			       channelTracks+1, p,		/* straight */
			       0, s,				/* bent */
			       i+1, channelColumns, 1,		/* rght srch */
			       1, p-1, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else if ((i > 1) && (p > 1) &&
		Maze2Mech(TOP[i], i, 
			  channelTracks+1, p,			/* straight */
			  0, s,					/* bent */
			  i-1, 1, -1,				/* left srch */
			  1, p-1, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
#endif 
#ifdef TODD
	    else if ((i > 1) && (s < channelTracks) &&
		     Maze2Mech(BOT[i], i,
			       0, s,				/* straight */
			       channelTracks+1, p,		/* bent */
			       i-1, 1, -1,			/* left srch */
			       s+1, channelTracks, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else if ((i < channelColumns) && (s < channelTracks) &&
		     Maze2Mech(BOT[i], i,
			       0, s,				/* straight */
			       channelTracks+1, p,		/* bent */
			       i+1, channelColumns, 1,		/* rght srch */
			       s+1, channelTracks, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
#else
	    else if ((i < channelColumns) && (s < channelTracks) &&
		     Maze2Mech(BOT[i], i,
			       0, s,				/* straight */
			       channelTracks+1, p,		/* bent */
			       i+1, channelColumns, 1,		/* rght srch */
			       s+1, channelTracks, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else if ((i > 1) && (s < channelTracks) &&
		     Maze2Mech(BOT[i], i,
			       0, s,				/* straight */
			       channelTracks+1, p,		/* bent */
			       i-1, 1, -1,			/* left srch */
			       s+1, channelTracks, 1)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
#endif
	    else {
		/* could not maze2 route this column */
		numLeft++;
	    }
#ifdef VERBOSE
	    if (!mazeRoute[i]) {
		/* got one */
		printf("Maze2 routed vertical column %d...\n", i);
		PrintChannel();
	    }
	    else
		printf("Maze2 could not route vertical column %d...\n", i);
#endif
	}
    }
    return numLeft;
}


void
FindFreeHorzSeg(unsigned long startCol, unsigned long row,
		unsigned long * rowStart, unsigned long * rowEnd)
{
    unsigned long i;

    for (i=startCol; i >= 1; i--) {
	if (ACCESS_MAP(horzPlane, i, row))
	    break;
    }
    *rowStart = i+1;

    for (i=startCol; i <= channelColumns; i++) {
	if (ACCESS_MAP(horzPlane, i, row))
	    break;
    }
    *rowEnd = i-1;
}

static int
Maze3Mech(unsigned long topNet,		/* top net to bend */
	  unsigned long botNet,		/* bottom net to bend */
	  unsigned long i,		/* column */
	  unsigned long s1,		/* top segment from s1 to s2 */
	  unsigned long s2,		/* bend is next to s2 */
	  unsigned long b1,		/* bottom segment from b1 to b2 */
	  unsigned long b2)		/* s1, b1 are at the terminals */
{
    unsigned long topRow, topCol, botRow, botCol;
    unsigned long topStart, topEnd, botStart, botEnd;
    ;

    for (topRow = b2+1; topRow < s2-1; topRow++) {
	FindFreeHorzSeg(i, topRow, &topStart, &topEnd);
	if (topEnd <= topStart)
	    continue;
	for (botRow = topRow+1; botRow < s2; botRow++) {
	    FindFreeHorzSeg(i, botRow, &botStart, &botEnd);
	    if (botEnd <= botStart)
		continue;
	    for (topCol = topStart; topCol <= topEnd; topCol++) {
		for (botCol = botStart; botCol <= botEnd; botCol++) {
		    if ((topCol != i) && (botCol != i) &&
			(topRow != botRow) && (topCol != botCol) &&
			SegmentFree(vertPlane,	/* top down */
				    i, s1,
				    i, topRow) &&
			SegmentFree(horzPlane,	/* over to drop point */
				    i, topRow,
				    topCol, topRow) &&
			SegmentFree(vertPlane,	/* down to net */
				    topCol, topRow,
				    topCol, s2+1) &&
			!HasVCV(topCol) &&
			ExtendOK(topNet, horzPlane,
				 topCol, s2,
				 i, s2) &&
			SegmentFree(vertPlane,  /* bot up */
				    i, b1,
				    i, botRow) &&
			SegmentFree(horzPlane,  /* over to up point */
				    i, botRow,
				    botCol, botRow) &&
			SegmentFree(vertPlane,  /* up to net */
				    botCol, botRow,
				    botCol, b2-1) &&
			!HasVCV(botCol) &&
			ExtendOK(botNet, horzPlane,
				 botCol, b2,
				 i, b2)) {
			/* draw it! */
			DrawSegment(vertPlane,  /* top down */
				    i, s1,
				    i, topRow);
			DrawVia(i, topRow);	/* via to horz */
			DrawSegment(horzPlane,  /* over to drop point */
				    i, topRow,
				    topCol, topRow);
			DrawVia(topCol, topRow);	/* up to vert */
			DrawSegment(vertPlane,  /* down to net */
				    topCol, topRow,
				    topCol, s2);
			DrawVia(topCol, s2);	/* via to net */
			DrawSegment(horzPlane,	/* conn to net */
				    topCol, s2,
				    i, s2);

			DrawSegment(vertPlane, /* bot up */
				    i, b1,
				    i, botRow);
			DrawVia(i, botRow);	/* via to horz */
			DrawSegment(horzPlane,  /* over to up point */
				    i, botRow,
				    botCol, botRow);
			DrawVia(botCol, botRow);	/* via to vert */
			DrawSegment(vertPlane,  /* up to net */
				    botCol, botRow,
				    botCol, b2);
			DrawVia(botCol, b2);	/* via to net */
			DrawSegment(horzPlane,	/* conn to net */
				    botCol, b2,
				    i, b2);
    			;
			return 1;
		    }
		}
	    }
	}
    }
    ;
    return 0;
}


int Maze3(void)
{
    int numLeft = 0;
    unsigned long p, s;
    unsigned long i;

    for (i=1; i<=channelColumns; i++) {
	if (mazeRoute[i]) {

	    s = netsAssign[TOP[i]];
	    p = netsAssign[BOT[i]];
	    assert(s > p);

	    if (Maze3Mech(TOP[i], BOT[i], i,
			   0, s,
			   channelTracks+1, p)) {
		mazeRoute[i] = 0;
		CleanNet(TOP[i]);
		CleanNet(BOT[i]);
	    }
	    else {
		/* could not maze2 route this column */
		numLeft++;
	    }
#ifdef VERBOSE
	    if (!mazeRoute[i]) {
		/* got one */
		printf("Maze3 routed vertical column %d...\n", i);
		PrintChannel();
	    }
	    else
		printf("Maze3 could not route vertical column %d...\n", i);
#endif
	}
    }
    return numLeft;
}
