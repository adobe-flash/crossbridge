/* #define DEBUG1 */
/* #define DEBUG2 */
#include "route.h"
#include "23tree.h"
extern TNODEPTR netRoot ;
TNODEPTR indexRoot ;
int treeSize ;
int pathIndex ;
extern int bareMinimum ;

extern void tinsert( TNODEPTR *root , int value , int property );
extern void recursePath( int dsptr , int atptr , int attachPt, int addEquivs);
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);
extern int mshortest( int source , int soleTarget );
extern void tdelete( TNODEPTR *root , int value , int property );
extern void tmax( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);

void routenet( int segments , int numpnodes )
{

int i , j , savei , min , count , node , node1 , bareflag ;
GNODEPTR gptr , gptr1 ; 
NNODEPTR nptr ;
LIST2PTR lptr ;
TNODEPTR tptr ;


if( segments > 2 ) {

    i = MAXPATHS / (segments - 1) ;
    Mpaths = ( i > 1 ) ? i : 1 ;

    while( (pow( (double) Mpaths , (double)(segments - 1) ) > 
			    80.0 * (double) MAXPATHS ) && (Mpaths > 1) ) {
	Mpaths-- ;
    }
} else {
    Mpaths = 1 ;
}

bareflag = 0 ;
if( Mpaths == 1 && segments > 19 ) {
    if( bareMinimum == 0 ) {
	bareMinimum = 1 ;
	bareflag = 1 ;
    }
}

indexRoot = (TNODEPTR) NULL ;
treeSize = 0 ;
for( i = 1 ; i <= 2 * MAXPATHS ; i++ ) {
    tinsert( &indexRoot , i , 0 ) ;
}

for( i = 1 ; i <= numpnodes ; i++ ) {
    sourceList[i] = i + numnodes ;
    delSourceList[i] = 0 ;
}
sourcePtr = numpnodes ;
for( i = 1 ; i <= numnodes + numpnodes ; i++ ) {
    targetList[i] = 0 ;
    addTargetList[i] = 0 ;
}
targetPtr = 0 ;
segPtr = 0 ;

min = VLARGE ;
for( i = 1 + numnodes ; i <= numpnodes + numnodes ; i++ ) {
    nptr = pnodeArray[ i - numnodes ].nodeList ;
    for( j = 1 + numnodes ; j <= numpnodes + numnodes ; j++ ) {
	if( i == j ) {
	    continue ;
	}
	if( (pnodeArray[i - numnodes].eptr != 0) && 
			    (pnodeArray[j - numnodes].eptr != 0) ) {
	    if( pnodeArray[i - numnodes].eptr == 
				pnodeArray[j - numnodes].eptr ) {
		continue ;
	    }
	} else if( (pnodeArray[i - numnodes].eptr == 0) && 
			    (pnodeArray[j - numnodes].eptr == 0) ) {
	} else {
	    if( pnodeArray[i - numnodes].eptr != 0 ) {
		if( pnodeArray[i - numnodes].eptr == j - numnodes ) {
		    continue ;
		}
	    } else {
		if( pnodeArray[j - numnodes].eptr == i - numnodes ) {
		    continue ;
		}
	    }
	}

	if( nptr[j].distance < min ) {
	    min = nptr[j].distance ;
	    savei = i ;
	}
    }
}
pathLength = 0 ;
pathIndex = 0 ;
delSourceList[1] = savei ;
addTargetList[1] = savei ;

count = 0 ;
if( pnodeArray[ savei - numnodes ].eptr != 0 ) {
    count = 1 ;
    node = pnodeArray[ savei - numnodes ].eptr + numnodes ;
    delSourceList[2] = node ;
    addTargetList[2] = node ;
    lptr = pnodeArray[ pnodeArray[ savei - numnodes ].eptr ].equiv ;
    while( lptr != (LIST2PTR) NULL ) {
	node = lptr->node + numnodes ;
	if( node == savei ) {
	    lptr = lptr->next ;
	    continue ;
	}
	delSourceList[ ++count + 1 ] = node ;
	addTargetList[ count + 1 ] = node ;
	lptr = lptr->next ;
    }
} else {
    lptr = pnodeArray[ savei - numnodes ].equiv ;
    while( lptr != (LIST2PTR) NULL ) {
	node = lptr->node + numnodes ;
	delSourceList[ ++count + 1 ] = node ;
	addTargetList[ count + 1 ] = node ;
	lptr = lptr->next ;
    }
}

for( i = 1 ; i <= count + 1 ; i++ ) {
    node = addTargetList[i] ;
    gptr = gnodeArray[node] ;
    for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	node1 = gptr->node ;
	gptr1 = gnodeArray[node1] ;
	for( ; ; ) {
	    if( gptr1->node == node ) {
		gptr1->einactive = 1 ;
		break ;
	    }
	    gptr1 = gptr1->next ;
	}
    }
}

