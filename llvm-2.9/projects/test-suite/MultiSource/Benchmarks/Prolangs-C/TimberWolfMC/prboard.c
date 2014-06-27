#include "custom.h"

void prboard(void)
{

int xcenter , ycenter ;
int j , cell , site , terminal , xpos , ypos ;
CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
TERMBOXPTR termptr ;
FILE *fp , *fopen() ;
char filename[1024] ;

sprintf( filename, "%s.brd" , cktName ) ;
if( (fp = fopen ( filename , "w")) == (FILE *) NULL ) {
    fprintf(fpo,"can't open %s\n", filename ) ;
    exit(0);
}

for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    cellptr = cellarray[ cell ] ;
    tileptr = cellptr->config[ cellptr->orient ] ;
    termptr = tileptr->termptr ;

    xcenter = cellptr->xcenter ;
    ycenter = cellptr->ycenter ;
    
    fprintf(fp,"\n\n\n\nBOUNDING BOX OF CELL:%6d  is ", cell );
    fprintf(fp,"LEFT:%6d  BOTTOM:%6d  RIGHT:%6d  TOP:%6d\n",
	tileptr->left  + xcenter , tileptr->bottom + ycenter ,
	tileptr->right + xcenter , tileptr->top + ycenter ) ;

    fprintf(fp,"\nTHESE ARE THE TILES COMPRISING THIS CELL\n");

    for( tileptr = tileptr->nexttile ; tileptr != TILENULL ;
				tileptr = tileptr->nexttile ) {
	fprintf(fp,"LEFT:%6d  BOTTOM:%6d" , 
	   tileptr->left  + xcenter,tileptr->bottom + ycenter);
	fprintf(fp,"  RIGHT:%6d  TOP:%6d\n\n" , 
	   tileptr->right + xcenter, tileptr->top + ycenter );
    }

    fprintf(fp,"THESE ARE THE TERMINALS FOR THIS CELL\n");

    for( ; termptr != TERMNULL ; termptr = termptr->nextterm ){

	fprintf(fp,"TERMINAL:%s   XPOS:%6d   YPOS:%6d\n\n" ,
				pinnames[termptr->terminal] , 
				termptr->xpos + xcenter , 
				termptr->ypos + ycenter ) ;
    }
    if( cellptr->softflag == 1 ) {
	for( j = 1 ; j <= cellptr->numUnComTerms ; j++ ) {
	    terminal = cellptr->unComTerms[ j ].terminal ;
	    site = cellptr-> unComTerms[ j ].site ;
	    xpos = cellptr->config[ cellptr->orient ]->
				siteLocArray[ site ].xpos ;
	    ypos = cellptr->config[ cellptr->orient ]->
				siteLocArray[ site ].ypos ;
	    fprintf(fp,"TERMINAL:%s   XPOS:%6d   YPOS:%6d\n" ,
			pinnames[terminal] , xpos + xcenter , 
					   ypos + ycenter ) ;
	    fprintf(fp,"ACTUALS           XPOS:%6d   YPOS:%6d\n\n",
		 cellptr->unComTerms[ j ].finalx + xcenter , 
		 cellptr->unComTerms[ j ].finaly + ycenter ) ;
	    fprintf(fp,"   SITE:%6d\n\n", site ) ;
	}
    }
}
fclose( fp ) ;
return ;
}
