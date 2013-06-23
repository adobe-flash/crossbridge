#include "geo.h"

extern void tdelete( TNODEPTR *root , int value , int property );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);

void xgraph(void)
{

int i , vedge , location , eindex , et , eb , t , b , cell , count ;
int beginEdge , iedge , inode , oedge , onode , lastE ;
WCPTR saveptr ;
DLINK1PTR vptr ;
PLISTPTR plptr ;
FLAREPTR fptr , ifptr , ofptr ;
TNODEPTR tnode ;

xNodules = (NODPTR) malloc( (2 + numberCells) * sizeof(NODBOX));
for( i = 0 ; i <= numberCells + 1 ; i++ ) {
    xNodules[i].inList = (FLAREPTR) NULL ;
    xNodules[i].outList = (FLAREPTR) NULL ;
    xNodules[i].done = 0 ;
}
vptr = Vlist ;
vedge = vptr->edge ;
location = edgeList[vedge].loc ;
et = edgeList[vedge].end ;
eb = edgeList[vedge].start ;
plptr = tplist( vChanBeginRoot , location ) ;
for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
    eindex = plptr->property ;
    t = eArray[eindex].hiend ;
    b = eArray[eindex].loend ;
    if( ! (b >= eb && t <= et) ) {
	continue ;
    }
    fptr = xNodules[0].outList ;
    xNodules[0].outList = (FLAREPTR) malloc( sizeof(FLARE) ) ;
    xNodules[0].outList->next = fptr ;
    xNodules[0].outList->fixEdge = vedge ;
    xNodules[0].outList->eindex = (int *) malloc( 2 * sizeof(int) ) ; 
    xNodules[0].outList->eindex[0] = 1 ; 
    xNodules[0].outList->eindex[1] = eindex ; 
    xNodules[0].done = 1 ; 
}
vptr = Vend ;
vedge = vptr->edge ;
location = edgeList[vedge].loc ;
et = edgeList[vedge].end ;
eb = edgeList[vedge].start ;
plptr = tplist( vChanEndRoot , location ) ;
for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
    eindex = plptr->property ;
    t = eArray[eindex].hiend ;
    b = eArray[eindex].loend ;
    if( ! (b >= eb && t <= et) ) {
	continue ;
    }
    fptr = xNodules[numberCells + 1].inList ;
    xNodules[numberCells + 1].inList = (FLAREPTR) malloc(sizeof(FLARE));
    xNodules[numberCells + 1].inList->next = fptr ;
    xNodules[numberCells + 1].inList->fixEdge = vedge ;
    xNodules[numberCells + 1].inList->eindex = (int *) 
						malloc(2 * sizeof(int)); 
    xNodules[numberCells + 1].inList->eindex[0] = 1 ; 
    xNodules[numberCells + 1].inList->eindex[1] = eindex ; 
    xNodules[numberCells + 1].done = 1 ; 
}

vptr = vFixedList->next ;
for( ; vptr->next != (DLINK1PTR) NULL ; vptr = vptr->next ) {
    vedge = vptr->edge ;
    cell = edgeList[vedge].cell ;
    if( xNodules[cell].done == 1 ) {
	continue ;
    }
    xNodules[cell].done = 1 ;
    beginEdge = vedge ;
    do {
	if( edgeList[vedge].UorR > 0 ) {
	    location = edgeList[vedge].loc ;
	    plptr = tplist( vChanEndRoot , location ) ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
		eindex = plptr->property ;
		if( eArray[eindex].edge2 != vedge ) {
		    continue ;
		}
		fptr = xNodules[cell].inList ;
		xNodules[cell].inList = (FLAREPTR) malloc(sizeof(FLARE));
		xNodules[cell].inList->next = fptr ;
		xNodules[cell].inList->fixEdge = vedge ;
		xNodules[cell].inList->eindex = (int *) 
						malloc(2 * sizeof(int)); 
		xNodules[cell].inList->eindex[0] = 1 ; 
		xNodules[cell].inList->eindex[1] = eindex ; 
	    }
	} else {
	    location = edgeList[vedge].loc ;
	    plptr = tplist( vChanBeginRoot , location ) ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
		eindex = plptr->property ;
		if( eArray[eindex].edge1 != vedge ) {
		    continue ;
		}
		fptr = xNodules[cell].outList ;
		xNodules[cell].outList = (FLAREPTR) malloc(sizeof(FLARE));
		xNodules[cell].outList->next = fptr ;
		xNodules[cell].outList->fixEdge = vedge ;
		xNodules[cell].outList->eindex = (int *) 
						malloc(2 * sizeof(int)); 
		xNodules[cell].outList->eindex[0] = 1 ; 
		xNodules[cell].outList->eindex[1] = eindex ; 
	    }
	}
	vedge = edgeList[vedge].nextEdge ;
	vedge = edgeList[vedge].nextEdge ;
    } while( vedge != beginEdge ) ;
}

