#include "geo.h"
#define DEBUG
extern FILE *fpo ;

void bellman(void)
{

int i , j , D , distance ;
WCPTR ptr ;

xBellArray = (BELLBOXPTR) malloc((1 + numXnodes) * sizeof(BELLBOX));
yBellArray = (BELLBOXPTR) malloc((1 + numYnodes) * sizeof(BELLBOX));

xBellArray[1].from     = 0 ;
xBellArray[1].distance = 0 ;
for( i = 2 ; i <= numXnodes ; i++ ) {
    xBellArray[i].distance = VBIG ;
}

for( i = 1 ; i <= numXnodes ; i++ ) {
    D = xBellArray[i].distance ;
    for( ptr = xNodeArray[i]; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	j = ptr->node ;
	distance = - (ptr->length) ;
	if( xBellArray[j].distance > D + distance ) {
	    xBellArray[j].distance = D + distance ;
	    xBellArray[j].from = i ;
	}
    }
}

yBellArray[1].from     = 0 ;
yBellArray[1].distance = 0 ;
for( i = 2 ; i <= numYnodes ; i++ ) {
    yBellArray[i].distance = VBIG ;
}

for( i = 1 ; i <= numYnodes ; i++ ) {
    D = yBellArray[i].distance ;
    for( ptr = yNodeArray[i]; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	j = ptr->node ;
	distance = - (ptr->length) ;
	if( yBellArray[j].distance > D + distance ) {
	    yBellArray[j].distance = D + distance ;
	    yBellArray[j].from = i ;
	}
    }
}
#ifdef DEBUG
fprintf(fpo,"Longest Hori. Path in Circuit Graph has span: <%d>\n",
			    - xBellArray[numXnodes].distance ) ;
fprintf(fpo,"Longest Vert. Path in Circuit Graph has span: <%d>\n",
			    - yBellArray[numYnodes].distance ) ;
#endif

return ;
}
