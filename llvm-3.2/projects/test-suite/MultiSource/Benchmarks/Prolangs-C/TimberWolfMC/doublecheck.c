#include "custom.h"

void doublecheck(void)
{
DIMBOXPTR dimptr ;
NETBOXPTR netptr ;
CELLBOXPTR ptr ;
TERMBOXPTR term ;
TERMNETSPTR termptr ;

int cell , net , terminal , pin ;
int x , y , site ;
int xpos , ypos , cost , value ;

cost  = 0 ;
for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    ptr = cellarray[ cell ] ;
    term = ptr->config[ptr->orient]->termptr ;
    for( ; term != TERMNULL ; term = term->nextterm ) {
	termptr = termarray[term->terminal ];
	termptr->termptr->xpos = term->xpos + ptr->xcenter ;
	termptr->termptr->ypos = term->ypos + ptr->ycenter ;
    }
    if( ptr->softflag == 1 ) {
	for( pin = 1 ; pin <= ptr->numUnComTerms ; pin++ ) {
	   site = ptr->unComTerms[pin].site ;
	   terminal = ptr->unComTerms[pin].terminal ;
	   xpos = ptr->config[ptr->orient]->siteLocArray[site].xpos;
	   ypos = ptr->config[ptr->orient]->siteLocArray[site].ypos;
	    
	   termptr = termarray[terminal] ;
	   termptr->termptr->xpos = xpos + ptr->xcenter ;
	   termptr->termptr->ypos = ypos + ptr->ycenter ;
	}
    }
}

for( net = 1 ; net <= numnets ; net++ ) {
    dimptr =  netarray[net] ;
    if( dimptr->skip == 1 ) {
	continue ;
    }
    netptr = dimptr->netptr ;
    for( ; ; netptr = netptr->nextterm ) {
	if( netptr == NETNULL ) {
	    break ;
	}
	if( netptr->skip == 1 ) {
	    continue ;
	}
	dimptr->xmin = dimptr->xmax = netptr->xpos ;
	dimptr->ymin = dimptr->ymax = netptr->ypos ;
	netptr = netptr->nextterm ;
	break ;
    }
    for( ; netptr != NETNULL ; netptr = netptr->nextterm ) {
	if( netptr->skip == 1 ) {
	    continue ;
	}
	x = netptr->xpos ;
	y = netptr->ypos ;

	if( x < dimptr->xmin ) {
	    dimptr->xmin = x ;
	} else if( x > dimptr->xmax ) {
	    dimptr->xmax = x ;
	}
	if( y < dimptr->ymin ) {
	    dimptr->ymin = y ;
	} else if( y > dimptr->ymax ) {
	    dimptr->ymax = y ;
	}
    }
    cost += ((int)( dimptr->Hweight *
	    (double)( dimptr->xmax - dimptr->xmin))) +
	    ((int)( dimptr->Vweight *
	    (double)( dimptr->ymax - dimptr->ymin)));
}

value = 0 ;
for( cell = 1 ; cell <= numcells + numpads + 4 ; cell++ ) {
    if( cell > numcells && cell <= numcells + numpads ) {
	continue ;
    }
    value += (*overlap)( cell, cellarray[cell]->xcenter, 
			      cellarray[cell]->ycenter,
			      cellarray[cell]->orient , 0 , 1 , 0 ) ;
}
value /= 2 ;  
fprintf(fpo,"DoubleCheck Wire Cost: %d      ", cost );
fprintf(fpo,"Penalty: %d\n", value );
return;
}