recursePath( count + 1 , 0 , 0 , count + 1 ) ;

for( i = 1 ; i <= count + 1 ; i++ ) {
    node = addTargetList[i] ;
    gptr = gnodeArray[node] ;
    for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	node1 = gptr->node ;
	gptr1 = gnodeArray[node1] ;
	for( ; ; ) {
	    if( gptr1->node == node ) {
		gptr1->einactive = 0 ;
		break ;
	    }
	    gptr1 = gptr1->next ;
	}
    }
}
if( indexRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &indexRoot , &tptr , &i , &j ) ;
	if( tptr == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}

if( bareflag == 1 ) {
    bareMinimum = 0 ;
}
return ;
}



void recursePath( int dsptr , int atptr , int attachPt, int addEquivs )
{

int i , j , k , l , min , dnode , savei , node , node1 , node2 , t ;
int numberPaths , count , numadd ;
int numdelete , junk , c , value , index , found , savej ;
int extraS , numExtra , s , *extraSlist , x ;
short int *dslist , *atlist ;
int **tempPL , pathExists , targ , from ;
NNODEPTR nptr ;
TNODEPTR junkptr , extraRoot ;
GNODEPTR gptr , g1ptr , gptr1 , gptr2 ;
LIST2PTR lptr ;

#ifdef DEBUG1
printf("\n\nTarget List:\n");
for( i = 1 ; i <= targetPtr ; i++ ) {
    printf(" %d ", targetList[i] ) ;
}
printf("\n");
printf("add to targets:\n");
for( i = 1 ; i <= atptr + addEquivs ; i++ ) {
    printf(" %d ", addTargetList[i] ) ;
}
printf("\n");
fflush(stdout);
#endif

if( sourcePtr - dsptr == 0 ) {
    tpop( &indexRoot , &junkptr , &pathIndex , &junk ) ;
    savePaths[pathIndex][0] = segPtr ;
    for( i = 1 ; i <= segPtr ; i++ ) {
	savePaths[pathIndex][i] = segList[i] ;
    }
#ifdef DEBUG1
    printf("pathLength:%d  pathIndex:%d\n", pathLength, pathIndex);
    for( i = 1 ; i <= segPtr ; i++ ) {
	printf(" %d ", savePaths[pathIndex][i] );
    }
    printf("\n");
    fflush(stdout);
#endif
    tinsert( &netRoot , pathLength , pathIndex ) ;
    treeSize++ ;
    return ;
}

extraRoot = (TNODEPTR) NULL ;

extraSlist = (int *) malloc( (1 + EXTRASOURCES) * sizeof(int) ) ;
tempPL = (int **) malloc((1 + EXTRASOURCES * MAXPATHS) * sizeof(int *));
for( i = 1 ; i <= EXTRASOURCES * MAXPATHS ; i++ ) {
    tempPL[i] = (int *) malloc( 21 * sizeof(int));
    for( j = 0 ; j <= 20 ; j++ ) {
	tempPL[i][j] = 0 ;
    }
}

dslist = (short int *) malloc( (1 + dsptr) * sizeof(short int) ) ;
for( i = 1 ; i <= dsptr ; i++ ) {
    dslist[i] = delSourceList[i] ;
}
atlist = (short int *) malloc( (1 + atptr + addEquivs) * 
						sizeof(short int) ) ;
for( i = 1 ; i <= atptr + addEquivs ; i++ ) {
    atlist[i] = addTargetList[i] ;
}


#ifdef DEBUG2
printf("Deleted Source Node List:\n");
for( i = 1 ; i <= dsptr ; i++ ) {
    printf(" %d ", dslist[i] ) ;
}
printf("\nSource Node List (BEFORE):\n");
for( i = 1 ; i <= sourcePtr ; i++ ) {
    printf(" %d ", sourceList[i] ) ;
}
fflush(stdout);
#endif

for( j = 1 ; j <= dsptr ; j++ ) {
    dnode = dslist[j] ;
    for( i = 1 ; i <= sourcePtr ; i++ ) {
	if( sourceList[i] == dnode ) {
	    sourceList[i] = sourceList[sourcePtr--] ;
	    break ;
	}
    }
}

#ifdef DEBUG2
printf("\nSource Node List (AFTER):\n");
for( i = 1 ; i <= sourcePtr ; i++ ) {
    printf(" %d ", sourceList[i] ) ;
}
fflush(stdout);
#endif


for( j = 1 ; j <= atptr + addEquivs ; j++ ) {
    targetList[++targetPtr] = atlist[j] ;
}

if( attachPt > 0 ) {
    for( j = 1 ; j <= atptr ; j++ ) {
	node1 = atlist[j] ;
	if( j == atptr ) {
	    node2 = attachPt ;
	} else {
	    node2 = atlist[ j + 1 ] ;
	}
	gptr1 = gnodeArray[ node1 ] ;
	gptr2 = gnodeArray[ node2 ] ;
	gptr = gptr1 ;
	while( gptr != (GNODEPTR) NULL ) {
	    if( gptr->node == node2 ) {
		gptr->cost = VLARGE ;
		break ;
	    }
	    gptr = gptr->next ;
	}
	gptr = gptr2 ;
	while( gptr != (GNODEPTR) NULL ) {
	    if( gptr->node == node1 ) {
		gptr->cost = VLARGE ;
		break ;
	    }
	    gptr = gptr->next ;
	}
    }
}
/* ************************************************************** */

numExtra = 0 ;
numberPaths = 0 ;
for( extraS = 1 ; extraS <= EXTRASOURCES ; extraS++ ) {
    if( extraS > 1 && bareMinimum ) {
	break ;
    }
    min = VLARGE ;
    found = 0 ;
    for( i = 1 ; i <= sourcePtr ; i++ ) {
	for( s = 1 ; s <= numExtra ; s++ ) {
	    if( extraSlist[s] == sourceList[i] ) {
		break ;
	    }
	}
	if( s <= numExtra ) {
	    continue ;
	}
	found = 1 ;
	node = sourceList[i] - numnodes ;
	nptr = pnodeArray[node].nodeList ;
	for( j = 1 ; j <= targetPtr ; j++ ) {
	    targ = targetList[j] ;
	    if( nptr[ targ ].distance < min ) {

		from = nptr[targ].from ;
		pathExists = 0 ;
		g1ptr = gnodeArray[ targ ] ;
		for( ; pathExists == 0 && g1ptr != (GNODEPTR) NULL ; 
						g1ptr = g1ptr->next ){
		    node = g1ptr->node ;
		    if( node != from ) {
			continue ;
		    }
		    gptr = gnodeArray[node] ;
		    for( ; ; ) {
			if( gptr->node == targ ) {
			    if( gptr->cost < VLARGE ) {
				pathExists = 1 ;
			    }
			    break ;
			}
			gptr = gptr->next ;
		    }
		}

		if( pathExists ) {
		    min = nptr[ targ ].distance ;
		    savei = sourceList[i] ;
		    savej = j ;
		}
	    }
	}
    }
    if( found == 1 ) {
	extraSlist[ ++numExtra ] = savei ;

#ifdef DEBUG2
	printf("\nselected source node: %d\n", savei ) ;
	fflush(stdout);
#endif

	i = numberPaths ;
	numberPaths += mshortest( savei , savej ) ;
	for( i++, l = 1 ; i <= numberPaths ; i++, l++ ) {
	    if( pathList[l][0] + 1 > 20 ) {
		free( (int *) tempPL[i] ) ;
		tempPL[i] = (int *) malloc( (pathList[l][0] + 2) *
						sizeof(int));
	    }
	    for( j = 0 ; j <= pathList[l][0] + 1 ; j++ ) {
		tempPL[i][j] = pathList[l][j] ;
	    }
	    tinsert( &extraRoot , tempPL[i][j - 1] , i ) ;
	}
    }
}
/* ************************************************************** */
s = ( numberPaths >= Mpaths ) ? Mpaths : numberPaths ;
for( l = 1 ; l <= s ; l++ ) {
    tpop( &extraRoot , &junkptr , &junk , &j ) ;
    k = tempPL[j][0] ;

#ifdef DEBUG2
    printf("Spill it: ");
    for( i = 1 ; i < k ; i++ ) {
	addTargetList[i] = tempPL[j][i] ;
	printf(" %d ", tempPL[j][i] ) ;
    }
    printf("\n");
fflush(stdout);
#else
    for( i = 1 ; i < k ; i++ ) {
	addTargetList[i] = tempPL[j][i] ;
    }
#endif

    numdelete = 0 ;
    numadd = k - 1 ;
    for( i = 1 ; i < k ; i++ ) {
	if( addTargetList[i] > numnodes ) {
	    delSourceList[ ++numdelete ] = addTargetList[i] ;
	}
    }
    count = 0 ;
    for( i = 1 ; i <= numdelete ; i++ ) {
	savei = delSourceList[i] ;
	if( pnodeArray[ savei - numnodes ].eptr != 0 ) {
	    node = pnodeArray[ savei - numnodes ].eptr + numnodes ;
	    for( t = 1 ; t <= numdelete + count ; t++ ) {
		if( node == delSourceList[t] ) {
		    break ;
		}
	    }
	    if( t > numdelete + count ) {
		delSourceList[ numdelete + ++count ] = node ;
		addTargetList[ numadd + count ] = node ;
	    }
	    lptr = pnodeArray[ pnodeArray[savei - numnodes].eptr ].equiv;
	} else {
	    lptr = pnodeArray[savei - numnodes].equiv;
	}
	while( lptr != (LIST2PTR) NULL ) {
	    node = lptr->node + numnodes ;
	    for( t = 1 ; t <= numdelete + count ; t++ ) {
		if( node == delSourceList[t] ) {
		    break ;
		}
	    }
	    if( t > numdelete + count ) {
		delSourceList[ numdelete + ++count ] = node ;
		addTargetList[ numadd + count ] = node ;
	    }
	    lptr = lptr->next ;
	}
    }
    pathLength += tempPL[j][k + 1] ;
    numdelete += count ;
    numadd += count ;
    for( i = 1 ; i <= k ; i++ ) {
	segList[ i + segPtr ] = tempPL[j][i] ;
    }
    segList[ i + segPtr ] = 0 ;
    segPtr += k + 1 ;

    for( x = numadd - count + 1 ; x <= numadd ; x++ ) {
	node = addTargetList[x] ;
	gptr = gnodeArray[node] ;
	for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	    node1 = gptr->node ;
	    gptr1 = gnodeArray[node1] ;
	    for( ; ; ) {
		if( gptr1->node == node ) {
		    gptr1->einactive = 1 ;
		    break ;
		}
		gptr1 = gptr1->next ;
	    }
	}
    }

    recursePath( numdelete , numadd - count , tempPL[j][k] , count ) ;

    segPtr -= k + 1 ;
    pathLength -= tempPL[j][k + 1] ;

    for( x = numadd - count + 1 ; x <= numadd ; x++ ) {
	node = addTargetList[x] ;
	gptr = gnodeArray[node] ;
	for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	    node1 = gptr->node ;
	    gptr1 = gnodeArray[node1] ;
	    for( ; ; ) {
		if( gptr1->node == node ) {
		    gptr1->einactive = 0 ;
		    break ;
		}
		gptr1 = gptr1->next ;
	    }
	}
    }
}
count = treeSize - MAXPATHS ;
if( count > 0 ) {
    for( c = 1 ; c <= count ; c++ ) {
	tmax( &netRoot , &junkptr , &value , &index ) ;
	tdelete( &netRoot , value , index ) ;
	tinsert( &indexRoot , index , 0 ) ;
    }
    treeSize -= count ;
}
if( extraRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &extraRoot , &junkptr , &junk, &j ) ;
	if( junkptr == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}


