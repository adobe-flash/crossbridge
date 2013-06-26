#include "geo.h"
extern int TWsignal ;

void gentwf(void)
{

FILE *fp2 , *fp1  ;
char filename[1024] , c2[4] , c3[4] , c4[4] ,c5[12] , c6[4] ;
char pname[128] , nname[128] ;
int x , y , cell , PorE , offset , channel , hit ;
int node1 , node2 , min , edge , loc , ecount , max ;
FLAREPTR ifptr , ofptr , savefptr ;


sprintf( filename, "%s.pin", cktName ) ;
fp1 = fopen( filename , "r" ) ;
sprintf( filename, "%s.twf", cktName ) ;
fp2 = fopen( filename , "w" ) ;

while( fscanf(fp1," %s ", c6) == 1 ) {
    if( strcmp( c6 , "net" ) == 0 ) {
	fscanf(fp1, " %s ", nname ) ;
	fprintf(fp2,"\nnet %s\n", nname);
    } else if( strcmp( c6 , "pin" ) == 0 ) {
	fscanf(fp1,"%s %s %d %s %d %s %d %s %d", pname ,
		    c2 , &x , c3 , &y , c4 , &cell , c5 , &PorE) ;
	if( PorE == 1 ) {
	    fprintf(fp2,"    pin   %s  nodes   ", pname);
	} else {
	    fprintf(fp2,"    equiv %s  nodes   ", pname);
	}
	min = 1000000 ;
	ifptr = xNodules[cell].inList ;
	for( ; ifptr != (FLAREPTR) NULL ; ifptr = ifptr->next ) {
	    edge = ifptr->fixEdge ;
	    loc = edgeList[edge].loc ;
	    if( ABS(loc - x) < min ) {
		if( y <= rectArray[ 
			    eArray[ ifptr->eindex[1] ].index2 ].yc &&
			    y >= rectArray[ 
			    eArray[ ifptr->eindex[1] ].index1 ].yc ) {
		    min = ABS(loc - x) ;
		    savefptr = ifptr ;
		    hit = 1 ;
		}
	    }
	}
	ofptr = xNodules[cell].outList ;
	for( ; ofptr != (FLAREPTR) NULL ; ofptr = ofptr->next ) {
	    edge = ofptr->fixEdge ;
	    loc = edgeList[edge].loc ;
	    if( ABS(loc - x) < min ) {
		if( y <= rectArray[ 
			    eArray[ ofptr->eindex[1] ].index2 ].yc &&
			    y >= rectArray[ 
			    eArray[ ofptr->eindex[1] ].index1 ].yc ) {
		    min = ABS(loc - x) ;
		    savefptr = ofptr ;
		    hit = 1 ;
		}
	    }
	}
	ifptr = yNodules[cell].inList ;
	for( ; ifptr != (FLAREPTR) NULL ; ifptr = ifptr->next ) {
	    edge = ifptr->fixEdge ;
	    loc = edgeList[edge].loc ;
	    if( ABS(loc - y) < min ) {
		if( x <= rectArray[ 
			    eArray[ ifptr->eindex[1] ].index2 ].xc &&
			    x >= rectArray[ 
			    eArray[ ifptr->eindex[1] ].index1 ].xc ) {
		    min = ABS(loc - y) ;
		    savefptr = ifptr ;
		    hit = 2 ;
		}
	    }
	}
	ofptr = yNodules[cell].outList ;
	for( ; ofptr != (FLAREPTR) NULL ; ofptr = ofptr->next ) {
	    edge = ofptr->fixEdge ;
	    loc = edgeList[edge].loc ;
	    if( ABS(loc - y) < min ) {
		if( x <= rectArray[ 
			    eArray[ ofptr->eindex[1] ].index2 ].xc &&
			    x >= rectArray[ 
			    eArray[ ofptr->eindex[1] ].index1 ].xc ) {
		    min = ABS(loc - y) ;
		    savefptr = ofptr ;
		    hit = 2 ;
		}
	    }
	}
	ecount = 0 ;
	do {
	    channel = savefptr->eindex[++ecount] ;
	} while( eArray[channel].notActive == 1 ) ;

	node1 = eArray[channel].index1 ;
	node2 = eArray[channel].index2 ;
	if( hit == 1 ) {
	    offset = y - rectArray[node1].yc ;
	    max = rectArray[node2].yc - rectArray[node1].yc ;
	} else {
	    offset = x - rectArray[node1].xc ;
	    max = rectArray[node2].xc - rectArray[node1].xc ;
	}
	if( offset < 0 ) {
	    printf("\n\n");
	    printf("Pin: %s has location outside the range\n",
							    pname );
	    printf("of the edge: %d %d to which it was assigned\n",
				node1, node2 ) ;
	    printf("by an amount: %d\n", offset ) ;
	    printf("\n\n");
	    fflush(stdout);
	    offset = 0 ;
	    TWsignal = 1 ;
	} else if( offset > max ) {
	    printf("\n\n\n\n\n\n");
	    printf("Pin: %s has location outside the range\n",
							    pname );
	    printf("of the edge: %d %d to which it was assigned\n",
				node1, node2 ) ;
	    printf("by an amount: %d\n", offset - max ) ;
	    printf("\n\n");
	    fflush(stdout);
	    offset = max ;
	    TWsignal = 1 ;
	}
	fprintf(fp2,"%d  %d  at  %d\n", node1 , node2 , offset ) ;
    }
}
fclose(fp1) ;
fclose(fp2) ;

return ;
}
