#include "route.h"

void readgraph( FILE *fp )
{

char input[32] ;
int node1 , node2 , length , capacity , edge , i , test ;
GNODEPTR gptr ;

numnodes = 0 ;
edge = 0 ;

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "edge") == 0 ) {
	edge++ ;
	test = fscanf( fp , " %d %d " , &node1 , &node2 ) ;
	if( test != 2 ) {
	    fprintf( fpo, "Failed to input a pair of nodes ");
	    fprintf( fpo, "following keyword edge in the .gph file\n");
	    fprintf( fpo, "Current edge: %d\n", edge ) ;
	    exit(0);
	}
	if( node1 > numnodes ) {
	    numnodes = node1 ;
	}
	if( node2 > numnodes ) {
	    numnodes = node2 ;
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "length") == 0 ) {
	    test = fscanf( fp , " %d " , &length ) ;
	    if( test != 1 ) {
		fprintf( fpo, "Failed to input length ");
		fprintf( fpo, "in the .gph file\n");
		fprintf( fpo, "Current edge: %d\n", edge ) ;
		exit(0);
	    }
	} else {
	    fprintf( fpo, "Failed to find keyword: length ");
	    fprintf( fpo, "for an edge\n") ;
	    fprintf( fpo, "Current edge: %d\n", edge ) ;
	    exit(0) ;
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "capacity") == 0 ) {
	    test = fscanf( fp , " %d " , &capacity ) ;
	    if( test != 1 ) {
		fprintf( fpo, "Failed to input capacity ");
		fprintf( fpo, "in the .gph file\n");
		fprintf( fpo, "Current edge: %d\n", edge ) ;
		exit(0);
	    }
	} else {
	    fprintf( fpo, "Failed to find keyword: capacity ");
	    fprintf( fpo, "for an edge\n") ;
	    fprintf( fpo, "Current edge: %d\n", edge ) ;
	    exit(0) ;
	}
    } else {
	fprintf( fpo, "Found unknown string when attempting ");
	fprintf( fpo, "to find keyword: edge in the .gph file\n") ;
	fprintf( fpo, "instead found: %s\n", input ) ;
	fprintf( fpo, "Current edge: %d\n", edge ) ;
	exit(0) ;
    }
}

rewind( fp ) ;

gnodeArray = (GNODEPTR *) malloc( (numnodes + 1) * sizeof(GNODEPTR) ) ;
for( i = 1 ; i <= numnodes ; i++ ) {
    gnodeArray[i] = (GNODEPTR) NULL ;
}

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "edge") == 0 ) {
	fscanf( fp , " %d %d " , &node1 , &node2 ) ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &length ) ;
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &capacity ) ;
	gptr = gnodeArray[node1] ;
	gnodeArray[node1] =(GNODEPTR) malloc( sizeof(GNODE) ) ;
	gnodeArray[node1]->next = gptr ;
	gnodeArray[node1]->node = node2 ;
	gnodeArray[node1]->length = length ;
	gnodeArray[node1]->ilength = length ;
	gnodeArray[node1]->cost = length ;
	gnodeArray[node1]->capacity = capacity ;
	gnodeArray[node1]->inactive = 0 ;
	gnodeArray[node1]->einactive = 0 ;
	gptr = gnodeArray[node2] ;
	gnodeArray[node2] =(GNODEPTR) malloc( sizeof(GNODE) ) ;
	gnodeArray[node2]->next = gptr ;
	gnodeArray[node2]->node = node1 ;
	gnodeArray[node2]->length = length ;
	gnodeArray[node2]->ilength = length ;
	gnodeArray[node2]->cost = length ;
	gnodeArray[node2]->capacity = capacity ;
	gnodeArray[node2]->inactive = 0 ;
	gnodeArray[node2]->einactive = 0 ;
    }
}
return ;
}
