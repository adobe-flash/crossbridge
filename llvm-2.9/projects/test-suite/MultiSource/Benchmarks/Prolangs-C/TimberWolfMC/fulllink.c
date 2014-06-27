#include "geo.h"

extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
                  int *property);
extern void tinsert( TNODEPTR *root , int value , int property );

void makeVertDownTree(void);
void makeHoriRiteTree(void);
void makeVertTree(void);
void makeHoriTree(void);

void fulllink(void)
{

TNODEPTR junkptr ;
DLINK1PTR ptr , pptr , nptr ;
int location , index ;

Hlist = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
Hlist->edge = edgeCount ;
Hlist->next = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
Hlist->prev = (DLINK1PTR) NULL ;
Hlist->next->next = (DLINK1PTR) NULL ;
Hlist->next->prev = Hlist ;
Hlist->next->edge = edgeCount - 2 ;
Hend = Hlist->next ;

Vlist = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
Vlist->edge = edgeCount - 3 ;
Vlist->next = (DLINK1PTR) malloc( sizeof( DLINK1 ) ) ;
Vlist->prev = (DLINK1PTR) NULL ;
Vlist->next->next = (DLINK1PTR) NULL ;
Vlist->next->prev = Vlist ;
Vlist->next->edge = edgeCount - 1 ;
Vend = Vlist->next ;

pptr = Hlist ;
nptr = Hlist->next ;
for( ; ; ) {
    tpop( &hEdgeRoot , &junkptr , &location , &index ) ;
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

pptr = Vlist ;
nptr = Vlist->next ;
for( ; ; ) {
    tpop( &vEdgeRoot , &junkptr , &location , &index ) ;
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

makeVertDownTree() ;
makeHoriRiteTree() ;
makeVertTree()     ;
makeHoriTree()     ;

return ;
}

void makeVertTree(void)
{

DLINK1PTR vptr ;
int last , edge , count ;

Vroot = (TNODEPTR) NULL ;
Vptrs = (DLINK1PTR *) malloc( 100 * sizeof(DLINK1PTR) ) ;
count = 0 ;

last = -1000000 ;
for( vptr = Vlist ; vptr != (DLINK1PTR) NULL ; vptr = vptr->next ) {
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




void makeHoriTree(void)
{

DLINK1PTR hptr ;
int last , edge , count ;

Hroot = (TNODEPTR) NULL ;
Hptrs = (DLINK1PTR *) malloc( 100 * sizeof(DLINK1PTR) ) ;
count = 0 ;

last = -1000000 ;
for( hptr = Hlist ; hptr != (DLINK1PTR) NULL ; hptr = hptr->next ) {
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

void makeVertDownTree(void)
{

DLINK1PTR vptr ;
int last , edge , count ;

VDroot = (TNODEPTR) NULL ;
VDptrs = (DLINK1PTR *) malloc( 100 * sizeof(DLINK1PTR) ) ;
count = 0 ;

last = -1000000 ;
for( vptr = Vlist ; vptr != (DLINK1PTR) NULL ; vptr = vptr->next ) {
    edge = vptr->edge ;
    if( edgeList[edge].UorR > 0 ) {
	continue ;
    }
    if( edgeList[edge].loc > last ) {
	last = edgeList[edge].loc ;
	if( ++count % 100 == 0 ) {
	    VDptrs = (DLINK1PTR *) realloc( VDptrs ,
				(count + 100) * sizeof(DLINK1PTR) ) ;
	}
	VDptrs[count] = vptr ;
	tinsert( &VDroot , last , count ) ;
    }
}

return ;
}



void makeHoriRiteTree(void)
{

DLINK1PTR hptr ;
int last , edge , count ;

HRroot = (TNODEPTR) NULL ;
HRptrs = (DLINK1PTR *) malloc( 100 * sizeof(DLINK1PTR) ) ;
count = 0 ;

last = -1000000 ;
for( hptr = Hlist ; hptr != (DLINK1PTR) NULL ; hptr = hptr->next ) {
    edge = hptr->edge ;
    if( edgeList[edge].UorR < 0 ) {
	continue ;
    }
    if( edgeList[edge].loc > last ) {
	last = edgeList[edge].loc ;
	if( ++count % 100 == 0 ) {
	    HRptrs = (DLINK1PTR *) realloc( HRptrs ,
				(count + 100) * sizeof(DLINK1PTR) ) ;
	}
	HRptrs[count] = hptr ;
	tinsert( &HRroot , last , count ) ;
    }
}

return ;
}
