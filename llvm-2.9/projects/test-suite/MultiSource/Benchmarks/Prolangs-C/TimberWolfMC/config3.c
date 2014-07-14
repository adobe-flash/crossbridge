#include "custom.h"
#define L 1
#define T 2
#define R 3
#define B 4

void config3(void)
{

CELLBOXPTR cellptr ;
int pad , left , right , top , bottom ;
int deltaX , deltaY , target , cell , l , r , b , t ;

left = 1000000 ;
bottom = 1000000 ;
top = 0 ;
right = 0 ;

for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[cell] ;
    l = cellptr->xcenter + cellptr->config[cellptr->orient]->left
		     - (2 * cellptr->config[cellptr->orient]->lborder) ;
    r = cellptr->xcenter + cellptr->config[cellptr->orient]->right
		     + (2 * cellptr->config[cellptr->orient]->rborder) ;
    b = cellptr->ycenter + cellptr->config[cellptr->orient]->bottom
		     - (2 * cellptr->config[cellptr->orient]->bborder) ;
    t = cellptr->ycenter + cellptr->config[cellptr->orient]->top
		     + (2 * cellptr->config[cellptr->orient]->tborder) ;
    if( l < left ) {
	left = l ;
    }
    if( r > right ) {
	right = r ;
    }
    if( b < bottom ) {
	bottom = b ;
    }
    if( t > top ) {
	top = t ;
    }
}
deltaX = 0 ; /* (int)( 0.1 * (double)(right - left) ) ; */
deltaY = 0 ; /* (int)( 0.1 * (double)(top - bottom) ) ; */

for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    cellptr = cellarray[pad] ;
    if( cellptr->padside != B ) {
	continue ;
    }
    target = bottom - deltaY ;
    b = cellptr->ycenter + cellptr->config[cellptr->orient]->top ;
    b = target - b ;
}
for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    cellptr = cellarray[pad] ;
    if( cellptr->padside != T ) {
	continue ;
    }
    target = top + deltaY ;
    t = cellptr->ycenter + cellptr->config[cellptr->orient]->bottom ;
    t -= target ;
}
for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    cellptr = cellarray[pad] ;
    if( cellptr->padside != L ) {
	continue ;
    }
    target = left - deltaX ;
    l = cellptr->xcenter + cellptr->config[cellptr->orient]->right ;
    l = target - l ;
}
for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    cellptr = cellarray[pad] ;
    if( cellptr->padside != R ) {
	continue ;
    }
    target = right + deltaX ;
    r = cellptr->xcenter + cellptr->config[cellptr->orient]->left ;
    r -= target ;
}

for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    if( cellarray[pad]->padside == B ) {
	cellarray[pad]->ycenter += b ;
    } else if( cellarray[pad]->padside == T ) {
	cellarray[pad]->ycenter -= t ;
    } else if( cellarray[pad]->padside == L ) {
	cellarray[pad]->xcenter += l ;
    } else if( cellarray[pad]->padside == R ) {
	cellarray[pad]->xcenter -= r ;
    }
}
return ;
}



void reconfigSides( double degree )
{
CELLBOXPTR ptr ;
int deltaX , deltaY ;


deltaX = (int)( degree * (double)(blockr - blockl) ) ;
deltaY = (int)( degree * (double)(blockt - blockb) ) ;

ptr = cellarray[ numcells + numpads + 1 ] ;
ptr->xcenter -= deltaX ;

ptr = cellarray[ numcells + numpads + 2 ] ;
ptr->xcenter += deltaX ;

ptr = cellarray[ numcells + numpads + 3 ] ;
ptr->ycenter -= deltaY ;

ptr = cellarray[ numcells + numpads + 4 ] ;
ptr->ycenter += deltaY ;

return ;
}
