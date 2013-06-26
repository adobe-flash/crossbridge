#include "route.h"
#include "23tree.h"
TNODEPTR netRoot ;

extern void grepair(void);
extern void prepair( int numpnodes );
extern void loadpg( int net , int totalnodes );
extern void shortpath( int numpnodes );
extern void routenet( int segments , int numpnodes );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);

void mergeSeg(void);
void joinSeg( int a , int aend , int b , int bend );

void procesnet( int net , int segments , int addnodes )
{
int node1, distance, i, j, k, numberRoutes, pin1 , prev ;
int index, bound, *iptr, pincount, fnode, tnode, pin2 ;
int firstNode, lastNode, neighbor1, neighbor2, foffset, toffset ;
short int *isptr ;
CHANBOXPTR lastcptr ;
TNODEPTR dumnode ;

if( segments == 1 ) {
    return ;
}

grepair() ;
prepair( addnodes ) ;

loadpg( net , addnodes ) ;
shortpath( addnodes ) ;

routenet( segments , addnodes ) ;

numberRoutes = 0 ;

for( ; ; ) {
    tpop( &netRoot , &dumnode , &distance , &index ) ;
    if( dumnode == (TNODEPTR) NULL ) {
	break ;
    }
    numberRoutes++ ;
    isptr = savePaths[index] ;
    bound = isptr[0] ;

    i = 1 ;
    netSegArray[0][0] = 1 ;
    netSegArray[i][0] = 0 ;
    for( k = 1 ; k < bound ; k++ ) {
	if( isptr[k] == 0 ) {
	    i++ ;
	    netSegArray[0][0]++ ;
	    netSegArray[i][0] = 0 ;
	    continue ;
	}
	netSegArray[i][ ++netSegArray[i][0] ] = isptr[k] ;
    }
    mergeSeg() ;

    pincount = 0 ;
    for( i = 1 ; i <= netSegArray[0][0] ; i++ ) {
	for( j = 1 ; j <= netSegArray[i][0] ; j++ ) {
	    if( netSegArray[i][j] > numnodes ) {
		pincount++ ;
	    }
	}
    }
    netRoutes[net].alternate[numberRoutes]->pinList = (int *) malloc(
				    (1 + pincount) * sizeof( int ) ) ;
    iptr = netRoutes[net].alternate[numberRoutes]->pinList ;
    netRoutes[net].alternate[numberRoutes]->distance = distance ;
    iptr[0] = pincount ;
    pincount = 0 ;

    for( i = 1 ; i <= netSegArray[0][0] ; i++ ) {
	for( j = 1 ; j <= netSegArray[i][0] ; j++ ) {
	    if( netSegArray[i][j] > numnodes ) {
		iptr[ ++pincount ] = netSegArray[i][j] - numnodes ;
	    }
	}
    }
    lastcptr = (CHANBOXPTR) NULL ;

    for( i = 1 ; i <= netSegArray[0][0] ; i++ ) {
	firstNode = 0 ;
	for( j = 1 ; j <= netSegArray[i][0] ; j++ ) {
	    if( netSegArray[i][j] <= numnodes ) {
		if( firstNode == 0 ) {
		    firstNode = j ;
		}
		lastNode = j ;
	    }
	}
	if( firstNode == 0 ) {     /* all nodes in one channel */
	    pin1 = netSegArray[i][1] ;
	    pin2 = netSegArray[i][2] ;
	    neighbor1 = gnodeArray[pin1]->node ;
	    neighbor2 = gnodeArray[pin1]->next->node ;
	    if( neighbor2 == pin2 ) {
		foffset = gnodeArray[pin1]->length ;
		fnode = neighbor1 ;
	    } else {
		foffset = gnodeArray[pin1]->next->length ;
		fnode = neighbor2 ;
	    }
	    prev = pin1 ;
	    while( fnode > numnodes ) {
		neighbor1 = gnodeArray[fnode]->node ;
		neighbor2 = gnodeArray[fnode]->next->node ;
		if( neighbor2 == prev ) {
		    foffset += gnodeArray[fnode]->length ;
		    prev = fnode ;
		    fnode = neighbor1 ;
		} else {
		    foffset += gnodeArray[fnode]->next->length ;
		    prev = fnode ;
		    fnode = neighbor2 ;
		}
	    }
	    pin1 = netSegArray[i][ netSegArray[i][0] ] ;
	    pin2 = netSegArray[i][ netSegArray[i][0] - 1 ] ;
	    neighbor1 = gnodeArray[pin1]->node ;
	    neighbor2 = gnodeArray[pin1]->next->node ;
	    if( neighbor2 == pin2 ) {
		toffset = gnodeArray[pin1]->length ;
		tnode = neighbor1 ;
	    } else {
		toffset = gnodeArray[pin1]->next->length ;
		tnode = neighbor2 ;
	    }
	    prev = pin1 ;
	    while( tnode > numnodes ) {
		neighbor1 = gnodeArray[tnode]->node ;
		neighbor2 = gnodeArray[tnode]->next->node ;
		if( neighbor2 == prev ) {
		    toffset += gnodeArray[tnode]->length ;
		    prev = tnode ;
		    tnode = neighbor1 ;
		} else {
		    toffset += gnodeArray[tnode]->next->length ;
		    prev = tnode ;
		    tnode = neighbor2 ;
		}
	    }
	    if( lastcptr == (CHANBOXPTR) NULL ) {
		netRoutes[net].alternate[numberRoutes]->chanList = 
			(CHANBOXPTR) malloc( sizeof( CHANBOX ) ) ;
		lastcptr = netRoutes[net].alternate[numberRoutes]->
							chanList ;
	    } else {
		lastcptr = lastcptr->next =
			(CHANBOXPTR) malloc( sizeof( CHANBOX ) ) ;
	    }
	    lastcptr->from = fnode ;
	    lastcptr->to   = tnode ;
	    lastcptr->foffset = foffset ;
	    lastcptr->toffset = toffset  ;
	    lastcptr->next = (CHANBOXPTR) NULL ;
	} else {
	    if( firstNode > 1 ) {
		pin1 = netSegArray[i][1] ;
		pin2 = netSegArray[i][2] ;
		neighbor1 = gnodeArray[pin1]->node ;
		neighbor2 = gnodeArray[pin1]->next->node ;
		if( neighbor2 == pin2 ) {
		    foffset = gnodeArray[pin1]->length ;
		    fnode = neighbor1 ;
		} else {
		    foffset = gnodeArray[pin1]->next->length ;
		    fnode = neighbor2 ;
		}
		prev = pin1 ;
		while( fnode > numnodes ) {
		    neighbor1 = gnodeArray[fnode]->node ;
		    neighbor2 = gnodeArray[fnode]->next->node ;
		    if( neighbor2 == prev ) {
			foffset += gnodeArray[fnode]->length ;
			prev = fnode ;
			fnode = neighbor1 ;
		    } else {
			foffset += gnodeArray[fnode]->next->length ;
			prev = fnode ;
			fnode = neighbor2 ;
		    }
		}
		if( lastcptr == (CHANBOXPTR) NULL ) {
		    netRoutes[net].alternate[numberRoutes]->chanList = 
			    (CHANBOXPTR) malloc( sizeof( CHANBOX ) ) ;
		    lastcptr = netRoutes[net].alternate[numberRoutes]->
							    chanList ;
		} else {
		    lastcptr = lastcptr->next =
			    (CHANBOXPTR) malloc( sizeof( CHANBOX ) ) ;
		}
		lastcptr->from = fnode ;
		lastcptr->foffset = foffset ;
		lastcptr->to   = netSegArray[i][ firstNode ] ;
		lastcptr->toffset = 0 ;
		lastcptr->next = (CHANBOXPTR) NULL ;
	    }
	    node1 = firstNode ;
	    for( ; ; ) {
		for( j = node1 + 1 ; j <= lastNode ; j++ ) {
		    if( netSegArray[i][j] <= numnodes ) {
			break ;
		    }
		}
		if( j > lastNode ) {
		    break ;
		} else {
		    if( lastcptr == (CHANBOXPTR) NULL ) {
			netRoutes[net].alternate[numberRoutes]->
			chanList = (CHANBOXPTR) malloc(sizeof(CHANBOX));
			lastcptr = netRoutes[net].alternate[
					numberRoutes]-> chanList ;
		    } else {
			lastcptr = lastcptr->next =
			    (CHANBOXPTR) malloc( sizeof( CHANBOX ) ) ;
		    }
		    lastcptr->from = netSegArray[i][node1] ;
		    lastcptr->foffset = 0 ;
		    lastcptr->to   = netSegArray[i][j] ;
		    lastcptr->toffset = 0 ;
		    lastcptr->next = (CHANBOXPTR) NULL ;
		    node1 = j ; 
	   	}
	    }
	    if( lastNode < netSegArray[i][0] ) {
		pin1 = netSegArray[i][ netSegArray[i][0] ] ;
		pin2 = netSegArray[i][ netSegArray[i][0] - 1 ] ;
		neighbor1 = gnodeArray[pin1]->node ;
		neighbor2 = gnodeArray[pin1]->next->node ;
		if( neighbor2 == pin2 ) {
		    toffset = gnodeArray[pin1]->length ;
		    tnode = neighbor1 ;
		} else {
		    toffset = gnodeArray[pin1]->next->length ;
		    tnode = neighbor2 ;
		}
		prev = pin1 ;
		while( tnode > numnodes ) {
		    neighbor1 = gnodeArray[tnode]->node ;
		    neighbor2 = gnodeArray[tnode]->next->node ;
		    if( neighbor2 == prev ) {
			toffset += gnodeArray[tnode]->length ;
			prev = tnode ;
			tnode = neighbor1 ;
		    } else {
			toffset += gnodeArray[tnode]->next->length ;
			prev = tnode ;
			tnode = neighbor2 ;
		    }
		}
		lastcptr = lastcptr->next =
			    (CHANBOXPTR) malloc( sizeof( CHANBOX ) ) ;
		lastcptr->from = netSegArray[i][lastNode] ;
		lastcptr->foffset = 0 ;
		lastcptr->to   = tnode ;
		lastcptr->toffset = toffset ;
		lastcptr->next = (CHANBOXPTR) NULL ;
	    }
	}
    }

    if( numberRoutes >= MAXPATHS ) {
	break ;
    }
}
netRoutes[net].numRoutes = numberRoutes ;

return ;
}



