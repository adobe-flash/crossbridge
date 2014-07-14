#include "custom.h"
#include "geo.h"
extern int numberCells ;
/* #define DEBUG */

int findBorder( int c , int loc , int beg , int end , int HorV );

void doborder(void)
{

int cell , i , orient ;
int l , b , r , t , xc , yc ;
CELLBOXPTR cellptr ;
TILEBOXPTR tileptr , tptr , tptr0 ;
CLBOXPTR clptr , cl2ptr ;


#ifdef DEBUG
fprintf(fpo,"\n\nGlobal Router Wiring Space Determination:\n");
#endif

for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[cell] ;
    tileptr = cellptr->config[ cellptr->orient ] ;
    xc = cellptr->xcenter ;
    yc = cellptr->ycenter ;
#ifdef DEBUG
    fprintf(fpo,"cell number: %d  with orientation: %d\n", 
					cell , cellptr->orient ) ;
#endif
    for( ; tileptr != TILENULL ; tileptr = tileptr->nexttile ) {
	
	l = tileptr->left   + xc ;
	r = tileptr->right  + xc ;
	b = tileptr->bottom + yc ;
	t = tileptr->top    + yc ;
	tileptr->lborder = findBorder( cell , l , b , t , 0 ) ;
	tileptr->rborder = findBorder( cell , r , b , t , 0 ) ;
	tileptr->bborder = findBorder( cell , b , l , r , 1 ) ;
	tileptr->tborder = findBorder( cell , t , l , r , 1 ) ;
#ifdef DEBUG
	fprintf(fpo,"tile:\n");
	fprintf(fpo,"     left border: %d\n", tileptr->lborder ) ;
	fprintf(fpo,"     top  border: %d\n", tileptr->tborder ) ;
	fprintf(fpo,"     rite border: %d\n", tileptr->rborder ) ;
	fprintf(fpo,"     bot  border: %d\n", tileptr->bborder ) ;
#endif
    }
    if( cellptr->orient != 0 ) {
	tptr0 = cellptr->config[0] ;
	tptr  = cellptr->config[cellptr->orient] ;
	for( ; tptr != TILENULL ; tptr = tptr->nexttile ,
				  tptr0 = tptr0->nexttile ) {
	    switch( cellptr->orient ) {
		case 1 : tptr0->tborder = tptr->bborder ;
			 tptr0->bborder = tptr->tborder ;
			 tptr0->rborder = tptr->rborder ;
			 tptr0->lborder = tptr->lborder ;
			 break ;
		case 2 : tptr0->rborder = tptr->lborder ;
			 tptr0->lborder = tptr->rborder ;
			 tptr0->rborder = tptr->rborder ;
			 tptr0->lborder = tptr->lborder ;
			 break ;
		case 3 : tptr0->tborder = tptr->bborder ;
			 tptr0->bborder = tptr->tborder ;
			 tptr0->rborder = tptr->lborder ;
			 tptr0->lborder = tptr->rborder ;
			 break ;
		case 4 : tptr0->tborder = tptr->lborder ;
			 tptr0->bborder = tptr->rborder ;
			 tptr0->rborder = tptr->bborder ;
			 tptr0->lborder = tptr->tborder ;
			 break ;
		case 5 : tptr0->tborder = tptr->rborder ;
			 tptr0->bborder = tptr->lborder ;
			 tptr0->rborder = tptr->tborder ;
			 tptr0->lborder = tptr->bborder ;
			 break ;
		case 6 : tptr0->lborder = tptr->bborder ;
			 tptr0->bborder = tptr->rborder ;
			 tptr0->rborder = tptr->tborder ;
			 tptr0->tborder = tptr->lborder ;
			 break ;
		case 7 : tptr0->lborder = tptr->tborder ;
			 tptr0->bborder = tptr->lborder ;
			 tptr0->rborder = tptr->bborder ;
			 tptr0->tborder = tptr->rborder ;
			 break ;
	    }
	}
    }
    for( orient = 1 ; orient <= 7 ; orient++ ) {
	if( orient == cellptr->orient ) {
	    continue ;
	}
	tptr0 = cellptr->config[0] ;
	tptr  = cellptr->config[orient] ;
	for( ; tptr != TILENULL ; tptr = tptr->nexttile ,
				  tptr0 = tptr0->nexttile ) {
	    switch( orient ) {
		case 1 : tptr->tborder = tptr0->bborder ;
			 tptr->bborder = tptr0->tborder ;
			 tptr->rborder = tptr0->rborder ;
			 tptr->lborder = tptr0->lborder ;
			 break ;
		case 2 : tptr->rborder = tptr0->lborder ;
			 tptr->lborder = tptr0->rborder ;
			 tptr->rborder = tptr0->rborder ;
			 tptr->lborder = tptr0->lborder ;
			 break ;
		case 3 : tptr->tborder = tptr0->bborder ;
			 tptr->bborder = tptr0->tborder ;
			 tptr->rborder = tptr0->lborder ;
			 tptr->lborder = tptr0->rborder ;
			 break ;
		case 4 : tptr->tborder = tptr0->lborder ;
			 tptr->bborder = tptr0->rborder ;
			 tptr->rborder = tptr0->bborder ;
			 tptr->lborder = tptr0->tborder ;
			 break ;
		case 5 : tptr->tborder = tptr0->rborder ;
			 tptr->bborder = tptr0->lborder ;
			 tptr->rborder = tptr0->tborder ;
			 tptr->lborder = tptr0->bborder ;
			 break ;
		case 6 : tptr->lborder = tptr0->tborder ;
			 tptr->bborder = tptr0->lborder ;
			 tptr->rborder = tptr0->bborder ;
			 tptr->tborder = tptr0->rborder ;
			 break ;
		case 7 : tptr->lborder = tptr0->bborder ;
			 tptr->bborder = tptr0->rborder ;
			 tptr->rborder = tptr0->tborder ;
			 tptr->tborder = tptr0->lborder ;
			 break ;
	    }
	}

    }
}

for( i = 1 ; i <= numberCells ; i++ ) {
    clptr = cellList[i] ;
    while( clptr != (CLBOXPTR) NULL ) {
	cl2ptr = clptr->next ;
	free( clptr ) ;
	clptr = cl2ptr ;
    }
}
free( cellList ) ;

return ;
}

int findBorder( int c , int loc , int beg , int end , int HorV )
{
int extraSpace ;
CLBOXPTR clptr ;

extraSpace = 0 ;
for( clptr = cellList[c]; clptr != (CLBOXPTR) NULL; clptr = clptr->next){
    if( clptr->HorV != HorV || clptr->loc != loc ) {
	continue ;
    }
    if( clptr->end <= beg || clptr->start >= end ) {
	continue ;
    }
    if( clptr->extraSpace > extraSpace ) {
	extraSpace = clptr->extraSpace ;
    }
}

return( extraSpace ) ;
}
