#include "custom.h"
extern BUSTBOXPTR A , R ;


void buster(void)
{

int k , Pk[2] , Pl[2] , Pm[2]  ;
int xmin , ymin , kmin , found ;


/*  find Pk  */
ymin = 1000000000 ;
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( A[k].yc < ymin ) {
	ymin = A[k].yc ;
    }
}  /* we have the lowest y coordinate  */
xmin = 1000000000 ;
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( A[k].yc == ymin ) {
	if( A[k].xc < xmin ) {
	    xmin = A[k].xc ;
	    kmin = k ;
	}
    }
}  /*  we have the leftmost lowest corner  */
Pk[0] = xmin ;
Pk[1] = ymin ;
xmin = 1000000000 ;
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( k == kmin ) {
	continue ;
    }
    if( A[k].yc == ymin ) {
	if( A[k].xc < xmin ) {
	    xmin = A[k].xc ;
	}
    }
}   /*  we have the next leftmost lowest corner  */
Pl[0] = xmin ;
Pl[1] = ymin ;
ymin = 1000000000 ;
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( A[k].yc == Pk[1] ) {
	continue ;
    }
    if( A[k].yc < ymin ) {
	ymin = A[k].yc ;
    }
}  /* we have the next lowest y coordinate  */
xmin = 1000000000 ;
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( A[k].yc == ymin ) {
	if( A[k].xc < Pk[0] || A[k].xc > Pl[0] ) {
	    continue ;
	}
	if( A[k].xc < xmin ) {
	    xmin = A[k].xc ;
	}
    }
}  /*  we have the leftmost next lowest corner  */
Pm[0] = xmin ;
Pm[1] = ymin ;

/*
 *  According to the instruction sheet I read, we can
 *  output the bounding rectangle of Pk , Pl , Pm.
 */
R[1].xc = Pk[0] ;
R[1].yc = Pk[1] ;
R[2].xc = Pk[0] ;
R[2].yc = Pm[1] ;
R[3].xc = Pl[0] ;
R[3].yc = Pm[1] ;
R[4].xc = Pl[0] ;
R[4].yc = Pk[1] ;

/*  
 *  Now weed out those elements of R which are in A and
 *  add those elements of R which are not in A.
 *  Note that index 1 and 4 are necessarily in A, and thus
 *  have to be removed from A.
 */
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( R[1].xc == A[k].xc && R[1].yc == A[k].yc ) {
	A[k].xc = A[ A[0].xc ].xc ;
	A[k].yc = A[ A[0].xc-- ].yc ;
	break ;
    }
}
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( R[4].xc == A[k].xc && R[4].yc == A[k].yc ) {
	A[k].xc = A[ A[0].xc ].xc ;
	A[k].yc = A[ A[0].xc-- ].yc ;
	break ;
    }
}
found = 0 ;
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( R[2].xc == A[k].xc && R[2].yc == A[k].yc ) {
	A[k].xc = A[ A[0].xc ].xc ;
	A[k].yc = A[ A[0].xc-- ].yc ;
	found = 1 ;
	break ;
    }
}
if( found == 0 ) {
    /*
     *  Add the thing to the list A
     */
    A[ ++A[0].xc ].xc = R[2].xc ;
    A[ A[0].xc ].yc = R[2].yc ;
}
found = 0 ;
for( k = 1 ; k <= A[0].xc ; k++ ) {
    if( R[3].xc == A[k].xc && R[3].yc == A[k].yc ) {
	A[k].xc = A[ A[0].xc ].xc ;
	A[k].yc = A[ A[0].xc-- ].yc ;
	found = 1 ;
	break ;
    }
}
if( found == 0 ) {
    /*
     *  Add the thing to the list A
     */
    A[ ++A[0].xc ].xc = R[3].xc ;
    A[ A[0].xc ].yc = R[3].yc ;
}
}
