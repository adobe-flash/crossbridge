#include "custom.h"
extern BUSTBOXPTR PtsArray ;
extern BUSTBOXPTR PtsOut   ;
typedef struct Rangle {
    int rl ;
    int rr ;
    int rb ;
    int rt ;
} RANGLE ;
extern RANGLE *rectang ;

extern void initPts( int cell , char *name , int orient );
extern void addPts( int l , int r , int b , int t );
extern void unbust(void);

void output(void)
{

FILE *fp ;
int j , x , y , pin ;
int cell ;
int l , b , r , t , xc , yc , k ;
char filename[1024] ;
CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
TERMBOXPTR termptr ;


sprintf( filename, "%s.dat", cktName ) ;
if( (fp = fopen( filename , "w") ) == NULL ) {
    fprintf(fpo,"can't open %s\n", filename ) ;
    exit(0) ;
}

for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    cellptr = cellarray[cell] ;
    if( cell <= numcells ) {
	fprintf(fp,"cell %s  orientation %d\n", cellptr->cname , 
						cellptr->orient ) ;
	fprintf(fp,"vertices ");
    } else {
	fprintf(fp,"pad %s\n", cellptr->cname ) ;
	fprintf(fp,"vertices ");
    }
    tileptr = cellptr->config[ cellptr->orient ] ;
    termptr = tileptr->termptr ;

    xc = cellptr->xcenter ;
    yc = cellptr->ycenter ;
    
    initPts( cell , cellptr->cname , cellptr->orient ) ;
    for( tileptr = tileptr->nexttile ; tileptr != TILENULL ;
				tileptr = tileptr->nexttile ) {
	
	l = tileptr->left ;
	r = tileptr->right ;
	b = tileptr->bottom ;
	t = tileptr->top ;
	addPts( l , r , b , t ) ;
    }
    unbust() ;
    for( k = 1 ; k < PtsOut[0].xc ; k++ ) {
	fprintf(fp," %d %d", xc + PtsOut[k].xc, yc + PtsOut[k].yc );
    }
    fprintf(fp,"\n");

   
    for( ; termptr != TERMNULL ; termptr = termptr->nextterm ) {
	x = xc + termptr->xpos ; 
	y = yc + termptr->ypos ; 
	pin = termptr->terminal ;
	if( strcmp( pinnames[pin] , "PHANTOM" ) != 0 ) {
	    fprintf(fp,"pin %s %s %d %d\n", pinnames[pin] , 
		    netarray[ termarray[pin]->net ]->nname, x , y ) ;
	}
    }
    
    if( cellptr->softflag == 1 ) {
	for( j = 1 ; j <= cellptr->numUnComTerms ; j++ ) {
	    x = xc + cellptr->unComTerms[j].finalx ;
	    y = yc + cellptr->unComTerms[j].finaly ;
	    pin  = cellptr->unComTerms[j].terminal ;
	    fprintf(fp,"pin %s %s %d %d\n", pinnames[pin] , 
		     netarray[ termarray[pin]->net ]->nname, x , y ) ;
	}
    }
}
fclose( fp ) ;
return ;

}
