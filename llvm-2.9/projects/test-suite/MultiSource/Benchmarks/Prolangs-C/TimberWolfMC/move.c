#include "port.h"
#include "mt.h"

extern void MTPoint(MT *t,int *x,int *y);
extern void MTIdentity(MT *t);
extern void MTMY(MT *t);
extern void MTMX(MT *t);
extern void MTRotate(MT *t,int x,int y);

static MT *mt = (MT *) NULL ;



void point( int *x , int *y )
{
    int xx , yy ;

    if( mt == (MT *) NULL ) {

        return;
    }

    xx = *x ;
    yy = *y ;

    MTPoint( mt , &xx , &yy ) ;

    *x = xx ;
    *y = yy ;

    return ;
}



void rect( int *l , int *b , int *r , int *t )
{

    int temp ;

    point( l , b ) ;
    point( r , t ) ;

    if( *l > *r ) {

        temp = *l   ;
        *l   = *r   ;
        *r   = temp ; 
    }

    if( *b > *t ) {

        temp = *b   ;
        *b   = *t   ;
        *t   = temp ; 
    }

    return ;
}



void move( int moveType )
{

    if( mt == (MT *) NULL ) {

    	mt = MTBegin() ;
    }

    MTIdentity( mt ) ;

    switch( moveType ) {

        case 0 :

            return ;


        case 1 :

            MTMY( mt ) ; 
    	    return     ;


        case 2 :

            MTMX( mt ) ; 
	    return     ;


        case 3 :

            MTRotate( mt , -1 , 0 ) ; 
	    return ; 


        case 4 :

            MTMX( mt ) ; 
	    MTRotate( mt , 0 , 1 ) ; 
	    return ;


        case 5 :

            MTMX( mt ) ; 
	    MTRotate( mt , 0 , -1 ) ; 
	    return ;


        case 6 :

            MTRotate( mt , 0 , 1 ) ; 
	    return ;


        case 7 :

            MTRotate( mt , 0 , -1 ) ; 
	    return ;

    }
}
