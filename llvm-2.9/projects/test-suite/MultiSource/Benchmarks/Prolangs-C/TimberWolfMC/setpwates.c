#include "custom.h"

void setpwates(void )
{

TILEBOXPTR tileptr ;
int cell ;

for( cell = 1 ; cell <= numcells ; cell++ ) {
    tileptr = cellarray[cell]->config[0] ;
    for( ; tileptr != TILENULL ; tileptr = tileptr->nexttile ) {
	if( tileptr->lweight > pinsPerLen ) {
	    tileptr->lweight /= pinsPerLen ;
	} else {
	    tileptr->lweight = 1.0 ;
	}
	if( tileptr->rweight > pinsPerLen ) {
	    tileptr->rweight /= pinsPerLen ;
	} else {
	    tileptr->rweight = 1.0 ;
	}
	if( tileptr->bweight > pinsPerLen ) {
	    tileptr->bweight /= pinsPerLen ;
	} else {
	    tileptr->bweight = 1.0 ;
	}
	if( tileptr->tweight > pinsPerLen ) {
	    tileptr->tweight /= pinsPerLen ;
	} else {
	    tileptr->tweight = 1.0 ;
	}
    }
}
return ;

}
