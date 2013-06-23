#include "custom.h"
extern int newxx , newyy ;
extern int flips ;
extern double Tsave ;

extern void forceGrid( int x , int y );
extern int usite1( int a , int xb , int yb);
extern void selectpin( CELLBOXPTR acellptr );

void fuloop(void)
{

CELLBOXPTR acellptr;
TERMBOXPTR termptr ;
int a , numberx , numbery ;
int attempts , i ;
int xb , yb , axcenter , aycenter ;
int flip1 , att1 ;
int ll , rr , bb , tt ;
int scalex , scaley ;
double bound ;

flips    = 0 ;
attempts = 0 ;
flip1    = 0 ;
att1     = 0 ;

bound = log10( Tsave ) ;
if( bound > 6.0 ) {
    bound = bound - 1.0 ;
} else {
    bound = 5.0 ;
}
bound = pow( 4.0 , bound ) ;

numberx = (int)( pow( 4.0 , log10(T) ) / bound * (double) bdxlength ) ;
numberx *= 2 ;
if( numberx < 3 ) {
    numberx = 3 ;
} else if( numberx > bdxlength ) {
    numberx = bdxlength ;
}

numbery = (int)( pow( 4.0 , log10(T) ) / bound * (double) bdylength ) ;
numbery *= 2 ;
if( numbery < 3 ) {
    numbery = 3 ;
} else if( numbery > bdylength ) {
    numbery = bdylength ;
} 
scalex = numberx / 3 ;
scaley = numbery / 3 ;
fprintf(fpo,"range limiter:%d units either way in x",numberx);
fprintf(fpo,"   compared to bdxlength:%d\n",bdxlength);
fprintf(fpo,"range limiter:%d units either way in y",numbery);
fprintf(fpo,"   compared to bdylength:%d\n",bdylength);
fprintf(fpo,"scalex:%d  scaley:%d\n", scalex , scaley ) ;

while( attempts < attmax ) {
    do {
	a = (int) ( (double) numcells * ( (double) RAND / 
				    (double) 0x7fffffff ) ) + 1 ;
    } while( a == numcells + 1 ) ;

    acellptr = cellarray[ a ]   ;

    if( acellptr->class != 0 ) {
	continue ;
    }
    axcenter = acellptr->xcenter ;
    aycenter = acellptr->ycenter ;

    ll = (blockl > axcenter-numberx) ? (blockl - axcenter) / scalex :
		( (blockr <= axcenter - numberx) ? 
		(blockr - axcenter) / scalex : (- numberx / scalex) ) ;
    rr = (blockr < axcenter+numberx) ? (blockr - axcenter) / scalex :
		( (blockl >= axcenter + numberx) ? 
		(blockl - axcenter) /scalex : (numberx / scalex) ) ;
    bb = (blockb > aycenter-numbery) ? (blockb - aycenter) / scaley :
		( (blockt <= aycenter - numbery) ? 
		(blockt - aycenter) / scaley : (- numbery / scaley) ) ;
    tt = (blockt < aycenter+numbery) ? (blockt - aycenter) / scaley :
		( (blockb >= aycenter + numbery) ? 
		(blockb - aycenter) / scaley : (numbery / scaley) ) ;
    xb = (int)((double)(rr - ll + 1) * ((double)RAND /
				    (double) 0x7fffffff ) ) + ll ;
    yb = (int)((double)(tt - bb + 1) * ((double)RAND /
				    (double) 0x7fffffff ) ) + bb ;
    if( xb == 0 && yb == 0 ) {
	continue ;
    }
    xb = axcenter + scalex * xb ;
    yb = aycenter + scaley * yb ;

    if( gridGiven ) {
	termptr  = acellptr->config[acellptr->orient]->termptr ;
	forceGrid( xb + termptr->xpos , yb + termptr->ypos ) ;
	xb = newxx - termptr->xpos ;
	yb = newyy - termptr->ypos ;
    }

    if( usite1( a , xb , yb ) ) {
	flips++ ;
	flip1++ ;
    }
    attempts++ ;
    att1++ ;

    if( acellptr->numUnComTerms != 0 ){
	for( i = 1 ; i <= acellptr->numgroups ; i++ ) {
	    selectpin( acellptr ) ;
	}
    }
}
fprintf(fpo,"acceptance breakdown:\n"); 
fprintf(fpo,"              single cell: %d / %d\n", flip1 , att1) ;
fprintf(fpo,"\tno. of accepted flips: %d\n", flips ) ;

return ;
}
