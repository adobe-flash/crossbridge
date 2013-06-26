#include "custom.h"

void watesides( CELLBOXPTR ptr )
{

TILEBOXPTR tileptr ;
int side , left , right , bottom , top ;

tileptr = ptr->config[0] ;
for( ; tileptr != TILENULL ; tileptr = tileptr->nexttile ) {
    left = ptr->xcenter + tileptr->left ;
    tileptr->lweight = -1.0 ;
    for( side = 1 ; side <= ptr->numsides ; side++ ) {
	if( pSideArray[side].vertical == 1 ) {
	    if( pSideArray[side].position == left ) {
		tileptr->lweight = pSideArray[side].pincount / 
				    (double) pSideArray[side].length ; 
	    }
	}
    }
    right = ptr->xcenter + tileptr->right ;
    tileptr->rweight = -1.0 ;
    for( side = 1 ; side <= ptr->numsides ; side++ ) {
	if( pSideArray[side].vertical == 1 ) {
	    if( pSideArray[side].position == right ) {
		tileptr->rweight = pSideArray[side].pincount / 
				    (double) pSideArray[side].length ; 
	    }
	}
    }
    bottom = ptr->ycenter + tileptr->bottom ;
    tileptr->bweight = -1.0 ;
    for( side = 1 ; side <= ptr->numsides ; side++ ) {
	if( pSideArray[side].vertical == 0 ) {
	    if( pSideArray[side].position == bottom ) {
		tileptr->bweight = pSideArray[side].pincount / 
				    (double) pSideArray[side].length ; 
	    }
	}
    }
    top = ptr->ycenter + tileptr->top ;
    tileptr->tweight = -1.0 ;
    for( side = 1 ; side <= ptr->numsides ; side++ ) {
	if( pSideArray[side].vertical == 0 ) {
	    if( pSideArray[side].position == top ) {
		tileptr->tweight = pSideArray[side].pincount / 
				    (double) pSideArray[side].length ; 
	    }
	}
    }
}
return ;

}
