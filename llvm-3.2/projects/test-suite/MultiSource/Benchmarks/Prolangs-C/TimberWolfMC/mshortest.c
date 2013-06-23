/* #define DEBUG */
#include "route.h"
#include "23tree.h"
extern int bareMinimum ;
extern int doCompaction ;
extern int finalShot ;

extern void tinsert( TNODEPTR *root , int value , int property );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);
extern void tdelete( TNODEPTR *root , int value , int property );
extern int prestrict( short int *ptr, int p , int source , int target );
extern void tmax( TNODEPTR *root , TNODEPTR *node , int *value ,
                  int *property);

void greduce(short int *ptr,int p,int ex0,int ex1,int ex2,int ex3,int ex4 );
void gunreduce(short int *ptr,int p,int ex0,int ex1,int ex2,int ex3,int ex4 );

/*ARGSUSED*/
int mshortest( int source , int soleTarget )
{

int target , i , j , k , d , t , distance , from , index , pindex , c ;
int numberPaths , p , q , *eptr , exnum , tree2size ;
int ex[5] , number , junk , treeSize , value , count , targetLimit ;
int wasInactive , node1 , xindex , targetCount , foo ;
int initialCount , initialLimit ;
short int dummy[2] ;
short int *ptr ;
NNODEPTR nptr ;
TNODEPTR root1 , node , rsave , junkptr , indexRoot , targetRoot ;
TNODEPTR initialRoot ;
GNODEPTR gptr , gptr1 ;

rsave = (TNODEPTR) NULL ;
indexRoot = (TNODEPTR) NULL ;
targetRoot = (TNODEPTR) NULL ;
initialRoot = (TNODEPTR) NULL ;

for( i = 1 ; i <= 2 + 2 * Mpaths ; i++ ) {
    tinsert( &indexRoot , i , 0 ) ;
}
treeSize = 0 ;
tree2size = 0 ;
wasInactive = 0 ;
nptr = pnodeArray[ source - numnodes ].nodeList ;

for( t = 1 ; t <= targetPtr ; t++ ) {
    target = targetList[t] ;
    tinsert( &initialRoot , nptr[target].distance , t ) ;
}

initialCount = 0 ;
initialLimit = ((int)(0.1 * (double) targetPtr) + 1) + 2 * Mpaths ;

if( initialLimit > targetPtr ) {
    initialLimit = targetPtr ;
}
if( bareMinimum ) {
    if( finalShot == doCompaction ) {
	initialLimit = 2 ;
    } else {
	initialLimit = 1 ;
    }
}

while( initialCount < initialLimit ) {
    tpop( &initialRoot , &junkptr , &foo , &t ) ;
    if( junkptr == (TNODEPTR) NULL ) {
	break ;
    }
    target = targetList[t] ;
    gptr = gnodeArray[target] ;
    node1 = gptr->node ;
    gptr1 = gnodeArray[node1] ;
    for( ; ; ) {
	if( gptr1->node == target ) {
	    if( gptr1->einactive == 1 ) {
		gptr1->einactive = 0 ;
		wasInactive = 1 ;
		gptr = gptr->next ;
		for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
		    node1 = gptr->node ;
		    gptr1 = gnodeArray[node1] ;
		    for( ; ; ) {
			if( gptr1->node == target ) {
			    gptr1->einactive = 0 ;
			    break ;
			}
			gptr1 = gptr1->next ;
		    }
		}
	    }
	    break ;
	}
	gptr1 = gptr1->next ;
    }
    dummy[1] = source ;
    d = prestrict( dummy , 1 , source , target ) ;
    if( wasInactive == 1 ) {
	gptr = gnodeArray[target] ;
	for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	    node1 = gptr->node ;
	    gptr1 = gnodeArray[node1] ;
	    for( ; ; ) {
		if( gptr1->node == target ) {
		    gptr1->einactive = 1 ;
		    break ;
		}
		gptr1 = gptr1->next ;
	    }
	}
	wasInactive = 0 ;
    }
    if( d >= 0 ) { 
	tinsert( &targetRoot , nptr[target].temp , t ) ;
	initialCount++ ;
    }
}
if( initialRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &initialRoot , &junkptr , &foo , &t ) ;
	if( junkptr == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
targetLimit = Mpaths ;
targetCount = 0 ;

while( targetCount < targetLimit ) {

    tpop( &targetRoot , &junkptr , &foo , &t ) ;
    if( junkptr == (TNODEPTR) NULL ) {
	break ;
    }
    
    numberPaths = 0 ;
    root1 = (TNODEPTR) NULL ;


    target = targetList[t] ;
    gptr = gnodeArray[target] ;
    node1 = gptr->node ;
    gptr1 = gnodeArray[node1] ;
    for( ; ; ) {
	if( gptr1->node == target ) {
	    if( gptr1->einactive == 1 ) {
		gptr1->einactive = 0 ;
		wasInactive = 1 ;
		gptr = gptr->next ;
		for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
		    node1 = gptr->node ;
		    gptr1 = gnodeArray[node1] ;
		    for( ; ; ) {
			if( gptr1->node == target ) {
			    gptr1->einactive = 0 ;
			    break ;
			}
			gptr1 = gptr1->next ;
		    }
		}
	    }
	    break ;
	}
	gptr1 = gptr1->next ;
    }
    dummy[1] = source ;
    d = prestrict( dummy , 1 , source , target ) ;
    if( d < 0 ) {
	if( wasInactive == 1 ) {
	    gptr = gnodeArray[target] ;
	    for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
		node1 = gptr->node ;
		gptr1 = gnodeArray[node1] ;
		for( ; ; ) {
		    if( gptr1->node == target ) {
			gptr1->einactive = 1 ;
			break ;
		    }
		    gptr1 = gptr1->next ;
		}
	    }
	    wasInactive = 0 ;
	}
	continue ;
    }
    distance = nptr[target].temp ;

    targetCount++ ;
    from = nptr[target].from2 ;
    number = 1 ;
    tempArray[1] = target ;
    while( from != 0 ) {
	tempArray[++number] = from ;
	from = nptr[from].from2 ;
    }
    tpop( &indexRoot , &junkptr , &pindex , &junk ) ;

