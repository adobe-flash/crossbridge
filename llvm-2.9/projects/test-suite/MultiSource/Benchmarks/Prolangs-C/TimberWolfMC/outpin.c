#include "custom.h"
#define L 1
#define T 2
#define R 3
#define B 4

void outpin(void)
{

FILE *fp ;
DIMBOXPTR dimptr ;
NETBOXPTR netptr ;
int net , count , cell , c[5] ;
char filename[1024] ;

sprintf( filename, "%s.pin", cktName ) ;
if( (fp = fopen( filename , "w") ) == NULL ) {
    fprintf(fpo,"can't open %s\n", filename ) ;
    exit(0) ;
}
c[1] = 0 ;
c[2] = 0 ;
c[3] = 0 ;
c[4] = 0 ;
for( cell = numcells + 1 ; cell <= numcells + numpads ; cell++ ) {
    if( cellarray[cell]->padside == L ) {
	c[1] = 1 ;
	break ;
    }
}
for( cell = numcells + 1 ; cell <= numcells + numpads ; cell++ ) {
    if( cellarray[cell]->padside == T ) {
	c[2] = 1 ;
	break ;
    }
}
for( cell = numcells + 1 ; cell <= numcells + numpads ; cell++ ) {
    if( cellarray[cell]->padside == R ) {
	c[3] = 1 ;
	break ;
    }
}
for( cell = numcells + 1 ; cell <= numcells + numpads ; cell++ ) {
    if( cellarray[cell]->padside == B ) {
	c[4] = 1 ;
	break ;
    }
}
c[0] = numcells + 1 ;
if( c[1] == 1 ) {
    c[1] = c[0]++ ;
}
if( c[2] == 1 ) {
    c[2] = c[0]++ ;
}
if( c[3] == 1 ) {
    c[3] = c[0]++ ;
}
if( c[4] == 1 ) {
    c[4] = c[0]++ ;
}

for( net = 1 ; net <= numnets ; net++ ) {
    count = 0 ;
    dimptr =  netarray[net] ;
    for( netptr = dimptr->netptr ; netptr != NETNULL ;
				    netptr = netptr->nextterm ) {
	if( netptr->PorE > 0 ) {
	    count++ ;
	}
    }
    if( count <= 1 ) {
	continue ;
    }
    dimptr =  netarray[net] ;
    fprintf(fp,"net %s\n", dimptr->nname ) ;
    for( netptr = dimptr->netptr ; netptr != NETNULL ;
				    netptr = netptr->nextterm ) {
	if( netptr->cell <= numcells ) {
	    cell = netptr->cell ;
	} else {
	    if( cellarray[netptr->cell]->padside == L ) {
		cell = c[1] ;
	    } else if( cellarray[netptr->cell]->padside == T ) {
		cell = c[2] ;
	    } else if( cellarray[netptr->cell]->padside == R ) {
		cell = c[3] ;
	    } else if( cellarray[netptr->cell]->padside == B ) {
		cell = c[4] ;
	    }
	}
	fprintf(fp,"pin %s  x %d  y %d  cell %d  PinOrEquiv %d\n",
		pinnames[netptr->terminal] , netptr->xpos ,
		netptr->ypos , cell , netptr->PorE ) ;
    }
    fprintf(fp,"\n");
}
fclose(fp);
return ;
}
