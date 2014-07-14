#include "custom.h"

int goverlap( int cell , int xc , int yc , int orient , int b , int flag ,
               int borient )
{

CELLBOXPTR cellptr1 , cellptr2 ;
TILEBOXPTR tileptr1 , tileptr2 , tileptr01, tileptr02 ;
int startx01 , endx01 , starty01 , endy01 , othercell ;
int startx02 , endx02 , starty02 , endy02 , value , tempval ;
int startx1 , endx1 , starty1 , endy1 ;
int startx2 , endx2 , starty2 , endy2 ;
int startx , endx , starty , endy , xcenter , ycenter ;
int i , j , k , *occuptr ;
int lowBinX , highBinX , lowBinY , highBinY ;

value = 0 ;
cellptr1 = cellarray[cell] ;
tileptr01 = cellptr1->config[ orient ]  ;

startx01 = xc + tileptr01->left    ;
endx01   = xc + tileptr01->right   ;
starty01 = yc + tileptr01->bottom  ;
endy01   = yc + tileptr01->top     ;
if( cell <= numcells ) {
    startx01 -= tileptr01->lborder ;
    endx01   += tileptr01->rborder ;
    starty01 -= tileptr01->bborder ;
    endy01   += tileptr01->tborder ;
}
/*
 *   Hash these coordinates to find the possible overlaps
 *   Bins to check are: bucket + (lowBinX->highBinX, lowBinY->highBinY)
 */

lowBinX = (startx01 - binOffsetX) / binWidthX ;
if( lowBinX < 1 ) {
    lowBinX = 1 ;
} else if( lowBinX > numBinsX ) {
    lowBinX = numBinsX ;
}
highBinX = (endx01 - binOffsetX) / binWidthX ;
if( highBinX > numBinsX ) {
    highBinX = numBinsX ;
} else if( highBinX < 1 ) {
    highBinX = 1 ;
}
lowBinY = (starty01 - binOffsetY) / binWidthY ;
if( lowBinY < 1 ) {
    lowBinY = 1 ;
} else if( lowBinY > numBinsY ) {
    lowBinY = numBinsY ;
}
highBinY = (endy01 - binOffsetY) / binWidthY ;
if( highBinY > numBinsY ) {
    highBinY = numBinsY ;
} else if( highBinY < 1 ) {
    highBinY = 1 ;
}
if( lowBinX == highBinX && lowBinY == highBinY ) {
    binX = lowBinX ;
    binY = lowBinY ;
} else {
    binX = 0 ;
    binY = 0 ;
}

for( i = 0 ; i <= highBinX ; i++ ) {
    if( !(i == 0 || i >= lowBinX) ) {
	continue ;
    }
    for( j = 0 ; j <= highBinY ; j++ ) {
	if( !((i == 0 && j == 0) || (i > 0 && j >= lowBinY)) ) {
	    continue ;
	}
	occuptr = blockarray[i][j] ;
	for( k = 1 ; k <= occuptr[0] ; k++ ) {
	    othercell = occuptr[k] ;
	    if( othercell == cell ) {
		continue ;
	    }
	    cellptr2 = cellarray[othercell] ;
	    if( othercell != b ) {
		tileptr02 = cellptr2->config[ cellptr2->orient ] ;
		xcenter = cellptr2->xcenter ;
		ycenter = cellptr2->ycenter ;
	    } else if( flag == 0 ) {
		tileptr02 = cellptr2->config[borient] ;
		xcenter = cellptr1->xcenter ;
		ycenter = cellptr1->ycenter ;
	    } else {
		continue ;
	    }
	    startx02 = xcenter + tileptr02->left    ;
	    endx02   = xcenter + tileptr02->right   ;
	    starty02 = ycenter + tileptr02->bottom  ;
	    endy02   = ycenter + tileptr02->top     ;
	    if( othercell <= numcells ) {
		startx02 -= tileptr02->lborder ;
		endx02   += tileptr02->rborder ;
		starty02 -= tileptr02->bborder ; 
		endy02   += tileptr02->tborder ;
	    }
	    if( startx02 >= endx01 || startx01 >= endx02 || 
			starty02 >= endy01 || starty01 >= endy02 ){
		continue ;
	    }
	    if( cellptr1->numtiles == 1 && cellptr2->numtiles == 1 ) {
		startx = (startx01 >= startx02) ? startx01 : startx02 ;
		endx = (endx01 <= endx02) ? endx01 : endx02 ;
		starty = (starty01 >= starty02) ? starty01 : starty02 ;
		endy = (endy01 <= endy02) ? endy01 : endy02 ;
		value += (int)( lapFactor * (double)( (endy - starty) * 
					(endx - startx) + offset ) ) ;
		/*
		fprintf(fpo,"Overlap condition: cell:%d othercell:%d",
						cell, othercell ) ;
		fprintf(fpo,"   overlap:%d\n", value ) ;
		*/
	    } else {
		tempval = 0 ;
		for( tileptr1 = tileptr01->nexttile ;
				    tileptr1 != TILENULL ;
				    tileptr1 = tileptr1->nexttile ) {
		    startx1 =  xc + tileptr1->left   ;
		    endx1   =  xc + tileptr1->right  ;
		    starty1 =  yc + tileptr1->bottom ;
		    endy1   =  yc + tileptr1->top    ;
		    if( cell <= numcells ) {
			startx1 -= tileptr1->lborder ;
			endx1   += tileptr1->rborder ;
			starty1 -= tileptr1->bborder ;
			endy1   += tileptr1->tborder ;
		    }
		    for( tileptr2 = tileptr02->nexttile ; 
				    tileptr2 != TILENULL ;
				    tileptr2 = tileptr2->nexttile ) {
			startx2 =  xcenter + tileptr2->left  ;
			endx2   =  xcenter + tileptr2->right ;
			starty2 =  ycenter + tileptr2->bottom;
			endy2   =  ycenter + tileptr2->top   ;
			if( othercell <= numcells ) {
			    startx2 -= tileptr2->lborder ;
			    endx2   += tileptr2->rborder ;
			    starty2 -= tileptr2->bborder ;
			    endy2   += tileptr2->tborder ;
			}

			if( startx2 >= endx1 || startx1 >= endx2 || 
				 starty2 >= endy1 || starty1 >= endy2 ){
			    continue ;
			}
			startx = (startx1 >= startx2) ? 
						    startx1 : startx2 ;
			endx = (endx1 <= endx2) ? endx1 : endx2 ;
			starty = (starty1 >= starty2) ? 
						    starty1 : starty2 ;
			endy = (endy1 <= endy2) ? endy1 : endy2 ;
			tempval += (endy - starty) * (endx - startx) ;
		    }
		}
		if( tempval != 0 ) {
		    value += (int)( lapFactor * (double)
						( tempval + offset ) ) ;
		}
	    }
	}
    }
}
return( value ) ;
}
