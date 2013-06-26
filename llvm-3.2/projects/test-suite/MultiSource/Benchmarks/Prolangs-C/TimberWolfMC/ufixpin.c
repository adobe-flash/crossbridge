#include "custom.h"

void ufixpin( TERMBOXPTR termsptr , int flag , int targetx , int targety )
{

DIMBOXPTR dimptr ;
TERMNETSPTR tmptr ;
TERMBOXPTR termptr ;

if( flag == 0 ) {
    for( termptr = termsptr ; termptr != TERMNULL ; termptr =
						termptr->nextterm ) {
	tmptr = termarray[ termptr->terminal ] ;
	netarray[ tmptr->net ]->flag = 1 ;
	tmptr->termptr->flag = 1 ;
	tmptr->termptr->newx = targetx + termptr->xpos ;
	tmptr->termptr->newy = targety + termptr->ypos ;
    }
} else {
    for( termptr = termsptr ; termptr != TERMNULL ; termptr =
					    termptr->nextterm ) {
	tmptr = termarray[ termptr->terminal ] ;
	dimptr = netarray[ tmptr->net ] ;
	dimptr->xmin = dimptr->newxmin ;
	dimptr->xmax = dimptr->newxmax ;
	dimptr->ymin = dimptr->newymin ;
	dimptr->ymax = dimptr->newymax ;
	tmptr->termptr->xpos = tmptr->termptr->newx ;
	tmptr->termptr->ypos = tmptr->termptr->newy ;
    }
}
return ;
}
