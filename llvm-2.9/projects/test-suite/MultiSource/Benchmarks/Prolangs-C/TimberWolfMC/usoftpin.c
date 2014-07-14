#include "custom.h"

void usoftpin( CELLBOXPTR cellptr , int flag , int targetx , int targety ,
              int orient )
{

DIMBOXPTR dimptr ;
TERMNETSPTR tmptr ;
UNCOMBOX *UCptr ;
LOCBOX *SLptr ;
int tm ;

UCptr = cellptr->unComTerms ;
SLptr = cellptr->config[ orient ]->siteLocArray ;

if( flag == 0 ) {
    for( tm = 1 ; tm <= cellptr->numUnComTerms ; tm++ ) {
	tmptr = termarray[ UCptr[tm].terminal ] ;
	netarray[ tmptr->net ]->flag = 1 ;
	tmptr->termptr->flag = 1 ;
	tmptr->termptr->newx = targetx + SLptr[ UCptr[tm].site ].xpos ;
	tmptr->termptr->newy = targety + SLptr[ UCptr[tm].site ].ypos ;
    }
} else {
    for( tm = 1 ; tm <= cellptr->numUnComTerms ; tm++ ) {
	tmptr = termarray[ UCptr[tm].terminal ] ;
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