#ifdef DEBUG
    printf("pindex:%d\n",pindex);
    fflush(stdout);
#endif

    pathArray[pindex].p = 1 ;
    pathArray[pindex].q = number - 1 ;
    pathArray[pindex].excluded = 0 ;
    for( j = 1 ; j <= number ; j++ ) {
	pathArray[pindex].nodeset[ number - j + 1 ] = tempArray[j] ;
    }
    tinsert( &root1 , distance , pindex ) ;
    treeSize++ ;

    for( ; ; ) {
	tpop( &root1 , &node , &distance , &index ) ;
	treeSize-- ;
	if( node == (TNODEPTR) NULL ) {
	    break ;
	}
	tinsert( &rsave , distance , index ) ;
	tree2size++ ;
	if( ++numberPaths >= Mpaths ) {
	    break ;
	}
	ptr = pathArray[index].nodeset ;
	p = pathArray[index].p ;
	q = pathArray[index].q ;
	exnum = pathArray[index].excluded ;
	eptr = pathArray[index].exlist ;
	if( p == q ) {
	    ex[0] = exnum + 1 ;
	    for( i = 1 ; i <= exnum ; i++ ) {
		 ex[i] = eptr[i] ;
	    }
	    ex[ ex[0] ] = ptr[ p + 1 ] ;

	    greduce( ptr, p, ex[0], ex[1], ex[2], ex[3], ex[4] ) ;
	    d = prestrict( ptr, p , source , target ) ;
	    gunreduce( ptr, p, ex[0], ex[1], ex[2], ex[3], ex[4] ) ;
	    if( d < 0 ) {
		continue ;
	    }
	    distance = nptr[target].temp + d ;
	    from = nptr[target].from2 ;
	    number = 1 ;
	    tempArray[1] = target ;
	    while( from != 0 ) {
		tempArray[++number] = from ;
		from = nptr[from].from2 ;
	    }
	    for( i = p - 1 ; i >= 1 ; i-- ) {
		tempArray[++number] = ptr[i] ;
	    }
	    tpop( &indexRoot , &junkptr , &pindex , &junk ) ;

#ifdef DEBUG
	    printf("pindex:%d\n",pindex);
	    fflush(stdout);
#endif

	    pathArray[pindex].p = p ;
	    pathArray[pindex].q = number - 1 ;
	    pathArray[pindex].excluded = ex[0] ;
	    for( i = 1 ; i <= ex[0] ; i++ ) {
		pathArray[pindex].exlist[i] = ex[i] ;
	    }
	    for( j = 1 ; j <= number ; j++ ) {
		pathArray[pindex].nodeset[number - j + 1] = tempArray[j];
	    }
	    tinsert( &root1 , distance , pindex ) ;
	    treeSize++ ;

	    
	    count = treeSize - (Mpaths - numberPaths) ;
	    if( count > 0 ) {
		for( c = 1 ; c <= count ; c++ ) {
		    tmax( &root1, &node, &value, &xindex ) ;
		    tdelete( &root1, value, xindex ) ;
		    tinsert( &indexRoot, xindex, 0 ) ;
		}
		treeSize -= count ;
	    }
	   
	} else {
	    for( k = 1 ; k <= q - p + 1 ; k++ ) {
		if( k == 1 ) {
		    ex[0] = exnum + 1 ;
		    for( i = 1 ; i <= exnum ; i++ ) {
			 ex[i] = eptr[i] ;
		    }
		    ex[ ex[0] ] = ptr[ p + 1 ] ;
		} else {
		    ex[ ex[0] = 1 ] = ptr[ p + k ] ;
		}
		greduce( ptr, p + k - 1, 
				ex[0], ex[1], ex[2], ex[3], ex[4] ) ;
		d = prestrict( ptr, p + k - 1 , source , target ) ;
		gunreduce( ptr, p + k - 1, 
				ex[0], ex[1], ex[2], ex[3], ex[4] ) ;
		if( d < 0 ) {
		    continue ;
		}
		distance = nptr[target].temp + d ;
		from = nptr[target].from2 ;
		number = 1 ;
		tempArray[1] = target ;
		while( from != 0 ) {
		    tempArray[++number] = from ;
		    from = nptr[from].from2 ;
		}
		for( i = p + k - 2 ; i >= 1 ; i-- ) {
		    tempArray[++number] = ptr[i] ;
		}
		tpop( &indexRoot , &junkptr , &pindex , &junk ) ;
		treeSize++ ;

#ifdef DEBUG
		printf("pindex:%d\n",pindex);
		fflush(stdout);
#endif

		pathArray[pindex].p = p + k - 1 ;
		pathArray[pindex].q = number - 1 ;
		pathArray[pindex].excluded = ex[0] ;
		for( i = 1 ; i <= ex[0] ; i++ ) {
		    pathArray[pindex].exlist[i] = ex[i] ;
		}
		for( j = 1 ; j <= number ; j++ ) {
		    pathArray[pindex].nodeset[number - j + 1] = 
							tempArray[j];
		}
		tinsert( &root1 , distance , pindex ) ;

		
		count = treeSize - (Mpaths - numberPaths) ;
		if( count > 0 ) {
		    for( c = 1 ; c <= count ; c++ ) {
			tmax( &root1, &node, &value, &xindex ) ;
			tdelete( &root1, value, xindex ) ;
			tinsert( &indexRoot, xindex, 0 ) ;
		    }
		    treeSize -= count ;
		}
	
	    }
	}
    }
    count = tree2size - Mpaths ;
    if( count > 0 ) {
	for( c = 1 ; c <= count ; c++ ) {
	    tmax( &rsave, &node, &value, &xindex ) ;
	    tdelete( &rsave, value, xindex ) ;
	    tinsert( &indexRoot, xindex, 0 ) ;
	}
	tree2size -= count ;
    }
   
    if( root1 != (TNODEPTR) NULL ) {
	for( ; ; ) {
	    tpop( &root1 , &node , &distance , &index ) ;
	    if( node == (TNODEPTR) NULL ) {
		break ;
	    }
	}
    }
    treeSize = 0 ;
    if( wasInactive == 1 ) {
	gptr = gnodeArray[target] ;
	for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	    node1 = gptr->node ;
	    gptr1 = gnodeArray[node1] ;
	    for( ; ; ) {
		if( gptr1->node == target ) {
		    gptr1->einactive = 1 ;
		    break ;
		}
		gptr1 = gptr1->next ;
	    }
	}
	wasInactive = 0 ;
    }
}
numberPaths = 0 ;
for( ; ; ) {
    tpop( &rsave , &node , &distance , &index ) ;
    if( node == (TNODEPTR) NULL ) {
	break ;
    }
    ptr = pathArray[index].nodeset ;
    k = pathArray[index].q + 1 ;
    pathList[++numberPaths][0] = k ;
    pathList[numberPaths][k + 1] = distance ;
    for( i = 1 ; i <= k ; i++ ) {
	pathList[numberPaths][i] = ptr[i] ;
    }
    if( numberPaths >= Mpaths ) {
	break ;
    }
}
if( indexRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &indexRoot , &node , &distance , &index ) ;
	if( node == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
if( targetRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &targetRoot , &junkptr , &foo , &t ) ;
	if( junkptr == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}

return( numberPaths ) ;
}


    

