#include "custom.h"

void rebin(int flag)
{

CELLBOXPTR ptr ;
TILEBOXPTR tileptr ;
int i , j , k , cell ;
int startx , endx , starty , endy , *occuptr ;
int lowBinX , highBinX , lowBinY , highBinY ;


for( i = 0 ; i <= numBinsX ; i++ ) {
    for( j = 0 ; j <= numBinsY ; j++ ) {
	for( k = 0 ; k <= 4 + numcells ; k++ ) {
	    blockarray[i][j][k] = 0 ;
	}
    }
}

for( cell = 1 ; cell <= numcells + numpads + 4 ; cell++ ) {
    if( cell > numcells && cell <= numcells + numpads ) {
	continue ;
    }
    ptr = cellarray[cell] ;
    tileptr = ptr->config[ptr->orient] ;

    startx = ptr->xcenter + tileptr->left ;
    endx   = ptr->xcenter + tileptr->right;
    starty = ptr->ycenter + tileptr->bottom ; 
    endy   = ptr->ycenter + tileptr->top;
    if( flag == 1 && cell <= numcells ) {
	startx -= tileptr->lborder ;
	endx   += tileptr->rborder ;
	starty -= tileptr->bborder ;
	endy   += tileptr->tborder ;
    }

    lowBinX = (startx - binOffsetX) / binWidthX ;
    if( lowBinX < 1 ) {
	lowBinX = 1 ;
    } else if( lowBinX > numBinsX ) {
	lowBinX = numBinsX ;
    }
    highBinX = (endx - binOffsetX) / binWidthX ;
    if( highBinX > numBinsX ) {
	highBinX = numBinsX ;
    } else if( highBinX < 1 ) {
	highBinX = 1 ;
    }
    if( lowBinX == highBinX ) {
	lowBinY = (starty - binOffsetY) / binWidthY ;
	if( lowBinY < 1 ) {
	    lowBinY = 1 ;
	} else if( lowBinY > numBinsY ) {
	    lowBinY = numBinsY ;
	}
	highBinY = (endy - binOffsetY) / binWidthY ;
	if( highBinY > numBinsY ) {
	    highBinY = numBinsY ;
	} else if( highBinY < 1 ) {
	    highBinY = 1 ;
	}
	if( lowBinY == highBinY ) {
	    occuptr = blockarray[lowBinX][lowBinY] ;
	} else {
	    occuptr = bucket ;
	}
    } else {
	occuptr = bucket ;
    }
    occuptr[ ++occuptr[0]] = cell ;
}
return ;
}
