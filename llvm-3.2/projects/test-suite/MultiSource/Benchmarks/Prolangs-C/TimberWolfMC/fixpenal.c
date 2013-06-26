#include "custom.h"
#define L 1
#define T 2
#define R 3
#define B 4
int dx , dy ;

int whoOverlaps( int cell , int xc , int yc , int orient , int b , 
		int flag , int borient );
void padOverlaps(void);

void fixpenal(void)
{

int cell , other , d , bx , by , d1 , d2 ;
int problems ;

bx = (blockr + blockl) / 2 ;
by = (blockt + blockb) / 2 ;

problems = 1 ;
while( problems ) {
    problems = 0 ;
    for( cell = 1 ; cell <= numcells ; cell++ ) {
	other = whoOverlaps( cell, cellarray[cell]->xcenter, 
				  cellarray[cell]->ycenter,
				  cellarray[cell]->orient , 0 , 1 , 0 ) ;
	if( other > 0 ) {
	    d1 = (int) sqrt((double)((ABS(cellarray[cell]->xcenter - bx) *
			      ABS(cellarray[cell]->xcenter - bx)) +
			     (ABS(cellarray[cell]->ycenter - by) *
			      ABS(cellarray[cell]->ycenter - by)) ) ) ;
	    d2 = (int) sqrt((double)((ABS(cellarray[other]->xcenter - bx) *
			      ABS(cellarray[other]->xcenter - bx)) +
			     (ABS(cellarray[other]->ycenter - by) *
			      ABS(cellarray[other]->ycenter - by)) ) ) ;
	    if( dx == 0 ) {
		dx = 1000000 ;
	    }
	    if( dy == 0 ) {
		dy = 1000000 ;
	    }
	    d = (dx <= dy) ? dx : dy ;
	    if( d == dx ) {
		if( d1 > d2 ) {
		    if( cellarray[cell]->xcenter > bx ) {
			cellarray[cell]->xcenter += d ;
		    } else {
			cellarray[cell]->xcenter -= d ;
		    }
		} else {
		    if( cellarray[other]->xcenter > bx ) {
			cellarray[other]->xcenter += d ;
		    } else {
			cellarray[other]->xcenter -= d ;
		    }
		}
	    } else {
		if( d1 > d2 ) {
		    if( cellarray[cell]->ycenter > by ) {
			cellarray[cell]->ycenter += d ;
		    } else {
			cellarray[cell]->ycenter -= d ;
		    }
		} else {
		    if( cellarray[other]->ycenter > by ) {
			cellarray[other]->ycenter += d ;
		    } else {
			cellarray[other]->ycenter -= d ;
		    }
		}
	    }
	    cell-- ;
	    problems = 1 ;
	}
    }
}
padOverlaps() ;

return ;
}




int whoOverlaps( int cell , int xc , int yc , int orient , int b , 
                  int flag , int borient )
{

CELLBOXPTR cellptr1 , cellptr2 ;
TILEBOXPTR tileptr1 , tileptr2 , tileptr01, tileptr02 ;
int startx01 , endx01 , starty01 , endy01 , othercell ;
int startx02 , endx02 , starty02 , endy02 ;
int startx1 , endx1 , starty1 , endy1 ;
int startx2 , endx2 , starty2 , endy2 ;
int startx , endx , starty , endy , xcenter , ycenter ;
int i , j , k , *occuptr ;
int lowBinX , highBinX , lowBinY , highBinY ;

dx = 0 ;
dy = 0 ;

cellptr1 = cellarray[cell] ;
tileptr01 = cellptr1->config[ orient ]  ;

startx01 = xc + tileptr01->left    ;
endx01   = xc + tileptr01->right   ;
starty01 = yc + tileptr01->bottom  ;
endy01   = yc + tileptr01->top     ;
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
	    if( othercell == cell || othercell > numcells ) {
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
	    
	    if( startx02 >= endx01 || startx01 >= endx02 || 
			starty02 >= endy01 || starty01 >= endy02 ){
		continue ;
	    }
	    if( cellptr1->numtiles == 1 && cellptr2->numtiles == 1 ) {
		startx = (startx01 >= startx02) ? startx01 : startx02 ;
		endx = (endx01 <= endx02) ? endx01 : endx02 ;
		starty = (starty01 >= starty02) ? starty01 : starty02 ;
		endy = (endy01 <= endy02) ? endy01 : endy02 ;

		dx = endx - startx ;
		dy = endy - starty ;
		return( othercell ) ;
	    } else {
		for( tileptr1 = tileptr01->nexttile ;
				    tileptr1 != TILENULL ;
				    tileptr1 = tileptr1->nexttile ) {
		    startx1 =  xc + tileptr1->left   ;
		    endx1   =  xc + tileptr1->right  ;
		    starty1 =  yc + tileptr1->bottom ;
		    endy1   =  yc + tileptr1->top    ;
		
		    for( tileptr2 = tileptr02->nexttile ; 
				    tileptr2 != TILENULL ;
				    tileptr2 = tileptr2->nexttile ) {
			startx2 =  xcenter + tileptr2->left  ;
			endx2   =  xcenter + tileptr2->right ;
			starty2 =  ycenter + tileptr2->bottom;
			endy2   =  ycenter + tileptr2->top   ;
	
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
			if( dx > 0 ) {
			    if( endx - startx < dx ) {
				dx = endx - startx ;
			    }
			} else {
			    dx = endx - startx ;
			}
			if( dy > 0 ) {
			    if( endy - starty < dy ) {
				dy = endy - starty ;
			    }
			} else {
			    dy = endy - starty ;
			}
		    }
		}
		if( dx != 0 || dy != 0 ) {
		    return( othercell ) ;
		}
	    }
	}
    }
}
return(0) ;
}



