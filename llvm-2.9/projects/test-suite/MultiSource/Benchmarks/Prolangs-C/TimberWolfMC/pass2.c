#include "custom.h"
int totPins ;
BUSTBOXPTR C ;
/*
 *  One of the main purposes of pass2 is to ascertain the number of 
 *  uncommitted pins per cell
 */

extern void maketabl(void);
extern int addhash( char hname[] );

void pass2( FILE *fp )
{

int i , cell , class ;
int xpos , ypos ;
int k , xx1 , xx2 , yy1 , yy2 ;
int sequence , firstside , lastside ;
int soft , cellnum , pinctr , term ;
int corner , ncorners , x , y , ecount , orient , norients ;
double aspub , asplb , space ;
char input[1024] ;
CELLBOXPTR ptr ;
DIMBOXPTR nptr ;

C = ( BUSTBOXPTR ) malloc( 101 * sizeof( BUSTBOX ) ) ;
cell = 0 ;
ecount = 0 ;
netctr = 0 ;
pinctr = 0 ;
maketabl() ;

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "cell") == 0 ) {
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* cell name */
	ptr = cellarray[ ++cell ] ;
	soft = 0 ;
	for( k = 0 ; k <= 100 ; k++ ) {
	    C[k].xc = 0 ;
	    C[k].yc = 0 ;
	}
	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	    C[ ++C[0].xc ].xc = x ;
	    C[ C[0].xc ].yc = y ;
	}
	for( k = 1 ; k <= C[0].xc ; k++ ) {
	    xx1 = C[k].xc ;
	    yy1 = C[k].yc ;
	    if( k == C[0].xc ) {
		xx2 = C[1].xc ;
		yy2 = C[1].yc ;
	    } else {
		xx2 = C[ k + 1 ].xc ;
		yy2 = C[ k + 1 ].yc ;
	    }
	    if( xx1 == xx2 && yy1 == yy2 ) {
		fprintf(fpo,"a zero length side was found for\n");
		fprintf(fpo,"the current cell:%d\n", cell );
		exit(0);
	    }
	    if( xx1 != xx2 && yy1 != yy2 ) {
		fprintf(fpo,"a non rectilinear side was found \n");
		fprintf(fpo,"for the current cell:%d\n", cell );
		exit(0);
	    }
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
	fscanf( fp , " %s " , input ) ; /* "signal"   */
	fscanf( fp , " %s " , input ) ; /* signal name */
	pinctr++ ;
	addhash( input ) ;
	fscanf( fp , " %d %d " , &xpos , &ypos );
	if( soft == 1 && ptr->numUnComTerms != 0 ) {
	    for( k = 1 ; k <= C[0].xc ; k++ ) {
		xx1 = C[k].xc ;
		yy1 = C[k].yc ;
		if( k == C[0].xc ) {
		    xx2 = C[1].xc ;
		    yy2 = C[1].yc ;
		} else {
		    xx2 = C[ k + 1 ].xc ;
		    yy2 = C[ k + 1 ].yc ;
		}
		if( xx1 == xx2 ) {
		    if( xpos == xx1 ) {
			ptr->numUnComTerms++ ;
			break ;
		    }
		} else if( ypos == yy1 ) {
		    ptr->numUnComTerms++ ;
		    break ;
		}
	    }
	}
    } else if( strcmp( input , "equiv") == 0 ) {
	if( soft == 1 ) {
	    fprintf(fpo,"equiv pin cannot reside on soft cell\n");
	    fprintf(fpo,"problematic cell: %d\n", cell );
	    exit(0);
	}
	ecount++ ;
	fscanf( fp , " %s " , input ) ; /* "name"   */
	fscanf( fp , " %s " , input ) ; /* pin name */
	fscanf( fp , " %d %d " , &xpos , &ypos );
    } else if( strcmp( input , "softcell") == 0 ) {
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* cell name */
	ptr = cellarray[ ++cell ] ;
	soft = 1 ;
	for( k = 0 ; k <= 100 ; k++ ) {
	    C[k].xc = 0 ;
	    C[k].yc = 0 ;
	}
	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	    C[ ++C[0].xc ].xc = x ;
	    C[ C[0].xc ].yc = y ;
	}
	for( k = 1 ; k <= C[0].xc ; k++ ) {
	    xx1 = C[k].xc ;
	    yy1 = C[k].yc ;
	    if( k == C[0].xc ) {
		xx2 = C[1].xc ;
		yy2 = C[1].yc ;
	    } else {
		xx2 = C[ k + 1 ].xc ;
		yy2 = C[ k + 1 ].yc ;
	    }
	    if( xx1 == xx2 && yy1 == yy2 ) {
		fprintf(fpo,"a zero length side was found for\n");
		fprintf(fpo,"the current cell:%d\n", cell );
		exit(0);
	    }
	    if( xx1 != xx2 && yy1 != yy2 ) {
		fprintf(fpo,"a non rectilinear side found \n");
		fprintf(fpo,"for the current cell:%d\n", cell );
		exit(0);
	    }
	}
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , "%lf" , &asplb ) ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , "%lf" , &aspub ) ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &class ) ;
	fscanf( fp , " %d " , &norients ) ;
	fscanf( fp , " %s " , input ) ;
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    fscanf( fp , " %d " , &x ) ;
	}
    } else if( strcmp( input , "sequence") == 0 ||
				    strcmp( input , "group") == 0 ) {
	fscanf( fp , " %d " , &sequence ) ;
	ptr->numUnComTerms += sequence ;
	for( i = 1 ; i <= sequence ; i++ ) {
	    fscanf( fp , " %s " , input ) ; /* "pin"    */
	    fscanf( fp , " %s " , input ) ; /* "name"   */
	    fscanf( fp , " %s " , input ) ; /* pin name */
	    fscanf( fp , " %s " , input ) ; /* "signal"   */
	    fscanf( fp , " %s " , input ) ; /* signal name */
	    pinctr++ ;
	    addhash( input ) ;
	}
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d %d " , &firstside , &lastside ) ;
    } else if( strcmp( input , "pad") == 0 ) {
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* cell name */
	ptr = cellarray[ ++cell ] ;
	soft = 0 ;
	for( k = 0 ; k <= 100 ; k++ ) {
	    C[k].xc = 0 ;
	    C[k].yc = 0 ;
	}
	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	    C[ ++C[0].xc ].xc = x ;
	    C[ C[0].xc ].yc = y ;
	}
	for( k = 1 ; k <= C[0].xc ; k++ ) {
	    xx1 = C[k].xc ;
	    yy1 = C[k].yc ;
	    if( k == C[0].xc ) {
		xx2 = C[1].xc ;
		yy2 = C[1].yc ;
	    } else {
		xx2 = C[ k + 1 ].xc ;
		yy2 = C[ k + 1 ].yc ;
	    }
	    if( xx1 == xx2 && yy1 == yy2 ) {
		fprintf(fpo,"a zero length side was found for\n");
		fprintf(fpo,"the current cell:%d\n", cell );
		exit(0);
	    }
	    if( xx1 != xx2 && yy1 != yy2 ) {
		fprintf(fpo,"a non rectilinear side found \n");
		fprintf(fpo,"for the current cell:%d\n", cell );
		exit(0);
	    }
	}
	fscanf( fp , " %s " , input ) ;
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
    }
}
rewind( fp ) ;

termarray = (TERMNETSPTR *) malloc( (pinctr + 1) * 
				    sizeof( TERMNETSPTR ) );
for( term = 1 ; term <= pinctr ; term++ ) {
    termarray[ term ] = TERMNETSNULL ;
}
maxterm = pinctr ;

pinnames = (char **) malloc( (pinctr + 1) * sizeof( char * ) ) ;

numnets = netctr ;
netarray = (DIMBOXPTR *) malloc( (netctr + 1) * sizeof(DIMBOXPTR));
for( i = 1 ; i <= netctr ; i++ ) {
    nptr = netarray[ i ] = (DIMBOXPTR) malloc( sizeof( DIMBOX ) ) ;
    nptr->netptr  = NETNULL ;
    nptr->nname   = NULL    ;
    nptr->skip    = 0       ;
    nptr->flag    = 0       ;
    nptr->xmin    = 0       ;
    nptr->newxmin = 0       ;
    nptr->xmax    = 0       ;
    nptr->newxmax = 0       ;
    nptr->ymin    = 0       ;
    nptr->newymin = 0       ;
    nptr->ymax    = 0       ;
    nptr->newymax = 0       ;
}

totPins = ecount + maxterm ;
return ;

}
