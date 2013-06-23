#include "custom.h"
extern int finalShot ;
extern int doCompaction ;
extern double Tsave ;
int count ;

extern void uloop(void);
extern void fuloop(void);

void utemp(void)
{

double Tfactor , Tscale ;

Tscale = Tsave / 100000.0 ;
count = 0 ;

for( ; ; ) {
    if( finalShot == 0 ) {
	if( T > 3000.0 * Tscale ) {
	    Tfactor = 0.88 ;
	} else if( T > 200.0 * Tscale ) {
	    Tfactor = 0.955 ;
	} else if( T > 20.0 * Tscale ) {
	    Tfactor = 0.85 ;
	} else {
	    Tfactor = 0.80 ;
	}
	uloop() ;
    } else {
	if( T < 10.0 * Tscale ) {
	    Tfactor = 0.70 ;
	} else {
	    Tfactor = 0.82 ;
	}
	if( finalShot >= 1 ) {
	    fuloop() ;
	} else {
	    uloop() ;
	}
    }

    /* savewolf() ; */

    fprintf(fpo,"THE ROUTE COST OF THE CURRENT PLACEMENT: %d\n", 
						funccost ) ;
    fprintf(fpo,"THE CURRENT PENALTY IS: %d   ", penalty ) ;
    fprintf(fpo,"THE CURRENT TOTAL COST IS: %d\n", 
				    penalty + overfill + funccost );
    fprintf(fpo,"THE CURRENT OVERFILL IS: %d\n", overfill ) ;
    fprintf(fpo,"TEMPERATURE = kT where k=1.0 and T=%g   ",T );
    fprintf(fpo,"OFFSET: %d\n", offset ) ;
    fprintf(fpo,"ITERATION NUMBER:%8d\n\n\n", ++count );
    fflush( fpo ) ;
    if( (count - 1) % 15 == 0 ) {
	printf("\n");
    }
    printf("%3d ", count );
    fflush( stdout ) ;

    T = Tfactor * T ;

    if( (finalShot < doCompaction && T < 0.8) || 
	(finalShot == 0 && T < 5.0 * Tscale ) ||
	(finalShot == doCompaction && T < 0.2) ) {
	/*
	 *  * WE ARE FINISHED *
	 */

	fprintf(fpo,"FINAL ROUTING COST OF THE PLACEMENT: %d\n"
						, funccost ) ;
	fprintf(fpo,"FINAL VALUE OF PENALTY IS: %d   ",penalty);
	fprintf(fpo,"FINAL VALUE OF TOTAL COST IS: %d\n", 
			    funccost + penalty + overfill ) ;

	fprintf(fpo,"THE FINAL OVERFILL IS: %d\n", overfill ) ;
	fprintf(fpo,"MAX NUMBER OF ATTEMPTED FLIPS PER T:%8d\n",
						    attmax ) ;
	break ;
    }
}
return ;
}
