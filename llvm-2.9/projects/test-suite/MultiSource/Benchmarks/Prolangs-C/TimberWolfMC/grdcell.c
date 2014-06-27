#include "custom.h"
extern BUSTBOXPTR A ;
int ecount ;

extern void gpass2( FILE *fp );
extern void ggenorien(void);
extern int hashfind( char hname[] );

void grdcell( FILE *fp )
{

int i , cell , class ;
int xcenter , ycenter ;
int xpos , ypos ;
int sequence , k ;
int firstside , lastside , ncorners , corner , orient , norients ;
int x , y , minx , miny , maxx , maxy ;
int cellnum , pinctr , netx ;
double aspub , asplb , space ;
char input[1024] ;
CELLBOXPTR ptr ;
TERMBOXPTR term , saveterm ;
NETBOXPTR netptr , saveptr ;

gpass2( fp ) ;
pinctr = 0   ;
ecount = 0   ;

cell = 0 ;

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "cell") == 0 ) {
	ptr = cellarray[ ++cell ] ;
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* cell name */

	for( k = 0 ; k <= 30 ; k++ ) {
	    A[k].xc = 0 ;
	    A[k].yc = 0 ;
	}
	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	    A[ ++A[0].xc ].xc = x ;
	    A[ A[0].xc ].yc = y ;
	}

	for( k = 1 ; k <= A[0].xc ; k++ ) {
	    x = A[k].xc ;
	    y = A[k].yc ;
	    if( k == 1 ) {
		minx = x ;
		miny = y ;
		maxx = x ;
		maxy = y ;
	    } else {
		if( x < minx ) {
		    minx = x ;
		}
		if( x > maxx ) {
		    maxx = x ;
		}
		if( y < miny ) {
		    miny = y ;
		}
		if( y > maxy ) {
		    maxy = y ;
		}
	    }
	}
	xcenter = (maxx + minx) / 2 ;
	ycenter = (maxy + miny) / 2 ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &class ) ;
	fscanf( fp , " %d " , &norients ) ;
	fscanf( fp , " %s " , input ) ;
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    fscanf( fp , " %d " , &x ) ;
	}

    } else if( strcmp( input , "pad") == 0 ) {

	ptr = cellarray[ ++cell ] ;
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* pad name */
	for( k = 0 ; k <= 30 ; k++ ) {
	    A[k].xc = 0 ;
	    A[k].yc = 0 ;
	}

	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	    A[ ++A[0].xc ].xc = x ;
	    A[ A[0].xc ].yc = y ;
	}

	for( k = 1 ; k <= A[0].xc ; k++ ) {
	    x = A[k].xc ;
	    y = A[k].yc ;
	    if( k == 1 ) {
		minx = x ;
		miny = y ;
		maxx = x ;
		maxy = y ;
	    } else {
		if( x < minx ) {
		    minx = x ;
		}
		if( x > maxx ) {
		    maxx = x ;
		}
		if( y < miny ) {
		    miny = y ;
		}
		if( y > maxy ) {
		    maxy = y ;
		}
	    }
	}
	xcenter = (maxx + minx) / 2 ;
	ycenter = (maxy + miny) / 2 ;

    } else if( strcmp( input , "padside") == 0 ) {

	fscanf( fp , " %s " , input ) ;

	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &class ) ;
	fscanf( fp , " %d " , &norients ) ;
	fscanf( fp , " %s " , input ) ;
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    fscanf( fp , " %d " , &x ) ;
	}

    } else if( strcmp( input , "sidespace") == 0 ) {
	fscanf( fp , " %lf " , &space ) ;

    } else if( strcmp( input , "softcell") == 0 ) {

	ptr = cellarray[ ++cell ] ;
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* cell name */
	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	}
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &class ) ;
	fscanf( fp , " %d " , &norients ) ;
	fscanf( fp , " %s " , input ) ;
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    fscanf( fp , " %d " , &x ) ;
	}

    } else if( strcmp( input , "pin") == 0 ) {

	fscanf( fp , " %s " , input ) ; /* "name"   */
	fscanf( fp , " %s " , input ) ; /* pin name */
	fscanf( fp , " %s " , input ) ; /* "signal" */
	fscanf( fp , " %s " , input ) ; /* signal name */
	fscanf( fp , " %d %d " , &xpos , &ypos ) ;

	pinctr++ ;

	netx = hashfind( input ) ;
	netptr  = netarray[netx]->netptr ;
	for( ; ; netptr = netptr->nextterm ){ 
	    if( netptr->terminal == pinctr ) {
		break ;
	    }
	}
	netptr->cell = cell ;
	netptr->PorE = 1 ;

	if( ptr->softflag == 0 ) {
	    term = ptr->config[0]->termptr ;
	    for( ; ; term = term->nextterm ){ 
		if( term->terminal == pinctr ) {
		    break ;
		}
	    }
	    term->xpos  = xpos - xcenter ;
	    term->ypos  = ypos - ycenter ;
	    term->oxpos = xpos - xcenter ;
	    term->oypos = ypos - ycenter ;
	}

    } else if( strcmp( input , "equiv") == 0 ) {

	fscanf( fp , " %s " , input ) ; /* "name"   */
	fscanf( fp , " %s " , input ) ; /* pin name */
	pinnames[++ecount + maxterm] = (char *)
			malloc((strlen(input) + 1) * sizeof(char) );
	sprintf( pinnames[ecount + maxterm] , "%s" , input ) ;

	saveptr = netptr->nextterm ;
	netptr  = netptr->nextterm = (NETBOXPTR) 
					 malloc( sizeof(NETBOX) );
	netptr->nextterm = saveptr ;
	netptr->terminal = ecount + maxterm ;
	netptr->xpos     = 0    ;
	netptr->ypos     = 0    ;
	netptr->newx     = 0    ;
	netptr->newy     = 0    ;
	netptr->flag     = 0    ;
	netptr->cell     = cell ;
	netptr->PorE     = 0 ;
	netptr->skip     = 0 ;

	fscanf( fp , " %d %d " , &xpos , &ypos ) ;

	ptr->numterms++ ;
	
	saveterm = term->nextterm ;
	term = term->nextterm = (TERMBOXPTR) malloc( sizeof(TERMBOX) ) ;

	term->nextterm = saveterm ;
	term->terminal = ecount + maxterm ;
	term->xpos  = xpos - xcenter ;
	term->ypos  = ypos - ycenter ;
	term->oxpos = xpos - xcenter ;
	term->oypos = ypos - ycenter ;
	
    } else if( strcmp( input , "asplb") == 0 ) {

	fscanf( fp , "%lf" , &asplb ) ;

    } else if( strcmp( input , "aspub") == 0 ) {

	fscanf( fp , "%lf" , &aspub ) ;

    } else if( strcmp( input , "sequence") == 0 ||
				    strcmp( input , "group") == 0 ) {
	fscanf( fp , " %d " , &sequence ) ;
	for( i = 1 ; i <= sequence ; i++ ) {
	    fscanf( fp , " %s " , input ) ; /* "pin"    */
	    fscanf( fp , " %s " , input ) ; /* "name"   */
	    fscanf( fp , " %s " , input ) ; /* pin name */
	    pinctr++ ;
	    fscanf( fp , " %s " , input ) ; /* "signal" */
	    fscanf( fp , " %s " , input ) ; /* signal name */

	    netx = hashfind( input ) ;
	    netptr  = netarray[netx]->netptr ;
	    for( ; ; netptr = netptr->nextterm ){ 
		if( netptr->terminal == pinctr ) {
		    break ;
		}
	    }
	    netptr->cell = cell ;
	    netptr->PorE = 1 ;
	}
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d %d " , &firstside , &lastside ) ;
    }
}

ggenorien() ;

return ;
}