void padOverlaps(void)
{

CELLBOXPTR cellptr , padptr ;
TILEBOXPTR tileptr ;
int startx1 , endx1 , starty1 , endy1 ;
int deltaX , deltaY , pad , cell ;


for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[cell] ;
    tileptr = cellptr->config[cellptr->orient] ;

    startx1 = cellptr->xcenter + tileptr->left   ;
    endx1   = cellptr->xcenter + tileptr->right  ;
    starty1 = cellptr->ycenter + tileptr->bottom ;
    endy1   = cellptr->ycenter + tileptr->top    ;

    if( startx1 >= blockl && endx1 <= blockr &&
			    starty1 >= blockb && endy1 <= blockt ) {
	continue ;
    }
    if( startx1 < blockl ) {
	for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ){
	    padptr = cellarray[pad] ;
	    if( padptr->padside != L ) {
		continue ;
	    }
	    deltaX = padptr->xcenter + 
			padptr->config[padptr->orient]->right - startx1 ;
	    if( deltaX <= 0 ) {
		break ;
	    }
	    for( pad = numcells + 1; pad <= numcells + numpads; pad++){
		padptr = cellarray[pad] ;
		if( padptr->padside == L ) {
		    padptr->xcenter -= deltaX ;
		}
	    }
	    break ;
	}
    } else if( endx1 > blockr ) {
	for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ){
	    padptr = cellarray[pad] ;
	    if( padptr->padside != R ) {
		continue ;
	    }
	    deltaX = endx1 - ( padptr->xcenter + 
			padptr->config[padptr->orient]->left ) ;
	    if( deltaX <= 0 ) {
		break ;
	    }
	    for( pad = numcells + 1; pad <= numcells + numpads; pad++){
		padptr = cellarray[pad] ;
		if( padptr->padside == R ) {
		    padptr->xcenter += deltaX ;
		}
	    }
	    break ;
	}
    } else if( starty1 < blockb ) {
	for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ){
	    padptr = cellarray[pad] ;
	    if( padptr->padside != B ) {
		continue ;
	    }
	    deltaY = padptr->ycenter + 
			padptr->config[padptr->orient]->top - starty1 ;
	    if( deltaY <= 0 ) {
		break ;
	    }
	    for( pad = numcells + 1; pad <= numcells + numpads; pad++){
		padptr = cellarray[pad] ;
		if( padptr->padside == B ) {
		    padptr->ycenter -= deltaY ;
		}
	    }
	    break ;
	}
    } else if( endy1 > blockt ) {
	for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ){
	    padptr = cellarray[pad] ;
	    if( padptr->padside != T ) {
		continue ;
	    }
	    deltaY = endy1 - ( padptr->ycenter + 
			padptr->config[padptr->orient]->bottom ) ;
	    if( deltaY <= 0 ) {
		break ;
	    }
	    for( pad = numcells + 1; pad <= numcells + numpads; pad++){
		padptr = cellarray[pad] ;
		if( padptr->padside == T ) {
		    padptr->ycenter += deltaY ;
		}
	    }
	    break ;
	}
    }
}
return ;
}
