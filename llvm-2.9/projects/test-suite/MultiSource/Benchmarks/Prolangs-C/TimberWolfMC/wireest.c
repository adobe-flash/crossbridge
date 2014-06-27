#include "port.h"
#include "custom.h"

int wireestx( int xc , int yy1 , int yy2 , double factor3 )
{

int x , y ;
double factor1 , factor2 ;

if( xc <= blockmx ) {
    x = xc - blockl ;
    if( x < 0 ) {
	x = 0 ;
    }
} else {
    x = blockr - xc ;
    if( x < 0 ) {
	x = 0 ;
    }
}
if( yy2 >= blockmy && yy1 <= blockmy ) {
    y = (blockt - blockb) / 2 ;
} else if( ABS(yy1 - blockmy) <= ABS(yy2 - blockmy) ) {
    y = blockt - yy1 ;
    if( y < 0 ) {
	y = 0 ;
    }
} else {
    y = yy2 - blockb ;
    if( y < 0 ) {
	y = 0 ;
    }
}
factor1 = slopeX * (double)x + basefactor ;
factor2 = slopeY * (double)y + basefactor ;

return( (int)( 0.5 * ( factor1 * factor2 * factor3 * 
					    (double) aveChanWid ) ) ) ;
}



int wireesty( int yc , int xx1 , int xx2 , double factor3 )
{

int x , y ;
double factor1 , factor2 ;

if( yc <= blockmy ) {
    y = yc - blockb ;
    if( y < 0 ) {
	y = 0 ;
    }
} else {
    y = blockt - yc ;
    if( y < 0 ) {
	y = 0 ;
    }
}
if( xx2 >= blockmx && xx1 <= blockmx ) {
    x = (blockr - blockl) / 2 ;
} else if( ABS(xx1 - blockmx) <= ABS(xx2 - blockmx) ) {
    x = blockr - xx1 ;
    if( x < 0 ) {
	x = 0 ;
    }
} else {
    x = xx2 - blockl ;
    if( x < 0 ) {
	x = 0 ;
    }
}
factor1 = slopeX * (double)x + basefactor ;
factor2 = slopeY * (double)y + basefactor ;

return( (int)( 0.5 * ( factor1 * factor2 * factor3 * 
					    (double) aveChanWid ) ) ) ;

}
