/* -*- mode: c -*-
 * $Id: ackermann.c 35417 2007-03-28 04:46:30Z jeffc $
 * http://www.bagley.org/~doug/shootout/
 */

#include <stdio.h>
#include <stdlib.h>

int 
Ack(int M, int N) {
    if (M == 0) return( N + 1 );
    if (N == 0) return( Ack(M - 1, 1) );
    return( Ack(M - 1, Ack(M, (N - 1))) );
}

int
main(int argc, char *argv[]) {
    int n = ((argc == 2) ? atoi(argv[1]) : 8);

    printf("Ack(3,%d): %d\n", n, Ack(3, n));
    return(0);
}