void mergeSeg(void)
{

int i , j , flag ;

flag = 0 ;

for( i = 1 ; i < netSegArray[0][0] ; i++ ) {
    if( netSegArray[i][1] > numnodes ) {
	for( j = i + 1 ; j <= netSegArray[0][0] ; j++ ) {
	    if( netSegArray[i][1] == netSegArray[j][1] ) {
		joinSeg( i , -1 , j , -1 ) ;
		flag = 1 ;
		break ;
	    } else if( netSegArray[i][1] == 
			    netSegArray[j][ netSegArray[j][0] ] ) {
		joinSeg( i , -1 , j , 1 ) ;
		flag = 1 ;
		break ;
	    }
	}
	if( flag == 1 ) {
	    flag = 0 ;
	    i-- ;
	    continue ;
	}
    }
    if( netSegArray[i][ netSegArray[i][0] ] > numnodes ) {
	for( j = i + 1 ; j <= netSegArray[0][0] ; j++ ) {
	    if( netSegArray[i][ netSegArray[i][0] ] == 
				netSegArray[j][ netSegArray[j][0] ] ) {
		joinSeg( i , 1 , j , 1 ) ;
		i-- ;
		break ;
	    } else if( netSegArray[i][ netSegArray[i][0] ] == 
						netSegArray[j][1] ) {
		joinSeg( i , 1 , j , -1 ) ;
		i-- ;
		break ;
	    }
	}
    }
}
return ;
}




