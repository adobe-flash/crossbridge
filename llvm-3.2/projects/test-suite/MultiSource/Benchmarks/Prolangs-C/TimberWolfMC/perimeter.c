#include "custom.h"
extern BUSTBOXPTR A ;

int perimeter(void)
{
int i , sum ;

sum = 0 ;
for( i = 1 ; i <= A[0].xc ; i++ ) {
    if( i == A[0].xc ) {
	sum += ABS(A[1].xc - A[i].xc) + ABS(A[1].yc - A[i].yc) ;
    } else {
	sum += ABS(A[i + 1].xc - A[i].xc) + ABS(A[i + 1].yc - A[i].yc) ;
    }
}
return( sum ) ;
}
