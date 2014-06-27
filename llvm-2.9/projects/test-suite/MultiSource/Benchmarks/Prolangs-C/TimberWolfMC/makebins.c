#include "custom.h"

void makebins( int numc )
{

double b , e , c ;
int a , b1 , xbins , ybins ;
int saveHx , saveHy , saveLx , saveLy , totalH , totalL ;
int saveL2x , saveL2y , totalL2 ;
int i , j , x , y , diff ;

c = (double) numc ;
for( b1 = 1 ; b1 <= 19 ; b1++ ) {
    for( a = 0 ; a <= 99 ; a++ ) {
	b = b1 + (double) a / 100.0 ;
	e = pow((double)b, 4.0) - 
			(2 * sqrt(c) + 2.0) * pow((double)b,3.0) +
			(2 * sqrt(c) + 1.0) * pow((double)b,2.0) + c ;
	if( e < 0 ) {
	    b1 = 20 ;
	    break ;
	}
    }
}
xbins = (int) b + 1 ;
ybins = (int) b + 1 ;

totalH = xbins + ybins ;
totalL = totalH - 1 ;
totalL2 = totalL - 1 ;

saveHx = 100 ;
diff = 32000 ;
for( x = 1 ; x <= xbins ; x++ ) {
    y = totalH - x ;
    if( x * y < b * b - 0.5 ) {
	continue ;
    }
    if( x * y - b * b < diff ) {
	diff = x * y - b * b ;
	saveHx = x ;
	saveHy = y ;
    }
}
saveLx = 100 ;
diff = 32000 ;
for( x = 1 ; x <= xbins ; x++ ) {
    y = totalL - x ;
    if( x * y < b * b - 0.5 ) {
	continue ;
    }
    if( x * y - b * b < diff ) {
	diff = x * y - b * b ;
	saveLx = x ;
	saveLy = y ;
    }
}
saveL2x = 100 ;
diff = 32000 ;
for( x = 1 ; x <= xbins ; x++ ) {
    y = totalL2 - x ;
    if( x * y < b * b - 0.5 ) {
	continue ;
    }
    if( x * y - b * b < diff ) {
	diff = x * y - b * b ;
	saveL2x = x ;
	saveL2y = y ;
    }
}
if( saveLx < 100 && saveHx < 100 ) {
    if( saveLx * saveLy <= saveHx * saveHy ) {
	xbins = saveLx ;
	ybins = saveLy ;
    } else {
	xbins = saveHx ;
	ybins = saveHy ;
    }
} else if( saveHx < 100 ) {
    xbins = saveHx ;
    ybins = saveHy ;
} else if( saveLx < 100 ) {
    xbins = saveLx ;
    ybins = saveLy ;
}
if( saveLx < 100 || saveHx < 100 ) {
    if( saveL2x < 100 ) {
	if( xbins * ybins >= saveL2x * saveL2y ) {
	    xbins = saveL2x ;
	    ybins = saveL2y ;
	}
    }
} else {
    xbins = saveL2x ;
    ybins = saveL2y ;
}
numBinsX = xbins ;
numBinsY = ybins ;

fprintf( fpo , "numBinsX automatically set to:%d\n", numBinsX );
fprintf( fpo , "numBinsY automatically set to:%d\n", numBinsY );

blockarray = (int ***) malloc( (1 + numBinsX) * sizeof(int **) ) ; 
for( i = 0 ; i <= numBinsX ; i++ ) {
    blockarray[i] = (int **) malloc((1 + numBinsY) * sizeof(int *));
    for( j = 0 ; j <= numBinsY ; j++ ) {
	blockarray[i][j] = (int *) malloc((5 + numcells) * 
						       sizeof(int));
    }
}
bucket = blockarray[0][0] ;

}
