#include "custom.h"

extern void ufixpin( TERMBOXPTR termsptr , int flag , int targetx ,
		    int targety );
extern int ufixnet( TERMBOXPTR termsptr );
extern void usoftpin( CELLBOXPTR cellptr , int flag , int targetx ,
                      int target , int orient );
extern int usoftnet( CELLBOXPTR cellptr );


int usite0( int a , int newaor )
{

CELLBOXPTR acellptr ;
TERMBOXPTR anewtermptr ;

int cost , newpenalty , i ;
int axcenter , aycenter ;


acellptr = cellarray[a]    ;
axcenter = acellptr->xcenter ;
aycenter = acellptr->ycenter ;
anewtermptr = acellptr->config[newaor]->termptr ;


newpenalty = penalty ;

newpenalty -= (*overlap)( a, axcenter, aycenter, acellptr->orient,
							    0, 1, 0 ) ;
occa1ptr = blockarray[binX][binY] ;

newpenalty += (*overlap)( a, axcenter, aycenter, newaor, 0, 1, 0 ) ;
occa2ptr = blockarray[binX][binY] ;


ufixpin( anewtermptr, 0, axcenter, aycenter ) ;
usoftpin( acellptr, 0, axcenter, aycenter, newaor ) ;

cost = funccost ;

cost += ufixnet( anewtermptr ) ;
cost += usoftnet( acellptr ) ;


if( ( cost + newpenalty <= funccost + penalty ) || 
	(exp( (double)( funccost + penalty - cost - newpenalty ) /
	T) > ( (double) RAND / (double)0x7fffffff ) ) ){

    ufixpin( anewtermptr, 1, 0, 0 ) ;
    usoftpin( acellptr, 1, 0, 0, 0 ) ;

    if( occa1ptr != occa2ptr ) {
	for( i = 1 ; occa1ptr[ i ] != a ; i++ ) ;
	occa1ptr[ i ] = occa1ptr[  occa1ptr[0]--  ] ;
	occa2ptr[ ++occa2ptr[0]  ] = a ;
    }

    acellptr->orient = newaor ;

    funccost = cost ;
    penalty = newpenalty ;

    return (1) ;
} else {
    return (0) ;
}
}
