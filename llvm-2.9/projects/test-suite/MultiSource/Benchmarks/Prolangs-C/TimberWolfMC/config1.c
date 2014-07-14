#include "custom.h"
extern int perim ;
int totChanLen ;
int totNetLen  ;

extern void placepads(void);
extern void loadbins(int new);

void config1(void)
{

CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
int l , r , b , t , totalArea , cell ;


/*
 *   Sum the areas of the cells
 */
totalArea = 0 ;
for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[cell] ;
    if( cellptr->numtiles == 1 ) {
	tileptr = cellptr->config[cellptr->orient] ;
	l = tileptr->left   ;
	r = tileptr->right  ;
	b = tileptr->bottom ;
	t = tileptr->top    ;
	totalArea += (r - l) * (t - b) ;
    } else {
	tileptr = cellptr->config[cellptr->orient]->nexttile ;
	for( ; tileptr != TILENULL; tileptr = tileptr->nexttile ){
	    l = tileptr->left   ;
	    r = tileptr->right  ;
	    b = tileptr->bottom ;
	    t = tileptr->top    ;
	    totalArea += (r - l) * (t - b) ;
	}
    }
}
if( coreGiven == 0 ) {
    blockr = blockt = (int) sqrt( (double) totalArea ) + 1 ;
    totChanLen = perim / 2 - (blockr + blockt) ;
    aveChanWid = 0 ;
} else {
    r = t = (int) sqrt( (double) totalArea ) + 1 ;
    totChanLen = perim / 2 - (r + t) ;
    aveChanWid = 0 ;
}

slopeX = (double)(maxWeight - baseWeight) / ( (double) blockr * 0.5 ) ;
slopeY = (double)(maxWeight - baseWeight) / ( (double) blockt * 0.5 ) ;
basefactor = (double) baseWeight ;

placepads() ;

blockmx = (blockr + blockl) / 2 ;
blockmy = (blockt + blockb) / 2 ;

binWidthX = (blockr - blockl) / numBinsX ;
if( (blockr - blockl - binWidthX * numBinsX) >= numBinsX / 2 ) {
    binWidthX++ ;
}
binOffsetX = blockl + 1 - binWidthX ;

binWidthY = (blockt - blockb) / numBinsY ;
if( (blockt - blockb - binWidthY * numBinsY) >= numBinsY / 2 ) {
    binWidthY++ ;
}
binOffsetY = blockb + 1 - binWidthY ;

loadbins(1) ;

return ;
}
