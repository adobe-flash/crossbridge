#include "route.h"
#include "geo.h"
extern int doChannelGraph ;
extern int doCompaction ;
extern int finalShot ;
extern int routerMaxPaths ;
extern int routerExtraS ;
extern int bareFlag ;
extern int pnodeAlength ;
int bareMinimum ;

extern void readgraph( FILE *fp );
extern void readpnode( FILE *fp );
extern void printnets(void);
extern void density(void);
extern void bellman(void);
extern void tpop(TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);
void rmain(void)
{

FILE *fp;
char filename[1024] ;
GNODEPTR gptr , g2ptr ;
QUADPTR qptr , tmpqptr ;
CHANBOXPTR cptr , c2ptr ;
NNODEPTR nptr ;
TNODEPTR tnode ;
LIST2PTR lptr , l2ptr ;
WCPTR wcptr , wc2ptr ;
int i , j , dummy1 , dummy2 ;



if( finalShot == doCompaction ) {
    if( bareFlag == 0 ) {
	MAXPATHS = routerMaxPaths ;
	EXTRASOURCES = routerExtraS ;
	bareMinimum = 0 ;
    } else {
	MAXPATHS = 1 ;
	EXTRASOURCES = 1 ;
	bareMinimum = 1 ;
    }
} else {
    MAXPATHS = 1 ;
    EXTRASOURCES = 1 ;
    bareMinimum = 1 ;
}


sprintf( filename , "%s.gph" , cktName ) ;
if( (fp = fopen ( filename , "r")) == (FILE *) NULL ) {
    fprintf(fpo,"Error: file: %s not present\n", filename );
    exit(0);
}
readgraph( fp ) ;

sprintf( filename , "%s.twf" , cktName ) ;
if( (fp = fopen ( filename , "r")) == (FILE *) NULL ) {
    printf("can't open %s\n", filename ) ;
    exit(0);
}
readpnode( fp ) ;

printnets() ;

if( doChannelGraph ) {
    density() ;
    bellman() ;
}




/*   ***************************************   */
/*   EVERYTHING BELOW IS CLEAN-UP OPERATIONS   */
/*   ***************************************   */
fclose(fp);


/*  We will want to remove this if we implement the s.a.
    for the various net routes
*/
for( i = 1 ; i <= eNum ; i++ ) {
    if( eArray[i].root != (TNODEPTR) NULL ) {
	for( ; ; ) {
	    tpop( &eArray[i].root , &tnode , &dummy1 , &dummy2 ) ;
	    if( tnode == (TNODEPTR) NULL ) {
		break ;
	    }
	}
    }
}
free( eArray ) ;

for( i = 1 ; i <= numRects ; i++ ) {
    free( eIndexArray[i] ) ;
}
free( eIndexArray ) ;

for( i = 1 ; i <= numXnodes ; i++ ) {
    wcptr = xNodeArray[i] ;
    while( wcptr != (WCPTR) NULL ) {
	if( wcptr->channels != (int *) NULL ) {
	    free( wcptr->channels ) ;
	}
	wc2ptr = wcptr->next ;
	free( wcptr ) ;
	wcptr = wc2ptr ;
    }
}
for( i = 1 ; i <= numYnodes ; i++ ) {
    wcptr = yNodeArray[i] ;
    while( wcptr != (WCPTR) NULL ) {
	if( wcptr->channels != (int *) NULL ) {
	    free( wcptr->channels ) ;
	}
	wc2ptr = wcptr->next ;
	free( wcptr ) ;
	wcptr = wc2ptr ;
    }
}
free( xNodeArray ) ;
free( yNodeArray ) ;

free( edgeList ) ;


free( xBellArray ) ;
free( yBellArray ) ;

for( i = 1 ; i <= pnodeAlength ; i++ ) {
    lptr = pnodeArray[i].equiv ;
    while( lptr != (LIST2PTR) NULL ) {
	l2ptr = lptr->next ;
	free( lptr ) ;
	lptr = l2ptr ;
    }
    nptr =  pnodeArray[i].nodeList ;
    if( nptr != (NNODEPTR) NULL ) {
	free( pnodeArray[i].nodeList ) ;
    }
}
free( pnodeArray ) ;

for( i = 1 ; i <= numnodes + maxpnode ; i++ ) {
    gptr = gnodeArray[i] ;
    while( gptr != (GNODEPTR) NULL ) {
	g2ptr = gptr->next ;
	free( gptr ) ;
	gptr = g2ptr ;
    }
}
free( gnodeArray ) ;



for( i = 1 ; i <= 4 ; i++ ) {
    free( gtrace[i] );
}
free( gtrace );

for( i = 1 ; i <= largestNet ; i++ ) {
    for( j = 1 ; j <= MAXPATHS ; j++ ) {
	cptr = netRoutes[i].alternate[j]->chanList ;
	while( cptr != (CHANBOXPTR) NULL ) {
	    c2ptr = cptr->next ;
	    free( cptr ) ;
	    cptr = c2ptr ;
	}
	free( netRoutes[i].alternate[j]->pinList ) ;
	free( netRoutes[i].alternate[j] ) ;
    }
    free( netRoutes[i].alternate ) ;
}
free( netRoutes );

for( i = 1 ; i <= 2 * MAXPATHS ; i++ ) {
    free( savePaths[i] ) ; 
}
free( savePaths ) ;

for( i = 1 ; i <= numpins ; i++ ) {
    if( pnameArray[i] != (char *) NULL ) {
	free( pnameArray[i] ) ;
    }
}
free( pnameArray ) ;

for( i = 1 ; i <= largestNet ; i++ ) {
    if( nnameArray[i] != (char *) NULL ) {
	free( nnameArray[i] ) ;
    }
}
free( nnameArray ) ;

free( pinOffset ) ;

free( segList ) ;

for( i = 0 ; i < pnodeAlength ; i++ ) {
    free( netSegArray[i] ) ;
}
free( netSegArray ) ;

free( sourceList ) ; 
free( targetList ) ; 
free( delSourceList ) ; 
free( addTargetList ) ;

for( i = 1 ; i <= EXTRASOURCES * MAXPATHS ; i++ ) {
    free( pathList[i] ) ;
}
free( pathList ) ;

for( i = 1 ; i <= 2 + 2 * MAXPATHS ; i++ ) {
    free( pathArray[i].nodeset ) ;
}
free( pathArray ) ;

free( tempArray ) ;

for( qptr = pinlist ; qptr != (QUADPTR) NULL ; ) {
    tmpqptr = qptr->next ;
    free( qptr ) ;
    qptr = tmpqptr ;
}

return ;
}
