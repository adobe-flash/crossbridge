#include "geo.h"

extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value , 
                  int *property);
extern void tinsert( TNODEPTR *root , int value , int property );

void makeVtree(void);
void makeHtree(void);

void makelink(void)
{

TNODEPTR junkptr ;
DLINK1PTR ptr , pptr , nptr ;
int location , index ;

hFixedList = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
hFixedList->edge = edgeCount ;
hFixedList->next = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
hFixedList->prev = (DLINK1PTR) NULL ;
hFixedList->next->next = (DLINK1PTR) NULL ;
hFixedList->next->prev = hFixedList ;
hFixedList->next->edge = edgeCount - 2 ;
hFixedEnd = hFixedList->next ;

vFixedList = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
vFixedList->edge = edgeCount - 3 ;
vFixedList->next = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
vFixedList->prev = (DLINK1PTR) NULL ;
vFixedList->next->next = (DLINK1PTR) NULL ;
vFixedList->next->prev = vFixedList ;
vFixedList->next->edge = edgeCount - 1 ;
vFixedEnd = vFixedList->next ;

pptr = hFixedList ;
nptr = hFixedList->next ;
for( ; ; ) {
    tpop( &hFixedEdgeRoot , &junkptr , &location , &index ) ;
    if( junkptr == (TNODEPTR) NULL ) {
	break ;
    }
    ptr = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
    ptr->edge = index ;
    ptr->prev = pptr ;
    ptr->next = nptr ;
    pptr->next = ptr ;
    nptr->prev = ptr ;
    pptr = ptr ;
}

pptr = vFixedList ;
nptr = vFixedList->next ;
for( ; ; ) {
    tpop( &vFixedEdgeRoot , &junkptr , &location , &index ) ;
    if( junkptr == (TNODEPTR) NULL ) {
	break ;
    }
    ptr = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
    ptr->edge = index ;
    ptr->prev = pptr ;
    ptr->next = nptr ;
    pptr->next = ptr ;
    nptr->prev = ptr ;
    pptr = ptr ;
}
makeVtree() ;
makeHtree() ;

return ;
}




void makeVtree(void)
{

DLINK1PTR vptr ;
int last , edge , count ;

Vroot = (TNODEPTR) NULL ;
Vptrs = (DLINK1PTR *) malloc( 100 * sizeof(DLINK1PTR) ) ;
count = 0 ;

last = -1000000 ;
for( vptr = vFixedList ; vptr != (DLINK1PTR) NULL ; vptr = vptr->next ) {
    edge = vptr->edge ;
    if( edgeList[edge].loc > last ) {
	last = edgeList[edge].loc ;
	if( ++count % 100 == 0 ) {
	    Vptrs = (DLINK1PTR *) realloc( Vptrs ,
				(count + 100) * sizeof(DLINK1PTR) ) ;
	}
	Vptrs[count] = vptr ;
	tinsert( &Vroot , last , count ) ;
    }
}

return ;
}




void makeHtree(void)
{

DLINK1PTR hptr ;
int last , edge , count ;

Hroot = (TNODEPTR) NULL ;
Hptrs = (DLINK1PTR *) malloc( 100 * sizeof(DLINK1PTR) ) ;
count = 0 ;

last = -1000000 ;
for( hptr = hFixedList ; hptr != (DLINK1PTR) NULL ; hptr = hptr->next ) {
    edge = hptr->edge ;
    if( edgeList[edge].loc > last ) {
	last = edgeList[edge].loc ;
	if( ++count % 100 == 0 ) {
	    Hptrs = (DLINK1PTR *) realloc( Hptrs ,
				(count + 100) * sizeof(DLINK1PTR) ) ;
	}
	Hptrs[count] = hptr ;
	tinsert( &Hroot , last , count ) ;
    }
}

return ;
}
