#include "custom.h"

void maketabl(void)
{
int i ;

hashtab = ( HASHPTR * ) malloc( 1009 * sizeof( HASHPTR ) ) ;
for( i = 0 ; i < 1009 ; i++ ) {
    hashtab[i] = ( HASHPTR ) NULL ;
}
return ;
}



void delHtab(void)
{

int i ;
HASHPTR hptr , zapptr ;

for( i = 0 ; i < 1009 ; i++ ) {
    hptr = hashtab[i] ;
    if( hptr != (HASHPTR) NULL ) {
	zapptr = hptr ;
	while( hptr->hnext != (HASHPTR) NULL ) {
	    hptr = hptr->hnext ;
	    free( zapptr ) ;
	    zapptr = hptr ;
	}
	free( hptr ) ;
    }
}
free( hashtab ) ;
return ;
}


int addhash( char hname[] )
{

int i ;
HASHPTR hptr ;
unsigned int hsum = 0 ;

for( i = 0 ; i < strlen( hname ) ; i++ ) {
    hsum += ( unsigned int ) hname[i] ;
}
hsum %= 1009 ;
if( (hptr = hashtab[hsum]) == (HASHPTR) NULL ) {
    hptr = hashtab[hsum] = (HASHPTR) malloc( sizeof( HASHBOX ) ) ;
    hptr->hnext = (HASHPTR) NULL ;
    hptr->hnum = ++netctr ;
    hptr->hname = (char *)malloc( (strlen(hname) + 1) * sizeof( char ));
    sprintf( hptr->hname , "%s" , hname ) ;
    return(1) ;
} else {
    for( ; ; ) {
	if( strcmp( hname , hptr->hname ) == 0 ) {
	    return(0) ;
	}
	if( hptr->hnext == (HASHPTR) NULL ) {
	    hptr = hptr->hnext = (HASHPTR) malloc( sizeof(HASHBOX));
	    hptr->hnext = (HASHPTR) NULL ;
	    hptr->hnum = ++netctr ;
	    hptr->hname = (char *)malloc((strlen(hname)+ 1) * 
							sizeof(char));
	    sprintf( hptr->hname , "%s" , hname ) ;
	    return(1) ;
	} else {
	    hptr = hptr->hnext ;
	}
    }
}
}


int hashfind( char hname[] )
{

int i ;
HASHPTR hptr ;
unsigned int hsum = 0 ;

for( i = 0 ; i < strlen( hname ) ; i++ ) {
    hsum += ( unsigned int ) hname[i] ;
}
hsum %= 1009 ;
if( (hptr = hashtab[hsum]) == (HASHPTR) NULL ) {
    return(0) ;
} else {
    for( ; ; ) {
	if( strcmp( hname , hptr->hname ) == 0 ) {
	    return( hptr->hnum ) ;
	}
	if( hptr->hnext == (HASHPTR) NULL ) {
	    return(0) ;
	} else {
	    hptr = hptr->hnext ;
	}
    }
}
}
