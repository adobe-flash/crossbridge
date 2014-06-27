#include "custom.h"
extern int finalShot ;
extern int bbleft , bbright , bbbottom , bbtop ;
extern int bbbl , bbbr , bbbb , bbbt ;

extern void ufixpin( TERMBOXPTR termsptr , int flag , int targetx,
		    int targety );
extern int ufixnet( TERMBOXPTR termsptr );
extern void usoftpin( CELLBOXPTR cellptr , int flag , int targetx ,
                      int target , int orient );
extern int usoftnet( CELLBOXPTR cellptr );
extern int deltaBB( int cell , int xb , int yb );

int usite1( int a , int xb , int yb )
{

CELLBOXPTR acellptr ;
TILEBOXPTR atileptr ;
TERMBOXPTR atermptr ;
int cost , newpenalty , i ;
int axcenter , aycenter , delta ;

acellptr = cellarray[ a ]    ;
axcenter = acellptr->xcenter ;
aycenter = acellptr->ycenter ;
atileptr = acellptr->config[ acellptr->orient ] ;
atermptr = atileptr->termptr ;



newpenalty = penalty ;

newpenalty -= (*overlap)( a, axcenter, aycenter, acellptr->orient, 0, 1,0);
occa1ptr = blockarray[binX][binY] ;

newpenalty += (*overlap)( a , xb , yb , acellptr->orient , 0, 1, 0 ) ;
occa2ptr = blockarray[binX][binY] ;


ufixpin( atermptr , 0 , xb , yb ) ;
usoftpin( acellptr , 0 , xb , yb , acellptr->orient ) ;

cost = funccost ;

cost += ufixnet( atermptr ) ;
cost += usoftnet( acellptr ) ;

if( finalShot > 0 ) {
    delta = deltaBB( a , xb , yb ) ;
    newpenalty += delta ;
}

if( ( cost + newpenalty <= funccost + penalty ) || 
	(exp( (double)( funccost + penalty - cost - newpenalty) /
	T) > ( (double) RAND / (double)0x7fffffff ) ) ){


    ufixpin( atermptr , 1 , 0 , 0 ) ;
    usoftpin( acellptr , 1 , 0 , 0 , 0 ) ;

    if( occa1ptr != occa2ptr ) {
	for( i = 1 ; occa1ptr[ i ] != a ; i++ ) ;
	occa1ptr[ i ] = occa1ptr[  occa1ptr[0]--  ] ;
	occa2ptr[ ++occa2ptr[0]  ] = a ;
    }

    acellptr->xcenter = xb ;
    acellptr->ycenter = yb ;

    funccost = cost ;
    if( finalShot > 0 ) {
	newpenalty -= delta ;
	bbleft = bbbl ;
	bbright = bbbr ;
	bbbottom = bbbb ;
	bbtop = bbbt ;
    }
    penalty = newpenalty ;

    return (1) ;
} else {
    return (0) ;
}
}
