#include "geo.h"

extern void tdelete( TNODEPTR *root , int value , int property );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);

void ygraph(void)
{

int i , hedge , location , eindex , er , el , r , l , cell , count ;
int beginEdge , iedge , inode , oedge , onode , lastE ;
WCPTR saveptr ;
DLINK1PTR hptr ;
PLISTPTR plptr ;
FLAREPTR fptr , ifptr , ofptr ;
TNODEPTR tnode ;


yNodules = (NODPTR) malloc( (2 + numberCells) * sizeof(NODBOX));
for( i = 0 ; i <= numberCells + 1 ; i++ ) {
    yNodules[i].inList = (FLAREPTR) NULL ;
    yNodules[i].outList = (FLAREPTR) NULL ;
    yNodules[i].done = 0 ;
}
hptr = Hlist ;
hedge = hptr->edge ;
location = edgeList[hedge].loc ;
er = edgeList[hedge].end ;
el = edgeList[hedge].start ;
plptr = tplist( hChanBeginRoot , location ) ;
for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
    eindex = plptr->property ;
    r = eArray[eindex].hiend ;
    l = eArray[eindex].loend ;
    if( ! (l >= el && r <= er) ) {
	continue ;
    }
    fptr = yNodules[0].outList ;
    yNodules[0].outList = (FLAREPTR) malloc( sizeof(FLARE) ) ;
    yNodules[0].outList->next = fptr ;
    yNodules[0].outList->fixEdge = hedge ;
    yNodules[0].outList->eindex = (int *) malloc( 2 * sizeof(int) ) ; 
    yNodules[0].outList->eindex[0] = 1 ; 
    yNodules[0].outList->eindex[1] = eindex ; 
    yNodules[0].done = 1 ; 
}
hptr = Hend ;
hedge = hptr->edge ;
location = edgeList[hedge].loc ;
er = edgeList[hedge].end ;
el = edgeList[hedge].start ;
plptr = tplist( hChanEndRoot , location ) ;
for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
    eindex = plptr->property ;
    r = eArray[eindex].hiend ;
    l = eArray[eindex].loend ;
    if( ! (l >= el && r <= er) ) {
	continue ;
    }
    fptr = yNodules[numberCells + 1].inList ;
    yNodules[numberCells + 1].inList = (FLAREPTR) malloc(sizeof(FLARE));
    yNodules[numberCells + 1].inList->next = fptr ;
    yNodules[numberCells + 1].inList->fixEdge = hedge ;
    yNodules[numberCells + 1].inList->eindex = (int *) 
						malloc(2 * sizeof(int)); 
    yNodules[numberCells + 1].inList->eindex[0] = 1 ; 
    yNodules[numberCells + 1].inList->eindex[1] = eindex ; 
    yNodules[numberCells + 1].done = 1 ; 
}

hptr = hFixedList->next ;
for( ; hptr->next != (DLINK1PTR) NULL ; hptr = hptr->next ) {
    hedge = hptr->edge ;
    cell = edgeList[hedge].cell ;
    if( yNodules[cell].done == 1 ) {
	continue ;
    }
    yNodules[cell].done = 1 ;
    beginEdge = hedge ;
    do {
	if( edgeList[hedge].UorR < 0 ) {
	    location = edgeList[hedge].loc ;
	    plptr = tplist( hChanEndRoot , location ) ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
		eindex = plptr->property ;
		if( eArray[eindex].edge2 != hedge ) {
		    continue ;
		}
		fptr = yNodules[cell].inList ;
		yNodules[cell].inList = (FLAREPTR) malloc(sizeof(FLARE));
		yNodules[cell].inList->next = fptr ;
		yNodules[cell].inList->fixEdge = hedge ;
		yNodules[cell].inList->eindex = (int *) 
						malloc(2 * sizeof(int)); 
		yNodules[cell].inList->eindex[0] = 1 ; 
		yNodules[cell].inList->eindex[1] = eindex ; 
	    }
	} else {
	    location = edgeList[hedge].loc ;
	    plptr = tplist( hChanBeginRoot , location ) ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
		eindex = plptr->property ;
		if( eArray[eindex].edge1 != hedge ) {
		    continue ;
		}
		fptr = yNodules[cell].outList ;
		yNodules[cell].outList = (FLAREPTR) malloc(sizeof(FLARE));
		yNodules[cell].outList->next = fptr ;
		yNodules[cell].outList->fixEdge = hedge ;
		yNodules[cell].outList->eindex = (int *) 
						malloc(2 * sizeof(int)); 
		yNodules[cell].outList->eindex[0] = 1 ; 
		yNodules[cell].outList->eindex[1] = eindex ; 
	    }
	}
	hedge = edgeList[hedge].nextEdge ;
	hedge = edgeList[hedge].nextEdge ;
    } while( hedge != beginEdge ) ;
}