void joinSeg( int a , int aend , int b , int bend )
{

int i , j ;

if( aend < 0 ) {
    for( i = netSegArray[a][0] ; i >= 1 ; i-- ) {
	netSegArray[a][i + netSegArray[b][0] - 1] = netSegArray[a][i] ;
    }
    if( bend < 0 ) {
	for( i = 1 , j = netSegArray[b][0] ; j >= 2 ; j-- , i++ ) {
	    netSegArray[a][i] = netSegArray[b][j] ;
	}
    } else {
	for( i = 1 ; i < netSegArray[b][0] ; i++ ) {
	    netSegArray[a][i] = netSegArray[b][i] ;
	}
    }
} else {
    if( bend < 0 ) {
	for( i = 2 ; i <= netSegArray[b][0] ; i++ ) {
	    netSegArray[a][i + netSegArray[a][0] - 1] = netSegArray[b][i];
	}
    } else {
	for( i = netSegArray[a][0] + 1, j = netSegArray[b][0] - 1 ;
					    j >= 1 ; i++ , j-- ) {
	    netSegArray[a][i] = netSegArray[b][j] ;
	}
    }
}
netSegArray[a][0] += netSegArray[b][0] - 1 ;
for( i = b + 1 ; i <= netSegArray[0][0] ; i++ ) {
    for( j = 0 ; j <= netSegArray[i][0] ; j++ ) {
	netSegArray[i - 1][j] = netSegArray[i][j] ;
    }
}
netSegArray[0][0]-- ;
return ;
}
