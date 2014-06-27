#include "custom.h"
extern int iwire, iwirex , iwirey , icost ;
extern int fwire, fwirex , fwirey , fcost ;

extern int wireestx( int xc , int yy1 , int yy2 , double factor3 );

void finalcheck(void)
{

FILE *fp ;
DIMBOXPTR dimptr ;
NETBOXPTR netptr ;
TILEBOXPTR tileptr ;
CELLBOXPTR ptr ;
TERMBOXPTR term ;
TERMNETSPTR termptr ;
char filename[1024] ;
int cell , net , terminal , pin ;
int x , y , site , l , r , b , t , xc , yc ;
int xpos , ypos , value ;

sprintf( filename , "%s.wat" , cktName ) ;
fp = fopen( filename , "w" ) ;

fcost  = 0 ;
fwire  = 0 ;
fwirex = 0 ;
fwirey = 0 ;
fprintf(fp,"Cell wire estimation weighting factors per side:\n");
for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    if( cell <= numcells ) {
	xc = cellarray[cell]->xcenter ;
	yc = cellarray[cell]->ycenter ;
	tileptr = cellarray[cell]->config[cellarray[cell]->orient] ;
	l = xc + tileptr->left  ; 
	r = xc + tileptr->right ; 
	b = yc + tileptr->bottom; 
	t = yc + tileptr->top   ; 
	fprintf(fp,"  Cell: %s\n", cellarray[cell]->cname ) ;
	fprintf(fp,"        Border for left:%d  Pin_Factor:%g\n",
	    wireestx( l , b , t , tileptr->lweight ), tileptr->lweight);
	fprintf(fp,"        Border for rite:%d  Pin_Factor:%g\n",
	    wireestx( r , b , t , tileptr->rweight ), tileptr->rweight);
	fprintf(fp,"        Border for  bot:%d  Pin_Factor:%g\n",
	    wireestx( b , l , r , tileptr->bweight ), tileptr->bweight);
	fprintf(fp,"        Border for  top:%d  Pin_Factor:%g\n",
	    wireestx( t , l , r , tileptr->tweight ), tileptr->tweight);
    }
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
    fwirex += dimptr->xmax - dimptr->xmin ;
    fwirey += dimptr->ymax - dimptr->ymin ;
    fwire  += dimptr->xmax - dimptr->xmin + dimptr->ymax - dimptr->ymin;
    fcost += ((int)( dimptr->Hweight *
	    (double)( dimptr->xmax - dimptr->xmin))) +
	    ((int)( dimptr->Vweight *
	    (double)( dimptr->ymax - dimptr->ymin)));
}

value = 0 ;
for( cell = 1 ; cell <= numcells + numpads + 4 ; cell++ ) {
    if( cell > numcells && cell <= numcells + numpads ) {
	continue ;
    }
    value += (*overlapf)( cell, cellarray[cell]->xcenter, 
			      cellarray[cell]->ycenter,
			      cellarray[cell]->orient , 0 , 1 , 0 ) ;
}
value /= 2 ;  
fprintf(fpo,"\nFinal Overlap Penalty Function Value: %d\n", value );
return;
}



void initcheck(void)
{

DIMBOXPTR dimptr ;

int net ;

icost  = 0 ;
iwire  = 0 ;
iwirex = 0 ;
iwirey = 0 ;

for( net = 1 ; net <= numnets ; net++ ) {
    dimptr =  netarray[net] ;
    iwirex += dimptr->xmax - dimptr->xmin ;
    iwirey += dimptr->ymax - dimptr->ymin ;
    iwire  += dimptr->xmax - dimptr->xmin + dimptr->ymax - dimptr->ymin;
    icost += ((int)( dimptr->Hweight *
	    (double)( dimptr->xmax - dimptr->xmin))) +
	    ((int)( dimptr->Vweight *
	    (double)( dimptr->ymax - dimptr->ymin)));
}
return;
}
