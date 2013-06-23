#include "geo.h"

extern void tinsert( TNODEPTR *root , int value , int property );

void readgeo( FILE *fp )
{

char input[32] ;
int vertices , v , vx , vy , ux , uy , ux0 , uy0 ;
int rippleCount , cell , i ;
CLBOXPTR clptr ;

bbr = 0 ;
bbt = 0 ;
bbl = 10000000 ;
bbb = 10000000 ;
edgeCount = 0 ;
rippleCount = 0 ;
cell = 0 ;
hFixedEdgeRoot = (TNODEPTR) NULL ;
vFixedEdgeRoot = (TNODEPTR) NULL ;
hEdgeRoot = (TNODEPTR) NULL ;
vEdgeRoot = (TNODEPTR) NULL ;
edgeList = (EDGEBOXPTR) malloc( 401 * sizeof( EDGEBOX ) ) ;
cellList = (CLBOXPTR *) malloc( 50 * sizeof(CLBOXPTR) ) ;
for( i = 1 ; i <= 49 ; i++ ) {
    cellList[i] = (CLBOXPTR) NULL ;
}

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "cell") == 0 ) {
	clptr = (CLBOXPTR) NULL ;
	if( ++cell % 50 == 0 ) {
	    cellList = (CLBOXPTR *) realloc( cellList , 
				(cell + 50) * sizeof(CLBOXPTR) ) ;
	    for( i = cell ; i <= cell + 49 ; i++ ) {
		cellList[i] = (CLBOXPTR) NULL ;
	    }
	}
	fscanf( fp , " %s " , input ) ; /* cell name */
	fscanf( fp , " %d " , &vertices ) ;
	fscanf( fp , " %s " , input ) ; /* keyword: vertices */
	for( v = 1 ; v <= vertices ; v++ ) {
	    fscanf( fp , " %d %d " , &vx , &vy ) ;
	    if( vx > bbr ) {
		bbr = vx ;
	    }
	    if( vx < bbl ) {
		bbl = vx ;
	    }
	    if( vy > bbt ) {
		bbt = vy ;
	    }
	    if( vy < bbb ) {
		bbb = vy ;
	    }
	    if( v == 1 ) {
		ux0 = ux = vx ;
		uy0 = uy = vy ;
	    } else {
		edgeCount++ ;
		if( clptr == (CLBOXPTR) NULL ) {
		    cellList[cell] = (CLBOXPTR) malloc(sizeof(CLBOX));
		    clptr = cellList[cell] ;
		} else {
		    clptr->next = (CLBOXPTR) malloc(sizeof(CLBOX));
		    clptr = clptr->next ;
		}
		clptr->next = (CLBOXPTR) NULL ;
		clptr->edge = edgeCount ;
		clptr->extraSpace = 0 ;
		clptr->compact = 1000000 ;

		if( ++rippleCount > 400 ) {
		    edgeList = (EDGEBOXPTR) realloc( edgeList,
			    (edgeCount + 400) * sizeof(EDGEBOX));
		    rippleCount = 1 ;
		}
		if( v % 2 == 0 ) {
		    edgeList[edgeCount].start  = (uy <= vy) ? uy : vy ;
		    edgeList[edgeCount].end    = (uy <= vy) ? vy : uy ;
		    edgeList[edgeCount].loc    = ux ;
		    edgeList[edgeCount].length = (uy <= vy) ? 
						(vy - uy) : (uy - vy) ;
		    edgeList[edgeCount].UorR   = (uy <= vy) ? 1 : -1 ;
		    tinsert( &vFixedEdgeRoot, ux , edgeCount ) ;
		    tinsert( &vEdgeRoot, ux , edgeCount ) ;
		} else {
		    edgeList[edgeCount].start  = (ux <= vx) ? ux : vx ;
		    edgeList[edgeCount].end    = (ux <= vx) ? vx : ux ;
		    edgeList[edgeCount].loc    = uy ;
		    edgeList[edgeCount].length = (ux <= vx) ? 
						(vx - ux) : (ux - vx) ;
		    edgeList[edgeCount].UorR   = (ux <= vx) ? 1 : -1 ;
		    tinsert( &hFixedEdgeRoot, uy , edgeCount ) ;
		    tinsert( &hEdgeRoot, uy , edgeCount ) ;
		}
		edgeList[edgeCount].cell  = cell ;
		edgeList[edgeCount].fixed = 1 ;
		edgeList[edgeCount].nextEdge = edgeCount + 1 ;
		if( v == 2 ) {
		    edgeList[edgeCount].prevEdge = edgeCount + 
							vertices - 1 ;
		} else {
		    edgeList[edgeCount].prevEdge = edgeCount - 1 ;
		}
		ux = vx ;
		uy = vy ;
	    }
	}
	vx = ux0 ;
	vy = uy0 ;
	edgeCount++ ;

	clptr = clptr->next = (CLBOXPTR) malloc(sizeof(CLBOX));
	clptr->next = (CLBOXPTR) NULL ;
	clptr->edge = edgeCount ;
	clptr->extraSpace = 0 ;
	clptr->compact = 1000000 ;

	if( ++rippleCount > 400 ) {
	    edgeList = (EDGEBOXPTR) realloc( edgeList,
		    (edgeCount + 400) * sizeof(EDGEBOX));
	    rippleCount = 1 ;
	}
	edgeList[edgeCount].cell   = cell ;
	edgeList[edgeCount].start  = (ux <= vx) ? ux : vx ;
	edgeList[edgeCount].end    = (ux <= vx) ? vx : ux ;
	edgeList[edgeCount].loc    = uy ;
	edgeList[edgeCount].length = (ux <= vx) ? (vx - ux) : (ux - vx);
	edgeList[edgeCount].UorR   = (ux <= vx) ? 1 : -1 ;
	edgeList[edgeCount].prevEdge = edgeCount - 1 ;
	edgeList[edgeCount].nextEdge = edgeCount - (vertices - 1) ;
	edgeList[edgeCount].fixed    = 1 ;
	tinsert( &hFixedEdgeRoot, uy , edgeCount ) ;
	tinsert( &hEdgeRoot, uy , edgeCount ) ;
    } else {
	fprintf(fpdebug,"Found unparsable keyword: < %s > in ",
						    input );
	fprintf(fpdebug,"the .geo file\n");
	fprintf(fpdebug,"Current cell: %d\n", cell ) ;
	exit(0);
    }
}
numberCells = cell ;

