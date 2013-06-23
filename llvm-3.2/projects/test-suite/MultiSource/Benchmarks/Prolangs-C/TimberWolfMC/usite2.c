#include "custom.h"

extern void ufixpin( TERMBOXPTR termsptr , int flag , int targetx ,
		    int targety );
extern int ufixnet( TERMBOXPTR termsptr );
extern void usoftpin( CELLBOXPTR cellptr , int flag , int targetx ,
                      int target , int orient );
extern int usoftnet( CELLBOXPTR cellptr );

int usite2( int a , int b , int ax , int ay , int bx , int by )
{

CELLBOXPTR acellptr , bcellptr ;
TERMBOXPTR atermptr , btermptr ;

int cost , newpenalty , i ;
int axcenter , aycenter , bxcenter , bycenter ;
int aorient , borient ;


acellptr = cellarray[ a ]    ;
axcenter = acellptr->xcenter ;
aycenter = acellptr->ycenter ;
aorient  = acellptr->orient  ;
atermptr = acellptr->config[ aorient ]->termptr ;

bcellptr = cellarray[ b ]    ;
bxcenter = bcellptr->xcenter ;
bycenter = bcellptr->ycenter ;
borient  = bcellptr->orient  ;
btermptr = bcellptr->config[borient]->termptr ;


newpenalty = penalty ;

newpenalty -= (*overlap)( a, axcenter, aycenter, aorient, 0, 1 , 0 ) ;
occa1ptr = blockarray[binX][binY] ;
newpenalty -= (*overlap)( b, bxcenter, bycenter, borient, a, 1 , 0 ) ;
occb1ptr = blockarray[binX][binY] ;

newpenalty += (*overlap)( a, ax, ay, aorient, b, 0 , borient );
occa2ptr = blockarray[binX][binY] ;
newpenalty += (*overlap)( b, bx, by, borient, a, 1, 0 ) ;
occb2ptr = blockarray[binX][binY] ;


ufixpin( atermptr , 0 , ax , ay ) ;
usoftpin( acellptr , 0 , ax , ay , aorient ) ;

ufixpin( btermptr , 0 , bx , by ) ;
usoftpin( bcellptr , 0 , bx , by , borient ) ;

cost = funccost ;

cost += ufixnet( atermptr ) ;
cost += usoftnet( acellptr ) ;
cost += ufixnet( btermptr ) ;
cost += usoftnet( bcellptr ) ;


if( ( cost + newpenalty <= funccost + penalty ) || 
	(exp( (double)( funccost + penalty - cost - newpenalty ) /
	T) > ( (double) RAND / (double)0x7fffffff ) ) ){

    ufixpin( atermptr , 1 , 0 , 0 ) ;
    usoftpin( acellptr , 1 , 0 , 0 , 0 ) ;

    ufixpin( btermptr , 1 , 0 , 0 ) ;
    usoftpin( bcellptr , 1 , 0 , 0 , 0 ) ;

    if( occa1ptr != occa2ptr ) {
	for( i = 1 ; occa1ptr[ i ] != a ; i++ ) ;
	occa1ptr[ i ] = occa1ptr[  occa1ptr[0]--  ] ;
	occa2ptr[ ++occa2ptr[0]  ] = a ;
    }
    if( occb1ptr != occb2ptr ) {
	for( i = 1 ; occb1ptr[ i ] != b ; i++ ) ;
	occb1ptr[ i ] = occb1ptr[  occb1ptr[0]--  ] ;
	occb2ptr[ ++occb2ptr[0]  ] = b ;
    }

    acellptr->xcenter = ax ;
    acellptr->ycenter = ay ;
    bcellptr->xcenter = bx ;
    bcellptr->ycenter = by ;

    funccost = cost ;
    penalty = newpenalty ;

    return (1) ;
} else {
    return (0) ;
}
}
