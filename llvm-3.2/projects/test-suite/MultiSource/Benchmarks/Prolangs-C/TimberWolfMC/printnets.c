#include "route.h"

void printnets(void)
{

int *iptr , m , net , i , distance , num ;
CHANBOXPTR cptr ;
char filename[1024] ;
#ifdef notdef
FILE *fp1;
#endif
FILE *fp2 ;

#ifdef notdef
sprintf( filename, "%s.prt" , cktName ) ;
if( (fp1 = fopen( filename , "w" )) == (FILE *) NULL ) {
    fprintf( fpo, "can't open %s\n", filename ) ;
    exit(0);
}
#endif

sprintf( filename, "%s.pyt" , cktName ) ;
if( (fp2 = fopen( filename , "w" )) == (FILE *) NULL ) {
    fprintf( fpo, "can't open %s\n", filename ) ;
    exit(0);
}

for( net = 1 ; net <= largestNet ; net++ ) {
    for( m = 1 ; m <= netRoutes[net].numRoutes ; m++ ) {
	iptr = netRoutes[net].alternate[m]->pinList ;
	cptr = netRoutes[net].alternate[m]->chanList ;
	distance = netRoutes[net].alternate[m]->distance ;
#ifdef notdef
	fprintf(fp1,"\n\n*** NET: %s ***   Alternative:%d",
				    nnameArray[net] , m ) ;
	fprintf(fp1,"    NET_LENGTH: %d\n", distance ) ;
	fprintf(fp1,"pin list\n");
#endif
	if( m == 1 ) {
	    fprintf(fp2,"\n\nnet %s ", nnameArray[net] ) ;
	    fprintf(fp2,"    length %d\n", distance ) ;
	    fprintf(fp2,"pins %d\n", iptr[0] );
	}
	for( i = 1 ; i <= iptr[0] ; i++ ) {
#ifdef notdef
	    fprintf(fp1,"        %s\n", 
		pnameArray[ pinOffset[net] + iptr[i] ] ) ;
#endif
	    if( m == 1 ) {
		fprintf(fp2,"        %s\n", 
		    pnameArray[ pinOffset[net] + iptr[i] ] ) ;
	    }
	}
#ifdef notdef
	fprintf(fp1,"channel list\n");
#endif
	num = 0 ;
	while( cptr != (CHANBOXPTR) NULL ) {
#ifdef notdef
	    fprintf(fp1,"        %d - %d      starting offset: %d   ",
			cptr->from, cptr->to, cptr->foffset ) ;
	    fprintf(fp1," ending offset: %d\n", cptr->toffset ) ;
#endif
	    num++ ;
	    cptr = cptr->next ;
	}
	if( m == 1 ) {
	    fprintf(fp2,"channels %d\n", num );
	    cptr = netRoutes[net].alternate[m]->chanList ;
	    while( cptr != (CHANBOXPTR) NULL ) {
		fprintf(fp2,"        %d %d %d %d\n", cptr->from, 
			    cptr->to, cptr->foffset, cptr->toffset );
	
		cptr = cptr->next ;
	    }
	}
    }
}

#ifdef notdef
fclose(fp1);
#endif

fclose(fp2);
return ;
}