for( cell = 0 ; cell <= numberCells + 1 ; cell++ ) {
    fptr = yNodules[cell].inList ;
    for( ; fptr != (FLAREPTR) NULL ; fptr = fptr->next ) {
	for( count = fptr->eindex[0]; count <= fptr->eindex[0]; count++){
	    eindex = fptr->eindex[count] ;
	    location = eArray[eindex].lbside ;
	    er = eArray[eindex].hiend ;
	    el = eArray[eindex].loend ;
	    plptr = tplist( hChanEndRoot , location ) ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
		eindex = plptr->property ;
		r = eArray[eindex].hiend ;
		l = eArray[eindex].loend ;
		if( ! (l >= el && r <= er) ) {
		    continue ;
		}
		tdelete( &hChanEndRoot , location , eindex ) ;
		if( ++(fptr->eindex[0]) % 2 == 0 ) {
		    fptr->eindex = (int *) realloc( fptr->eindex ,
				(2 + fptr->eindex[0]) * sizeof(int) ) ;
		}
		fptr->eindex[ fptr->eindex[0] ] = eindex ;
		break ;
	    }
	}
    }
    fptr = yNodules[cell].outList ;
    for( ; fptr != (FLAREPTR) NULL ; fptr = fptr->next ) {
	for( count = fptr->eindex[0]; count <= fptr->eindex[0]; count++){
	    eindex = fptr->eindex[count] ;
	    location = eArray[eindex].rtside ;
	    er = eArray[eindex].hiend ;
	    el = eArray[eindex].loend ;
	    plptr = tplist( hChanBeginRoot , location ) ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ) {
		eindex = plptr->property ;
		r = eArray[eindex].hiend ;
		l = eArray[eindex].loend ;
		if( ! (l >= el && r <= er) ) {
		    continue ;
		}
		tdelete( &hChanBeginRoot , location , eindex ) ;
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

numYnodes = 0 ;
for( hptr = hFixedList; hptr != (DLINK1PTR) NULL; hptr = hptr->next ){
    edgeList[ hptr->edge ].node = ++numYnodes ;
}
yNodeArray = (WCPTR *) malloc( (1 + numYnodes) * sizeof(WCPTR) );
for( i = 1 ; i <= numYnodes ; i++ ) {
    yNodeArray[i] = (WCPTR) NULL ;
}
for( cell = 1 ; cell <= numberCells ; cell++ ) {
    ifptr = yNodules[cell].inList ;
    for( ; ifptr != (FLAREPTR) NULL ; ifptr = ifptr->next ) {
	iedge = ifptr->fixEdge ;
	inode = edgeList[iedge].node ;
	ofptr = yNodules[cell].outList ;
	for( ; ofptr != (FLAREPTR) NULL ; ofptr = ofptr->next ) {
	    oedge = ofptr->fixEdge ;
	    onode = edgeList[oedge].node ;
	    saveptr = yNodeArray[inode] ;
	    yNodeArray[inode] = (WCPTR) malloc( sizeof(WCBOX));
	    yNodeArray[inode]->next = saveptr ;
	    yNodeArray[inode]->fixedWidth = 1 ;
	    yNodeArray[inode]->node = onode ;
	    yNodeArray[inode]->channels = (int *) NULL ;
	    yNodeArray[inode]->length = edgeList[oedge].loc -
					edgeList[iedge].loc ;
	}
    }
}
for( cell = 0 ; cell <= numberCells + 1 ; cell++ ) {
    ofptr = yNodules[cell].outList ;
    for( ; ofptr != (FLAREPTR) NULL ; ofptr = ofptr->next ) {
	iedge = ofptr->fixEdge ;
	inode = edgeList[iedge].node ;

	lastE = ofptr->eindex[ ofptr->eindex[0] ] ;
	onode = edgeList[ eArray[lastE].edge2 ].node ;

	saveptr = yNodeArray[inode] ;
	yNodeArray[inode] = (WCPTR) malloc( sizeof(WCBOX));
	yNodeArray[inode]->next = saveptr ;
	yNodeArray[inode]->fixedWidth = 0 ;
	yNodeArray[inode]->length = 0 ;
	yNodeArray[inode]->node = onode ;
	yNodeArray[inode]->channels = (int *) malloc( (1 + 
				ofptr->eindex[0]) * sizeof(int) );
	for( count = 0 ; count <= ofptr->eindex[0] ; count++ ) {
	    yNodeArray[inode]->channels[count] = ofptr->eindex[count] ;
	}
    }
}

if( hChanBeginRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &hChanBeginRoot , &tnode , &location , &eindex ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
if( hChanEndRoot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &hChanEndRoot , &tnode , &location , &eindex ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}

return ;
}
