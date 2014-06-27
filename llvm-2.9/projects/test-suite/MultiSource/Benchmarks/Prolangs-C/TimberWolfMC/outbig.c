#include "custom.h"
extern int finalShot ;
BUSTBOXPTR PtsArray ;
BUSTBOXPTR PtsOut   ;
typedef struct Rangle {
    int rl ;
    int rr ;
    int rb ;
    int rt ;
} RANGLE ;
RANGLE *rectang ;

extern void initPts( int cell , char *name , int orient );
extern int wireestx( int xc , int yy1 , int yy2 , double factor3 );
extern void addPts( int l , int r , int b , int t );
extern void unbust(void);

void outbig(void)
{

FILE *fp ;
int j , x , y , pin ;
int cell , Wwd ;
int l , b , r , t , xc , yc , k ;
char filename[1024] ;
CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
TERMBOXPTR termptr ;

k = ( bdxlength > bdylength ) ? bdxlength : bdylength ;
Wwd = (int)( (double) k / 500.0 ) + 1 ;

if( finalShot == 0 ) {
    rectang = (RANGLE *) malloc( 26 * sizeof( RANGLE ) ) ;
    PtsArray = (BUSTBOXPTR) malloc( 101 * sizeof(BUSTBOX) ) ;
    PtsOut   = (BUSTBOXPTR) malloc( 101 * sizeof(BUSTBOX) ) ;
}

sprintf( filename, "%s.cfb", cktName ) ;
if( (fp = fopen( filename , "w") ) == NULL ) {
    fprintf(fpo,"can't open %s\n", filename ) ;
    exit(0) ;
}

for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    cellptr = cellarray[cell] ;
    tileptr = cellptr->config[ cellptr->orient ] ;
    termptr = tileptr->termptr ;

    xc = cellptr->xcenter ;
    yc = cellptr->ycenter ;
    
    initPts( cell , cellptr->cname , cellptr->orient ) ;

    l = tileptr->left ;
    r = tileptr->right ;
    b = tileptr->bottom ;
    t = tileptr->top ;
    if( cell <= numcells ) {
	if( finalShot == 0 ) {
	    l -= wireestx( xc+l , yc+b , yc+t , tileptr->lweight ) ;
	    r += wireestx( xc+r , yc+b , yc+t , tileptr->rweight ) ;
	    b -= wireestx( yc+b , xc+l , xc+r , tileptr->bweight ) ;
	    t += wireestx( yc+t , xc+l , xc+r , tileptr->tweight ) ;
	} else {
	    l -= tileptr->lborder ;
	    r += tileptr->rborder ;
	    b -= tileptr->bborder ;
	    t += tileptr->tborder ;
	}
    }
    addPts( l , r , b , t ) ;

    l += xc ;
    r += xc ;
    b += yc ;
    t += yc ;
    fprintf(fp,"L NC;\n94 %s %d %d;\n",
			    cellptr->cname , (r + l)/2 , (t + b)/2 );
    unbust() ;
    /*
     *  The vertices comprising the outline are in order
     *  in the array: PtsOut[]
     */

    fprintf(fp,"L NC;\nW %d", Wwd );
    for( k = 1 ; k < PtsOut[0].xc ; k++ ) {
	fprintf(fp," %d %d", xc + PtsOut[k].xc, yc + PtsOut[k].yc );
    }
    fprintf(fp," %d %d;\n", xc + PtsOut[1].xc, yc + PtsOut[1].yc );

   
    for( ; termptr != TERMNULL ; termptr = termptr->nextterm ) {
	x = xc + termptr->xpos ; 
	y = yc + termptr->ypos ; 
	pin = termptr->terminal ;
	if( strcmp( pinnames[pin] , "PHANTOM" ) != 0 ) {
	    fprintf(fp,"L NC;\n94 %s %d %d;\n",
		    netarray[ termarray[pin]->net ]->nname, x , y ) ;
	}
    }
    
    if( cellptr->softflag == 1 ) {
	for( j = 1 ; j <= cellptr->numUnComTerms ; j++ ) {
	    x = xc + cellptr->unComTerms[j].finalx ;
	    y = yc + cellptr->unComTerms[j].finaly ;
	    pin  = cellptr->unComTerms[j].terminal ;
	    fprintf(fp,"L NC;\n94 %s %d %d;\n", 
		     netarray[ termarray[pin]->net ]->nname, x , y ) ;
	}
    }
}

l = blockl ;
r = blockr ;
b = blockb ;
t = blockt ;

fprintf(fp,"L NC;\n");
fprintf(fp,"W %d %d %d %d %d %d %d %d %d %d %d;\n", Wwd ,
					l, b, l, t, r, t, r, b, l, b ) ;

fprintf( fp , "E\n" );
fclose( fp ) ;
return ;
}