void greduce(short int *ptr,int p,int ex0,int ex1,int ex2,int ex3,int ex4 )
{

int l , node , node1 , node2 , exl[5] ;
GNODEPTR gptr , gptr1 , gptr2 ;

exl[1] = ex1 ;
exl[2] = ex2 ;
exl[3] = ex3 ;
exl[4] = ex4 ;
node1 = ptr[p] ;
gptr1 = gnodeArray[ node1 ] ;
for( l = 1 ; l <= ex0 ; l++ ) {
    node2 = exl[l] ;
    gptr2 = gnodeArray[ node2 ] ;
    gptr = gptr1 ;
    for( ; ; ) {
	if( gptr->node == node2 ) {
	    gptr->cost = VLARGE ;
	    break ;
	}
	gptr = gptr->next ;
    }
    gptr = gptr2 ;
    for( ; ; ) {
	if( gptr->node == node1 ) {
	    gptr->cost = VLARGE ;
	    break ;
	}
	gptr = gptr->next ;
    }
}
for( l = 1 ; l < p ; l++ ) {
    node = ptr[l] ;
    gptr = gnodeArray[node] ;
    for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	node1 = gptr->node ;
	gptr1 = gnodeArray[node1] ;
	for( ; ; ) {
	    if( gptr1->node == node ) {
		gptr1->inactive = 1 ;
		break ;
	    }
	    gptr1 = gptr1->next ;
	}
    }
}
return ;
}





void gunreduce(short int *ptr,int p,int ex0,int ex1,int ex2,int ex3,int ex4 )
{

int l , node , node1 , node2 , exl[5] ;
GNODEPTR gptr , gptr1 , gptr2 ;

exl[1] = ex1 ;
exl[2] = ex2 ;
exl[3] = ex3 ;
exl[4] = ex4 ;
node1 = ptr[p] ;
gptr1 = gnodeArray[ node1 ] ;
for( l = 1 ; l <= ex0 ; l++ ) {
    node2 = exl[l] ;
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
for( l = 1 ; l < p ; l++ ) {
    node = ptr[l] ;
    gptr = gnodeArray[node] ;
    for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	node1 = gptr->node ;
	gptr1 = gnodeArray[node1] ;
	for( ; ; ) {
	    if( gptr1->node == node ) {
		gptr1->inactive = 0 ;
		break ;
	    }
	    gptr1 = gptr1->next ;
	}
    }
}
return ;
}
