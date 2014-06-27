#include "custom.h"
extern double chipaspect ;
extern int perim ;
extern int totChanLen ;
extern int totNetLen  ;
extern int core_expansion_given ;
extern double core_expansion ;
extern int wire_est_factor ;
double expandExtra ;
extern int defaultTracks ;

extern void placepads(void);
extern void loadbins(int new);

void config2(void)
{

CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
int l , r , b , t , sum , cell , tempint ;
double totNewArea , totalArea , temp , dub , reduction_factor ;


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
	for( ; tileptr != TILENULL ; tileptr = tileptr->nexttile ) {
	    l = tileptr->left   ;
	    r = tileptr->right  ;
	    b = tileptr->bottom ;
	    t = tileptr->top    ;
	    totalArea += (r - l) * (t - b) ;
	}
    }
}
if( coreGiven == 0 ) {
    blockr = blockt = (int) sqrt( totalArea ) ;
    totChanLen = perim / 2 - (blockr + blockt) ;
    temp = ((double) totNetLen / (double) totChanLen) /
		((double) layersFactor) ; 
} else {
    r = t = (int) sqrt( totalArea ) ;
    totChanLen = perim / 2 - (r + t) ;
    temp = ((double) totNetLen / (double) totChanLen) /
		((double) layersFactor) ; 
}
temp += (double) defaultTracks ;
tempint = (int)( temp ) + 3 ; /* d+1 tracks + roundoff */
tempint *= trackspacing ;

reduction_factor = 1.8 + (double) wire_est_factor / 10.0 ;
aveChanWid = (int)( (double) tempint / reduction_factor ) ;

aveChanWid += 2 ;
fprintf(fpo,"\n\nConfiguration Data\nInternal Channel Length:%d\n",
							totChanLen ) ;
fprintf(fpo,"Average Channel Width (un-normalized):%d\n\n", tempint );
fprintf(fpo,"Average Channel Width:%d\n\n", aveChanWid ) ;

if( coreGiven == 0 ) {
    /*
     *   Compute total new area
     */

    totNewArea = 0 ;
    for( cell = 1 ; cell <= numcells ; cell++ ) {
	cellptr = cellarray[cell] ;
	if( cellptr->numtiles == 1 ) {
	    tileptr = cellptr->config[cellptr->orient] ;
	    l = tileptr->left   ;
	    r = tileptr->right  ;
	    b = tileptr->bottom ;
	    t = tileptr->top    ;
	    totNewArea += (r - l + maxWeight * maxWeight * aveChanWid) *
			  (t - b + maxWeight * maxWeight * aveChanWid) ;
	} else {
	    sum = 0 ;
	    tileptr = cellptr->config[cellptr->orient]->nexttile ;
	    for( ; tileptr != TILENULL ; tileptr = tileptr->nexttile ) {
		l = tileptr->left   ;
		r = tileptr->right  ;
		b = tileptr->bottom ;
		t = tileptr->top    ;
		sum += (r - l) * (t - b) ;
	    }
	    tileptr = cellptr->config[cellptr->orient] ;
	    l = tileptr->left   ;
	    r = tileptr->right  ;
	    b = tileptr->bottom ;
	    t = tileptr->top    ;
	    totNewArea += (r - l + maxWeight * maxWeight * aveChanWid) * 
			  (t - b + maxWeight * maxWeight * aveChanWid) -
			  (r - l) * (t - b) + sum ;
	}
    }
    /*
    expandExtra = ( 117.5 - 5.0 * ( ((double) aveChanWid) / 
			 ((double) trackspacing) ) ) / 100.0 ;
    if( expandExtra < 1.05 ) {
	expandExtra = 1.05 ;
    } 
    */
    expandExtra = 1.05 ;

    if( numcells < 10 ) {
	expandExtra += (double) (10 - numcells) * 0.01 ;
	if( expandExtra > 1.10 ) {
	    expandExtra = 1.10 ;
	}
    }
    
    blockr = blockt = (int)( expandExtra * sqrt( totNewArea ) ) + 1 ;
    fprintf(fpo,"Core Expansion Factor: %f\n", expandExtra ) ;

    dub = log10( (double) blockr / sqrt( (double) numcells ) ) ;
    if( (!core_expansion_given) && dub >= 3.0 ) {
	expandExtra += 0.02 + (0.04 * (dub - 3.0)) ;
	if( expandExtra > 1.10 ) {
	    expandExtra = 1.10 ;
	}
	blockr = blockt = (int)( expandExtra * sqrt( totNewArea ) ) + 1 ;
	fprintf(fpo,"Core Expansion Factor: %f\n", expandExtra ) ;
    }
    if( core_expansion_given ) {
	expandExtra = core_expansion ;
	blockr = blockt = (int)( expandExtra * sqrt( totNewArea ) ) + 1 ;
	fprintf(fpo,"Core Expansion Factor: %f\n", expandExtra ) ;
    }
	
    

    /* 
     *    Take into account the aspect ratio requested by the user
     */
    blockt = (int)( sqrt(chipaspect) * (double) blockt ) + 1 ;
    blockr = (int)( 1.0 / sqrt(chipaspect) * (double) blockr ) + 1 ;
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

loadbins(0) ;

return ;
}
