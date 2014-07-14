#include "geo.h"

void printgph(void)
{

char filename[1024] ;
FILE *fp , *fpr ;
int i , index1 , index2 , length , width ;


sprintf( filename, "%s.rte", cktName ) ;
fpr = fopen( filename , "w" ) ;
sprintf( filename, "%s.gph", cktName ) ;
fp = fopen( filename , "w" ) ;

for( i = 1 ; i <= eNum ; i++ ) {
    if( eArray[i].notActive == 0 ) {
	index1 = eArray[i].index1 ;
	index2 = eArray[i].index2 ;
	if( i <= edgeTransition ) {
	    length = rectArray[index2].yc - rectArray[index1].yc  ;
	} else {
	    length = rectArray[index2].xc - rectArray[index1].xc  ;
	}
	eArray[i].length = length ;
	width  = eArray[i].width  ;
	fprintf( fp, "edge %5d %5d   length %8d  capacity %8d\n",
			    index1 , index2 , length , width ) ;

	fprintf(fpr,"edge  node: %5d  xloc: %d  yloc: %d    node: %5d  xloc: %d  yloc: %d   length: %d   capacity: %d\n", index1, rectArray[index1].xc,
	rectArray[index1].yc, index2, rectArray[index2].xc,
	rectArray[index2].yc, length, width ) ;
    }
}
fclose(fp);
fclose(fpr);
return ;
}
