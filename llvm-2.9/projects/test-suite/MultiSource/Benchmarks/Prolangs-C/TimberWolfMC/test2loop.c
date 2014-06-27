#include "custom.h"
extern int newxx , newyy ;
extern int bigcell ;
extern int toobig  ;
extern double totFunc ;
extern double totPen  ;

extern void forceGrid( int x , int y );
extern int usite1( int a , int xb , int yb);
extern int newOrient( CELLBOXPTR cellptr , int range );
extern int usiteo1( int a , int xb , int yb , int newaor );
extern int usite0( int a , int newaor );
extern int usite2( int a , int b , int ax , int ay , int bx , int by );
extern int usiteo2( int a , int b , int ax , int ay , int bx , int by ,
		   int newaor, int newbor );

int test2loop( int trials )
{

CELLBOXPTR acellptr , bcellptr ;
TERMBOXPTR termptr , btermptr ;
int a , b , numberx , numbery ;
int attempts , reject ;
int xb , yb , axcenter , aycenter ;
int aorient , borient , newaor , newbor ;
int ll , rr , bb , tt , ax , ay , bx , by ;
int scalex , scaley ;

attempts = 0 ;
totFunc = 0.0 ;
totPen  = 0.0 ;

/* numberx = (int) ( 0.1667 * (double) bdxlength * log10( 0.10 * T ));*/
numberx = (int)( pow( 4.0 , log10(T) ) / 1024.0 * (double) bdxlength ) ;
if( numberx < 3 ) {
    numberx = 3 ;
} else if( numberx > bdxlength ) {
    numberx = bdxlength ;
}

/* numbery = (int) ( 0.1667 * (double) bdylength * log10( 0.10 * T ));*/
numbery = (int)( pow( 4.0 , log10(T) ) / 1024.0 * (double) bdylength ) ;
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

while( attempts < trials ) {
    do {
	a = (int) ( (double) numcells * ( (double) RAND / 
				    (double) 0x7fffffff ) ) + 1 ;
    } while( a == numcells + 1 ) ;

    do {
	b = (int) ( (double) bigcell * ( (double) RAND / 
				    (double) 0x7fffffff ) ) + 1 ;
    } while( b == toobig ) ;

    if( a == b ) {
	continue ;
    }

    acellptr = cellarray[ a ]   ;
    aorient  = acellptr->orient ;

    if( b > numcells ) {
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
	    termptr  = acellptr->config[aorient]->termptr ;
	    forceGrid( xb + termptr->xpos , yb + termptr->ypos ) ;
	    xb = newxx - termptr->xpos ;
	    yb = newyy - termptr->ypos ;
	}

        if( usite1( a , xb , yb ) ) {
	    attempts++ ;
	    totFunc += (double) funccost ;
	    totPen  += (double) penalty  ;
	} else {
	    newaor = newOrient( acellptr , 4 ) ;
	    reject = 1 ;
	    if( newaor >= 0 ) {
		if( usiteo1( a , xb , yb , newaor ) ) {
		    attempts++ ;
		    totFunc += (double) funccost ;
		    totPen  += (double) penalty  ;
		    reject = 0 ;
		}
	    }
	    if( reject ) {
		newaor = newOrient( acellptr , 8 ) ;
		if( newaor >= 0 ) {
		    if( usite0( a , newaor ) ) {
			attempts++ ;
			totFunc += (double) funccost ;
			totPen  += (double) penalty  ;
			reject = 0 ;
		    }
		}
	    }
	} 
    } else { /*   b <= numcells   */
	bcellptr = cellarray[b] ;
	if( acellptr->class == -1 || bcellptr->class == -1 ||
			    acellptr->class != bcellptr->class ) {
	    continue ;
	}
	if( gridGiven ) {
	    /*
	     *   Force pin 1 to lie on the underlying grid, specified
	     *   by:  n * (grid) + offset , in each direction.
	     */
	    btermptr = bcellptr->config[bcellptr->orient]->termptr ;
	    termptr = acellptr->config[aorient]->termptr ;
	    forceGrid( bcellptr->xcenter + termptr->xpos , 
		       bcellptr->ycenter + termptr->ypos ) ;
	    ax = newxx - termptr->xpos ;
	    ay = newyy - termptr->ypos ;
	    forceGrid( acellptr->xcenter + btermptr->xpos , 
		       acellptr->ycenter + btermptr->ypos ) ;
	    bx = newxx - btermptr->xpos ;
	    by = newyy - btermptr->ypos ;
	} else {
	    ax = bcellptr->xcenter ;
	    ay = bcellptr->ycenter ;
	    bx = acellptr->xcenter ;
	    by = acellptr->ycenter ;
	}

	if( usite2( a , b , ax , ay , bx , by )  ) {
	    attempts++ ;
	    totFunc += (double) funccost ;
	    totPen  += (double) penalty  ;
	} else {
	    /*
		try again with opposite orientation types
	    */
	    newaor = newOrient( acellptr , 4 ) ;
	    borient = cellarray[b]->orient ;
	    newbor = newOrient( cellarray[b] , 4 ) ;
	    if( newaor >= 0 || newbor >= 0 ) {
		if( newaor < 0 ) {
		    newaor = aorient ;
		} else if( newbor < 0 ) {
		    newbor = borient ;
		}
		if( gridGiven ) {
		    btermptr = bcellptr->config[newbor]-> termptr ;
		    termptr = acellptr->config[newaor]->termptr ;
		    forceGrid( bcellptr->xcenter + termptr->xpos , 
			       bcellptr->ycenter + termptr->ypos ) ;
		    ax = newxx - termptr->xpos ;
		    ay = newyy - termptr->ypos ;
		    forceGrid( acellptr->xcenter + btermptr->xpos , 
			       acellptr->ycenter + btermptr->ypos ) ;
		    bx = newxx - btermptr->xpos ;
		    by = newyy - btermptr->ypos ;
		} else {
		    ax = bcellptr->xcenter ;
		    ay = bcellptr->ycenter ;
		    bx = acellptr->xcenter ;
		    by = acellptr->ycenter ;
		}
		if( usiteo2( a, b, ax, ay, bx, by, newaor, newbor) ) {
		    attempts++ ;
		    totFunc += (double) funccost ;
		    totPen  += (double) penalty  ;
		}
	    }
	}
    }
}
return( attempts ) ;
}
