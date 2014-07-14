#include "custom.h"

int findside( CELLBOXPTR cellptr , int x , int y )
{

int k , min , kmin ;

min = 10000000 ;
for( k = 1 ; k <= cellptr->numsides ; k++ ) {
    if( pSideArray[k].vertical == 1 ) {
	if( ABS(x - pSideArray[k].position) < min ) {
	    min = ABS(x - pSideArray[k].position) ;
	    kmin = k ;
	}
    } else {
	if( ABS(y - pSideArray[k].position) < min ) {
	    min = ABS(y - pSideArray[k].position) ;
	    kmin = k ;
	}
    }
}
return( kmin ) ;

}



void loadside( int side , double factor )
{

pSideArray[side].pincount += factor ;

return ;
}
