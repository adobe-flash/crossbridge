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

#define L 1
#define T 2
#define R 3
#define B 4

extern void initPts( int cell , char *name , int orient );
extern void addPts( int l , int r , int b , int t );
extern void unbust(void);

void outgeo(void)
{

FILE *fp ;
int cell ;
int l , b , r , t , xc , yc , k ;
char filename[1024] ;
CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;


sprintf( filename, "%s.geo", cktName ) ;
if( (fp = fopen( filename , "w") ) == NULL ) {
    fprintf(fpo,"can't open %s\n", filename ) ;
    exit(0) ;
}

for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[cell] ;

    tileptr = cellptr->config[ cellptr->orient ] ;

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

    fprintf(fp,"cell %s \n", cellptr->cname ) ;
    fprintf(fp,"%d vertices ", PtsOut[0].xc - 1 );
  
    for( k = 1 ; k < PtsOut[0].xc ; k++ ) {
	fprintf(fp," %d %d", xc + PtsOut[k].xc, yc + PtsOut[k].yc );
    }
    fprintf(fp,"\n");
}

l = 100000000 ;
b = 100000000 ;
r = -100000000 ;
t = -100000000 ;
for( cell = numcells + 1 ; cell <= numcells + numpads ; cell++ ) {
    cellptr = cellarray[cell] ;
    if( cellptr->padside != L ) {
	continue ;
    }
    xc = cellptr->xcenter ;
    yc = cellptr->ycenter ;
    tileptr = cellptr->config[ cellptr->orient ] ;

    if( xc + tileptr->left < l ) {
	l = xc + tileptr->left ;
    }
    if( xc + tileptr->right > r ) {
	r = xc + tileptr->right ;
    }
    if( yc + tileptr->bottom < b ) {
	b = yc + tileptr->bottom ;
    }
    if( yc + tileptr->top > t ) {
	t = yc + tileptr->top ;
    }
}
if( t != -100000000 ) {
    fprintf(fp,"cell %s \n", "pad.macro.l" ) ;
    fprintf(fp,"4 vertices ") ;
    fprintf(fp," %d %d %d %d %d %d %d %d\n", l, b, l, t, r, t, r, b ) ;
}

l = 100000000 ;
b = 100000000 ;
r = -100000000 ;
t = -100000000 ;
for( cell = numcells + 1 ; cell <= numcells + numpads ; cell++ ) {
    cellptr = cellarray[cell] ;
    if( cellptr->padside != T ) {
	continue ;
    }
    xc = cellptr->xcenter ;
    yc = cellptr->ycenter ;
    tileptr = cellptr->config[ cellptr->orient ] ;

    if( xc + tileptr->left < l ) {
	l = xc + tileptr->left ;
    }
    if( xc + tileptr->right > r ) {
	r = xc + tileptr->right ;
    }
    if( yc + tileptr->bottom < b ) {
	b = yc + tileptr->bottom ;
    }
    if( yc + tileptr->top > t ) {
	t = yc + tileptr->top ;
    }
}
if( t != -100000000 ) {
    fprintf(fp,"cell %s \n", "pad.macro.t" ) ;
    fprintf(fp,"4 vertices ") ;
    fprintf(fp," %d %d %d %d %d %d %d %d\n", l, b, l, t, r, t, r, b ) ;
}

l = 100000000 ;
b = 100000000 ;
r = -100000000 ;
t = -100000000 ;
for( cell = numcells + 1 ; cell <= numcells + numpads ; cell++ ) {
    cellptr = cellarray[cell] ;
    if( cellptr->padside != R ) {
	continue ;
    }
    xc = cellptr->xcenter ;
    yc = cellptr->ycenter ;
    tileptr = cellptr->config[ cellptr->orient ] ;

    if( xc + tileptr->left < l ) {
	l = xc + tileptr->left ;
    }
    if( xc + tileptr->right > r ) {
	r = xc + tileptr->right ;
    }
    if( yc + tileptr->bottom < b ) {
	b = yc + tileptr->bottom ;
    }
    if( yc + tileptr->top > t ) {
	t = yc + tileptr->top ;
    }
}
if( t != -100000000 ) {
    fprintf(fp,"cell %s \n", "pad.macro.r" ) ;
    fprintf(fp,"4 vertices ") ;
    fprintf(fp," %d %d %d %d %d %d %d %d\n", l, b, l, t, r, t, r, b ) ;
}

l = 100000000 ;
b = 100000000 ;
r = -100000000 ;
t = -100000000 ;
for( cell = numcells + 1 ; cell <= numcells + numpads ; cell++ ) {
    cellptr = cellarray[cell] ;
    if( cellptr->padside != B ) {
	continue ;
    }
    xc = cellptr->xcenter ;
    yc = cellptr->ycenter ;
    tileptr = cellptr->config[ cellptr->orient ] ;

    if( xc + tileptr->left < l ) {
	l = xc + tileptr->left ;
    }
    if( xc + tileptr->right > r ) {
	r = xc + tileptr->right ;
    }
    if( yc + tileptr->bottom < b ) {
	b = yc + tileptr->bottom ;
    }
    if( yc + tileptr->top > t ) {
	t = yc + tileptr->top ;
    }
}
if( t != -100000000 ) {
    fprintf(fp,"cell %s \n", "pad.macro.b" ) ;
    fprintf(fp,"4 vertices ") ;
    fprintf(fp," %d %d %d %d %d %d %d %d\n", l, b, l, t, r, t, r, b ) ;
}

fclose( fp ) ;
return ;

}