bbb-- ;
bbt++ ;
bbl-- ;
bbr++ ;

edgeList = (EDGEBOXPTR) realloc( edgeList,
			(3 * edgeCount + 5) * sizeof(EDGEBOX));
edgeList[++edgeCount].start  = bbb ;
edgeList[edgeCount].end      = bbt ;
edgeList[edgeCount].loc      = bbl ;
edgeList[edgeCount].length   = bbt - bbb ;
edgeList[edgeCount].fixed    = 1 ;
edgeList[edgeCount].UorR     = -1 ;
edgeList[edgeCount].prevEdge = edgeCount + 1 ;
edgeList[edgeCount].nextEdge = edgeCount + 3 ;
edgeList[edgeCount].cell     = -1 ;

edgeList[++edgeCount].start  = bbl ;
edgeList[edgeCount].end      = bbr ;
edgeList[edgeCount].loc      = bbt ;
edgeList[edgeCount].length   = bbr - bbl ;
edgeList[edgeCount].fixed    = 1 ;
edgeList[edgeCount].UorR     = -1 ;
edgeList[edgeCount].prevEdge = edgeCount + 1 ;
edgeList[edgeCount].nextEdge = edgeCount - 1 ;
edgeList[edgeCount].cell     = -1 ;

edgeList[++edgeCount].start  = bbb ;
edgeList[edgeCount].end      = bbt ;
edgeList[edgeCount].loc      = bbr ;
edgeList[edgeCount].length   = bbt - bbb ;
edgeList[edgeCount].fixed    = 1 ;
edgeList[edgeCount].UorR     = 1 ;
edgeList[edgeCount].prevEdge = edgeCount + 1 ;
edgeList[edgeCount].nextEdge = edgeCount - 1 ;
edgeList[edgeCount].cell     = -1 ;

edgeList[++edgeCount].start  = bbl ;
edgeList[edgeCount].end      = bbr ;
edgeList[edgeCount].loc      = bbb ;
edgeList[edgeCount].length   = bbr - bbl ;
edgeList[edgeCount].fixed    = 1 ;
edgeList[edgeCount].UorR     = 1 ;
edgeList[edgeCount].prevEdge = edgeCount - 3 ;
edgeList[edgeCount].nextEdge = edgeCount - 1 ;
edgeList[edgeCount].cell     = -1 ;

numProbes = 0 ;

return ;
}