for( cell = 0 ; cell <= numberCells + 1 ; cell++ ) {
    fptr = xNodules[cell].inList ;
    for( ; fptr != (FLAREPTR) NULL ; fptr = fptr->next ) {
	for( count = fptr->eindex[0]; count <= fptr->eindex[0]; count++){
	    eindex = fptr->eindex[count] ;
	    location = eArray[eindex].lbside ;
	    et = eArray[eindex].hiend ;
	    eb = eArray[eindex].loend ;
	    vedge = eArray[eindex].edge1 ;
	    plptr = tplist( vChanEndRoot , location ) ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
		eindex = plptr->property ;
		t = eArray[eindex].hiend ;
		b = eArray[eindex].loend ;
		if( ! (b >= eb && t <= et) ) {
		    continue ;
		}
		tdelete( &vChanEndRoot , location , eindex ) ;
		if( ++(fptr->eindex[0]) % 2 == 0 ) {
		    fptr->eindex = (int *) realloc( fptr->eindex ,
				(2 + fptr->eindex[0]) * sizeof(int) ) ;
		}
		fptr->eindex[ fptr->eindex[0] ] = eindex ;
		break ;
	    }
	}
    }
    fptr = xNodules[cell].outList ;
    for( ; fptr != (FLAREPTR) NULL ; fptr = fptr->next ) {
	for( count = fptr->eindex[0]; count <= fptr->eindex[0]; count++){
	    eindex = fptr->eindex[count] ;
	    location = eArray[eindex].rtside ;
	    et = eArray[eindex].hiend ;
	    eb = eArray[eindex].loend ;
	    plptr = tplist( vChanBeginRoot , location ) ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
		eindex = plptr->property ;
		t = eArray[eindex].hiend ;
		b = eArray[eindex].loend ;
		if( ! (b >= eb && t <= et) ) {
		    continue ;
		}
		tdelete( &vChanBeginRoot , location , eindex ) ;
		if( ++(fptr->eindex[0]) % 2 == 0 ) {
		    fptr->eindex = (int *) realloc( fptr->eindex ,
				(2 + fptr->eindex[0]) * sizeof(int) ) ;
		}
		fptr->eindex[ fptr->eindex[0] ] = eindex ;
		break ;
	    }
	}
    }
}

numXnodes = 0 ;
for( vptr = vFixedList; vptr != (DLINK1PTR) NULL; vptr = vptr->next ){
    edgeList[ vptr->edge ].node = ++numXnodes ;
}
xNodeArray = (WCPTR *) malloc( (1 + numXnodes) * sizeof(WCPTR) );
for( i = 1 ; i <= numXnodes ; i++ ) {
    xNodeArray[i] = (WCPTR) NULL ;
}
for( cell = 1 ; cell <= numberCells ; cell++ ) {
    ifptr = xNodules[cell].inList ;
    for( ; ifptr != (FLAREPTR) NULL ; ifptr = ifptr->next ) {
	iedge = ifptr->fixEdge ;
	inode = edgeList[iedge].node ;
	ofptr = xNodules[cell].outList ;
	for( ; ofptr != (FLAREPTR) NULL ; ofptr = ofptr->next ) {
	    oedge = ofptr->fixEdge ;
	    onode = edgeList[oedge].node ;
	    saveptr = xNodeArray[inode] ;
	    xNodeArray[inode] = (WCPTR) malloc( sizeof(WCBOX));
	    xNodeArray[inode]->next = saveptr ;
	    xNodeArray[inode]->fixedWidth = 1 ;
	    xNodeArray[inode]->node = onode ;
	    xNodeArray[inode]->channels = (int *) NULL ;
	    xNodeArray[inode]->length = edgeList[oedge].loc -
					edgeList[iedge].loc ;
	}
    }
}
for( cell = 0 ; cell <= numberCells + 1 ; cell++ ) {
    ofptr = xNodules[cell].outList ;
    for( ; ofptr != (FLAREPTR) NULL ; ofptr = ofptr->next ) {
	iedge = ofptr->fixEdge ;
	inode = edgeList[iedge].node ;

	lastE = ofptr->eindex[ ofptr->eindex[0] ] ;
	onode = edgeList[ eArray[lastE].edge2 ].node ;

	saveptr = xNodeArray[inode] ;
	xNodeArray[inode] = (WCPTR) malloc( sizeof(WCBOX));
	xNodeArray[inode]->next = saveptr ;
	xNodeArray[inode]->fixedWidth = 0 ;
	xNodeArray[inode]->length = 0 ;
	xNodeArray[inode]->node = onode ;
	xNodeArray[inode]->channels = (int *) malloc( (1 + 
				ofptr->eindex[0] ) * sizeof(int) );
	for( count = 0 ; count <= ofptr->eindex[0] ; count++ ) {
	    xNodeArray[inode]->channels[count] = ofptr->eindex[count] ;
	}
    }
}

if( vChanBeginRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &vChanBeginRoot , &tnode , &location , &eindex ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
if( vChanEndRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &vChanEndRoot , &tnode , &location , &eindex ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
return ;
}