for( j = 1 ; j <= dsptr ; j++ ) {
    sourceList[++sourcePtr] = dslist[j] ;
}

#ifdef DEBUG2
printf("\nSource Node List (AFTER End of Recurse):\n");
for( i = 1 ; i <= sourcePtr ; i++ ) {
    printf(" %d ", sourceList[i] ) ;
}
printf("\n");
fflush(stdout);
#endif


targetPtr -= atptr + addEquivs ;


if( attachPt > 0 ) {
    for( j = 1 ; j <= atptr ; j++ ) {
	node1 = atlist[j] ;
	if( j == atptr ) {
	    node2 = attachPt ;
	} else {
	    node2 = atlist[ j + 1 ] ;
	}
	gptr1 = gnodeArray[ node1 ] ;
	gptr2 = gnodeArray[ node2 ] ;
	gptr = gptr1 ;
	for( ; ; ) {
	    if( gptr->node == node2 ) {
		gptr->cost = gptr->length ;
		break ;
	    }
	    gptr = gptr->next ;
	}
	gptr = gptr2 ;
	for( ; ; ) {
	    if( gptr->node == node1 ) {
		gptr->cost = gptr->length ;
		break ;
	    }
	    gptr = gptr->next ;
	}
    }
}
free( (short int *) atlist ) ;
free( (short int *) dslist ) ;
free( extraSlist ) ;
for( i = 1 ; i <= EXTRASOURCES * MAXPATHS ; i++ ) {
    free( (int *) tempPL[i] ) ;
}
free( (int **) tempPL ) ;

return ;
}
