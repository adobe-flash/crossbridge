/*****************************************************************/
/******     C  _  P  R  I  N  T  _  R  E  S  U  L  T  S     ******/
/*****************************************************************/
#include <stdlib.h>
#include <stdio.h>

void c_print_results( char   *name,
                      char   class,
                      int    n1, 
                      int    n2,
                      int    n3,
                      int    niter,
		      char   *optype,
                      int    passed_verification,
                      char   *npbversion)
{
    printf( "\n\n %s Benchmark Completed\n", name ); 

    printf( " Class           =                        %c\n", class );

    if( n3 == 0 ) {
        long nn = n1;
        if ( n2 != 0 ) nn *= n2;
        printf( " Size            =             %12ld\n", nn );   /* as in IS */
    }
    else
        printf( " Size            =             %4dx%4dx%4d\n", n1,n2,n3 );

    printf( " Iterations      =             %12d\n", niter );

    printf( " Operation type  = %24s\n", optype);

    if( passed_verification < 0 )
        printf( " Verification    =            NOT PERFORMED\n" );
    else if( passed_verification )
        printf( " Verification    =               SUCCESSFUL\n" );
    else
        printf( " Verification    =             UNSUCCESSFUL\n" );

    printf( " Version         =             %12s\n", npbversion );
    
#ifdef SMP
    evalue = getenv("MP_SET_NUMTHREADS");
    printf( "   MULTICPUS = %s\n", evalue );
#endif

    printf( "\n\n" );
    printf( " Please send all errors/feedbacks to:\n\n" );
    printf( " NPB Development Team\n" );
    printf( " npb@nas.nasa.gov\n\n" );
}