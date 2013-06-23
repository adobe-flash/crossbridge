#include "route.h"
int pnodeAlength ;

extern void procesnet( int net , int segments , int addnodes );

void readpnode( FILE *fp )
{

char input[1024] ;
int pin , net , node1 , node2 , distance , i , j , segments , test ;
int pnode ;
LIST2PTR lptr , l2ptr ;
NNODEPTR nptr ;
QUADPTR qptr , q2ptr , tmpqptr ;

maxpnode = 0 ;
pnode = 0 ;
net = 0 ;
numpins = 0 ;

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "net") == 0 ) {
	net++ ;
	if( pnode > maxpnode ) {
	    maxpnode = pnode ;
	}
	pnode = 0 ;
	test = fscanf( fp , " %s " , input ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input a net name ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
    } else if( strcmp( input , "pin") == 0 ) {
	pnode++ ;
	numpins++ ;
	test = fscanf( fp , " %s " , input ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input a pin name ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
	test = fscanf( fp , " %s " , input ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input keyword: nodes ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	} else if( strcmp( input , "nodes" ) != 0 ) {
	    fprintf( fpo, "Failed to input keyword: at ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
	test = fscanf( fp , " %d %d " , &node1 , &node2 ) ;
	if( test != 2 ) {
	    fprintf( fpo, "Failed to input a pair of nodes ");
	    fprintf( fpo, "following keyword nodes in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
	test = fscanf( fp , " %s " , input ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input keyword: at ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	} else if( strcmp( input , "at" ) != 0 ) {
	    fprintf( fpo, "Failed to input keyword: at ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
	test = fscanf( fp , " %d " , &distance ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input an integer following: at ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
    } else if( strcmp( input , "equiv") == 0 ) {
	pnode++ ;
	numpins++ ;
	test = fscanf( fp , " %s " , input ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input a pin name ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
	test = fscanf( fp , " %s " , input ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input keyword: nodes ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	} else if( strcmp( input , "nodes" ) != 0 ) {
	    fprintf( fpo, "Failed to input keyword: at ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
	test = fscanf( fp , " %d %d " , &node1 , &node2 ) ;
	if( test != 2 ) {
	    fprintf( fpo, "Failed to input a pair of nodes ");
	    fprintf( fpo, "following keyword nodes in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
	test = fscanf( fp , " %s " , input ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input keyword: at ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	} else if( strcmp( input , "at" ) != 0 ) {
	    fprintf( fpo, "Failed to input keyword: at ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
	test = fscanf( fp , " %d " , &distance ) ;
	if( test != 1 ) {
	    fprintf( fpo, "Failed to input an integer following: at ");
	    fprintf( fpo, "in the .twf file\n");
	    fprintf( fpo, "Current net: %d\n", net ) ;
	    exit(0);
	}
    } else {
	fprintf( fpo, "Found unknown string: < %s >  ", input );
	fprintf( fpo, "in the .twf file\n") ;
	fprintf( fpo, "Current net: %d\n", net ) ;
	exit(0) ;
    }
}
if( pnode > maxpnode ) {
    maxpnode = pnode ;
}

rewind( fp ) ;

gnodeArray = (GNODEPTR *) realloc( gnodeArray, (1 + numnodes +
			    maxpnode ) * sizeof( GNODEPTR ) ) ;
for( i = numnodes + 1 ; i <= numnodes + maxpnode ; i++ ) {
    gnodeArray[i] = (GNODEPTR) NULL ;
}

gtrace = (short int **) malloc( 5 * sizeof( short int * ) ) ;
for( i = 1 ; i <= 4 ; i++ ) {
    gtrace[i] = (short int *) malloc( (1 + maxpnode) * 
					sizeof( short int ) ) ;
}

largestNet = net ;
netRoutes = (NRBOXPTR) malloc( (1 + largestNet) * sizeof(NRBOX) ) ;
for( i = 1 ; i <= largestNet ; i++ ) {
    netRoutes[i].currentRoute = 1 ;
    netRoutes[i].alternate = (ALTBOXPTR *) malloc(
				(1 + MAXPATHS) * sizeof(ALTBOXPTR) ) ;
    for( j = 1 ; j <= MAXPATHS ; j++ ) {
	netRoutes[i].alternate[j] = (ALTBOXPTR)malloc(sizeof(ALTBOX));
	netRoutes[i].alternate[j]->pinList = (int *) NULL ;
	netRoutes[i].alternate[j]->chanList = (CHANBOXPTR) NULL ;
    }
}

savePaths = (short int **) malloc( (1 + 2 * MAXPATHS) * 
					sizeof( short int *) ) ;
for( i = 1 ; i <= 2 * MAXPATHS ; i++ ) {
    savePaths[i] = (short int *) malloc( (1 + 3 * (numnodes + maxpnode)) * 
					sizeof(short int) ) ;
}

pnameArray = (char **) malloc( (1 + numpins) * sizeof( char *) ) ;
for( i = 1 ; i <= numpins ; i++ ) {
    pnameArray[i] = (char *) NULL ;
}

nnameArray = (char **) malloc( (1 + net) * sizeof( char *) ) ;
for( i = 1 ; i <= net ; i++ ) {
    nnameArray[i] = (char *) NULL ;
}

pinOffset = (int *) malloc( (1 + net) * sizeof( int ) ) ;
for( i = 1 ; i <= net ; i++ ) {
    pinOffset[i] = 0 ;
}

segList = (short int *) malloc((1 + 3 * (numnodes + maxpnode)) * 
						sizeof(short int));
sourceList = (short int *) malloc( (1 + maxpnode) * 
						sizeof( short int ) ) ;
targetList = (short int *) malloc( (1 + maxpnode + numnodes) * 
						sizeof(short int) );
delSourceList = (short int *) malloc( (1 + maxpnode) * 
						sizeof( short int ) ) ;
addTargetList = (short int *) malloc( (1 + maxpnode + numnodes) * 
						sizeof(short int) );

pathList = (int **) malloc((1 + EXTRASOURCES * MAXPATHS) * 
						sizeof(int *));
for( i = 1 ; i <= EXTRASOURCES * MAXPATHS ; i++ ) {
    pathList[i] = (int *) malloc( (2 + numnodes + maxpnode) * 
						sizeof(int));
    for( j = 0 ; j <= numnodes + maxpnode + 1 ; j++ ) {
	pathList[i][j] = 0 ;
    }
}

pathArray = (PATHPTR) malloc( (3 + 2 * MAXPATHS) * sizeof( PATH ) ) ;
for( i = 1 ; i <= 2 + 2 * MAXPATHS ; i++ ) {
    pathArray[i].nodeset = (short int *) malloc( 
			(1 + numnodes + maxpnode) * sizeof(short int));
}
tempArray = (short int *) malloc( (1 + numnodes + maxpnode) * 
						sizeof(short int));


pnode = 0 ;
pnodeAlength = 0 ;
net = 0 ;
pin = 0 ;
pinlist = (QUADPTR) NULL ;
qptr = (QUADPTR) NULL ;

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "net") == 0 ) {
	if( net > 0 ) {
	    if( net % 10 == 0 ) {
		fprintf(fpo,"Global Routing Net Number: %d\n", net );
		fflush(fpo);
	    }
	    if( (pnode > pnodeAlength) || 
		    ((pnodeAlength > 30) && (2 * pnode < pnodeAlength) )){
		if( pnodeAlength > 0 ) { 
		    for( i = 1 ; i <= pnodeAlength ; i++ ) {
			lptr = pnodeArray[i].equiv ;
			while( lptr != (LIST2PTR) NULL ) {
			    l2ptr = lptr->next ;
			    free( lptr ) ;
			    lptr = l2ptr ;
			}
			nptr = pnodeArray[i].nodeList ;
			if( nptr != (NNODEPTR) NULL ) {
			    free( pnodeArray[i].nodeList ) ;
			}
		    }
		    for( i = 0 ; i < pnodeAlength ; i++ ) {
			free( netSegArray[i] ) ;
		    }
		    free( (PNODEPTR) pnodeArray ) ;
		    free( (short int **) netSegArray ) ;
		}
		pnodeAlength = pnode ;
		pnodeArray = (PNODEPTR) malloc( (1 + pnode) * 
						sizeof(PNODE) ) ;
		for( i = 1 ; i <= pnode ; i++ ) {
		    pnodeArray[i].nodeList = 
				    (NNODEPTR) malloc( (1 + numnodes +
				    pnode ) * sizeof(NNODE) ) ;
		    pnodeArray[i].eptr = 0 ;
		    pnodeArray[i].equiv = (LIST2PTR) NULL ;
		}
		netSegArray = (short int **) malloc( pnode * 
					    sizeof( short int *) ) ;
		for( i = 0 ; i < pnode ; i++ ) {
		    netSegArray[i] = (short int *) 
				    malloc( (1 + numnodes + pnode) *
				    sizeof( short int ) ) ;
		}
	    }

	    procesnet( net , segments , pnode ) ;
	}
	net++ ;
	segments = 0 ;
	pinOffset[net] = pin ;
	fscanf( fp , " %s " , input ) ;
	nnameArray[net] = (char *) malloc( (strlen(input) + 1) * 
						sizeof(char) ) ;
	sprintf( nnameArray[net], "%s", input ) ;
	pnode = 0 ;
	while( qptr != (QUADPTR) NULL ) {
	    q2ptr = qptr->prev ;
	    free( qptr ) ;
	    qptr = q2ptr ;
	}
	qptr = (QUADPTR) NULL ;
	pinlist = (QUADPTR) NULL ;
    } else if( strcmp( input , "pin") == 0 ) {
	pin++ ;
	pnode++ ;
	segments++ ;
	fscanf( fp , " %s " , input ) ;
	pnameArray[pin] = (char *) malloc( (strlen(input) + 1) * 
						sizeof(char) ) ;
	sprintf( pnameArray[pin], "%s", input ) ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d %d " , &node1 , &node2 ) ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &distance ) ;
	if( qptr == (QUADPTR) NULL ) {
	    pinlist = qptr = (QUADPTR) malloc( sizeof(QUAD) ) ;
	    qptr->prev = (QUADPTR) NULL ;
	} else {
	    tmpqptr = qptr ;
	    qptr = qptr->next = (QUADPTR) malloc( sizeof(QUAD) ) ;
	    qptr->prev = tmpqptr ;
	}
	qptr->next = (QUADPTR) NULL ;
	qptr->PorE = 1 ;
	qptr->node1 = node1 ;
	qptr->node2 = node2 ;
	qptr->distance = distance ;
    } else if( strcmp( input , "equiv") == 0 ) {
	pin++ ;
	pnode++ ;
	fscanf( fp , " %s " , input ) ;
	pnameArray[pin] = (char *) malloc( (strlen(input) + 1) * 
						sizeof(char) ) ;
	sprintf( pnameArray[pin], "%s", input ) ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d %d " , &node1 , &node2 ) ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &distance ) ;
	tmpqptr = qptr ;
	qptr = qptr->next = (QUADPTR) malloc( sizeof(QUAD) ) ;
	qptr->prev = tmpqptr ;
	qptr->next = (QUADPTR) NULL ;
	qptr->PorE = -1 ;
	qptr->node1 = node1 ;
	qptr->node2 = node2 ;
	qptr->distance = distance ;
    }
}
if( (pnode > pnodeAlength) || (10 * pnode < pnodeAlength) ) {
    if( pnodeAlength > 0 ) { 
	for( i = 1 ; i <= pnodeAlength ; i++ ) {
	    lptr = pnodeArray[i].equiv ;
	    while( lptr != (LIST2PTR) NULL ) {
		l2ptr = lptr->next ;
		free( lptr ) ;
		lptr = l2ptr ;
	    }
	    nptr = pnodeArray[i].nodeList ;
	    if( nptr != (NNODEPTR) NULL ) {
		free( pnodeArray[i].nodeList ) ;
	    }
	}
	for( i = 0 ; i < pnodeAlength ; i++ ) {
	    free( netSegArray[i] ) ;
	}
	free( (PNODEPTR) pnodeArray ) ;
	free( (short int **) netSegArray ) ;
    }
    pnodeAlength = pnode ;
    pnodeArray = (PNODEPTR) malloc( (1 + pnode) * 
				    sizeof(PNODE) ) ;
    for( i = 1 ; i <= pnode ; i++ ) {
	pnodeArray[i].nodeList = 
			(NNODEPTR) malloc( (1 + numnodes +
			pnode ) * sizeof(NNODE) ) ;
	pnodeArray[i].eptr = 0 ;
	pnodeArray[i].equiv = (LIST2PTR) NULL ;
    }
    netSegArray = (short int **) malloc( pnode * 
				sizeof( short int *) ) ;
    for( i = 0 ; i < pnode ; i++ ) {
	netSegArray[i] = (short int *) 
			malloc( (1 + numnodes + pnode) *
			sizeof( short int ) ) ;
    }
}

procesnet( net , segments , pnode ) ;

return ;
}
