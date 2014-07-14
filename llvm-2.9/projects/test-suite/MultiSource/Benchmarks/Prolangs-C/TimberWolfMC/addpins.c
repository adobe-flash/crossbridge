#include "custom.h"
extern double Hdefault ;
extern double Vdefault ;

void addpins(void)
{

int i , extraNets , cell , pin ;
CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
TERMBOXPTR termptr , term ;
DIMBOXPTR dimptr ;
NETBOXPTR netptr ;


extraNets = numcells ;

netarray = (DIMBOXPTR *) realloc( netarray , (numnets + extraNets + 1) * 
						sizeof(DIMBOXPTR));
for( i = 1 + numnets ; i <= numnets + extraNets ; i++ ) {
    dimptr = netarray[i] = (DIMBOXPTR) malloc( sizeof( DIMBOX ) ) ;
    dimptr->netptr  = NETNULL ;
    dimptr->nname   = NULL    ;
    dimptr->skip    = 0       ;
    dimptr->flag    = 0       ;
    dimptr->xmin    = 0       ;
    dimptr->newxmin = 0       ;
    dimptr->xmax    = 0       ;
    dimptr->newxmax = 0       ;
    dimptr->ymin    = 0       ;
    dimptr->newymin = 0       ;
    dimptr->ymax    = 0       ;
    dimptr->newymax = 0       ;
    dimptr->Hweight = Hdefault ;
    dimptr->Vweight = Vdefault ;
}
pinnames = (char **) realloc( pinnames, 
		    (maxterm + 2 * extraNets + 1) * sizeof(char *) );
for( pin = 1 + maxterm ; pin <= maxterm + 2 * extraNets ; pin++ ) {
    pinnames[pin] = (char *) malloc( 9 * sizeof(char) ) ;
    sprintf( pinnames[pin] , "PHANTOM" ) ;
}

termarray = (TERMNETSPTR *) realloc( termarray , 
		(maxterm + 2 * extraNets + 1) * sizeof(TERMNETSPTR) );
for( pin = 1 + maxterm ; pin <= maxterm + 2 * extraNets ; pin++ ) {
    termarray[pin] = (TERMNETSPTR) malloc( sizeof(TERMNETS) ) ;
}

for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[cell] ;
    tileptr = cellptr->config[cellptr->orient] ;
    termptr = tileptr->termptr ;
    term = tileptr->termptr = (TERMBOXPTR) malloc( sizeof(TERMBOX) ) ;
    term->nextterm = termptr ;
    term->xpos = 0 ;
    term->ypos = 0 ;
    term->terminal = maxterm + 2 * cell - 1 ;

    netptr = netarray[numnets + cell]->netptr = 
				(NETBOXPTR) malloc( sizeof(NETBOX) );
    netptr->terminal = maxterm + 2 * cell - 1 ;
    netptr->cell = cell ;
    netptr->flag = 0 ;
    netptr->skip = 0 ;
    netptr->xpos = cellptr->xcenter ;
    netptr->ypos = cellptr->ycenter ;
    termarray[netptr->terminal]->net = numnets + cell ;
    termarray[netptr->terminal]->termptr = netptr ;

    netptr = netptr->nextterm = (NETBOXPTR) malloc( sizeof(NETBOX) );
    netptr->terminal = maxterm + 2 * cell ;
    netptr->cell = cell ;
    netptr->flag = 0 ;
    netptr->skip = 0 ;
    netptr->xpos = (blockl + blockr) / 2 ;
    netptr->ypos = (blockb + blockt) / 2 ;
    netptr->nextterm = NETNULL ;
    termarray[netptr->terminal]->net = numnets + cell ;
    termarray[netptr->terminal]->termptr = netptr ;
}

return ;
}





void subpins(void)
{

int i , extraNets , cell , pin ;
CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
TERMBOXPTR termptr , term ;
DIMBOXPTR dimptr ;


extraNets = numcells ;

for( i = 1 + numnets ; i <= numnets + extraNets ; i++ ) {
    dimptr = netarray[i] ;
    free( dimptr->netptr->nextterm ) ;
    free( dimptr->netptr ) ;
    dimptr->netptr  = NETNULL ;
    dimptr->nname   = NULL    ;
    dimptr->skip    = 0       ;
    dimptr->flag    = 0       ;
    dimptr->xmin    = 0       ;
    dimptr->newxmin = 0       ;
    dimptr->xmax    = 0       ;
    dimptr->newxmax = 0       ;
    dimptr->ymin    = 0       ;
    dimptr->newymin = 0       ;
    dimptr->ymax    = 0       ;
    dimptr->newymax = 0       ;
}

for( pin = 1 + maxterm ; pin <= maxterm + 2 * extraNets ; pin++ ) {
    termarray[pin] = (TERMNETSPTR) NULL ;
}

for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[cell] ;
    tileptr = cellptr->config[cellptr->orient] ;
    termptr = tileptr->termptr ;
    term = termptr->nextterm ;
    tileptr->termptr = term ;
    free( termptr ) ;
}

return ;
}
