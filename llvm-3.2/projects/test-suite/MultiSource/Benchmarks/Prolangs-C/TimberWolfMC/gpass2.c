#include "custom.h"
extern BUSTBOXPTR C ;

extern void maketabl(void);
extern int addhash( char hname[] );

void gpass2( FILE *fp )
{

int i ;
int xpos , ypos , class ;
int sequence , firstside , lastside ;
int cellnum , pinctr , term ;
int corner , ncorners , x , y , ecount , orient , norients ;
double aspub , asplb , space ;
char input[1024] ;

ecount = 0 ;
netctr = 0 ;
pinctr = 0 ;
maketabl() ;

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "cell") == 0 ) {
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
	fscanf( fp , " %s " , input ) ; /* "signal"   */
	fscanf( fp , " %s " , input ) ; /* signal name */
	pinctr++ ;
	addhash( input ) ;
	fscanf( fp , " %d %d " , &xpos , &ypos );
    } else if( strcmp( input , "equiv") == 0 ) {
	ecount++ ;
	fscanf( fp , " %s " , input ) ; /* "name"   */
	fscanf( fp , " %s " , input ) ; /* pin name */
	fscanf( fp , " %d %d " , &xpos , &ypos );
    } else if( strcmp( input , "softcell") == 0 ) {
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* cell name */
	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
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
	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
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

termarray = (TERMNETSPTR *) realloc( termarray , 
		(pinctr + ecount + 1) * sizeof( TERMNETSPTR ) );
for( term = 1 + pinctr ; term <= pinctr + ecount ; term++ ) {
    termarray[ term ] = TERMNETSNULL ;
}

pinnames = (char **) realloc( pinnames, 
			(pinctr + ecount + 1) * sizeof(char *) );
return ;
}
