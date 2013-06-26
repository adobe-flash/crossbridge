#include "custom.h"
/* #define DEBUG */
  /*
   *  P   : current vertex point
   *  u() : lowest point directly above P.  Returns 0 if none. 
   *  l() : rightmost point directly left of P.  Returns 0 if none.
   *  b() : highest point directly below P.  Returns 0 if none.
   *  r() : leftmost point directly right of P.  Returns 0 if none.
   *  outputP() : outputs vertex included in outline.
   *  elim()    : deletes vertex from list.
   */
typedef struct Rangle {
    int rl ;
    int rr ;
    int rb ;
    int rt ;
} RANGLE ;
extern RANGLE *rectang ;
extern BUSTBOXPTR PtsArray ;
extern BUSTBOXPTR PtsOut   ;
extern FILE *fpo ;

int u(int Pt);
int l(int Pt);
int r(int Pt);
int b(int Pt);
void elim( int Pt );
void outputP( int Pt );
void condense(void);
int firstP(void);
void coincide(void);
void dumpError(void);

void unbust(void)
{
    int P , status , P1 ;

    coincide() ;  /* eliminate coincidental points */
    P = firstP() ; /* P is the leftmost of lowest vertices. 
    /*
     *  the variable status reflects the direction the last
     *  step took.  If the last step was: 
     *  up then status = 1 ;
     *  left then status = 2 ;
     *  right then status = 3 ;
     *  bottom ( or down ) then status = 4 ;
     *
     *  After a u move, the next possibilities are ( in order ):
     * 				l , u , r
     *  After a l move, the next possibilities are ( in order ):
     * 				b , l , u
     *  After a r move, the next possibilities are ( in order ):
     * 				u , r , b
     *  After a b move, the next possibilities are ( in order ):
     * 				r , b , l
     *  If none of the possibilities are satisfied, there are
     *  no more vertices to consider.
     *
     */
    if( (P1 = u(P)) == 0 ) {
	fprintf(fpo,"Serious error: could not advance upward ");
	fprintf(fpo,"from first vertex\n");
	exit(0);
    }
    outputP(P);
    P = P1 ;
    status = 1 ;
    for( ; ; ) {
	switch( status ) {
	    case 1 : 
		    if( (P1 = l(P)) == 0 ) {
			if( (P1 = u(P)) == 0 ) {
			    if( (P1 = r(P)) == 0 ) {
				outputP(P);
				condense() ;
				return ;
			    } else {
				outputP(P);
				elim(P);
				if( P1 <= PtsArray[0].xc ) {
				    P = P1 ;
				}
				status = 3 ;
			    }
			} else {
			    outputP(P);
			    elim(P);
			    if( P1 <= PtsArray[0].xc ) {
				P = P1 ;
			    }
			    status = 1 ;
			}
		    } else {
			outputP(P);
			elim(P);
			if( P1 <= PtsArray[0].xc ) {
			    P = P1 ;
			}
			status = 2 ;
		    }
		    break ;
	    case 2 :
		    if( (P1 = b(P)) == 0 ) {
			if( (P1 = l(P)) == 0 ) {
			    if( (P1 = u(P)) == 0 ) {
				outputP(P);
				condense() ;
				return ;
			    } else {
				outputP(P);
				elim(P);
				if( P1 <= PtsArray[0].xc ) {
				    P = P1 ;
				}
				status = 1 ;
			    }
			} else {
			    outputP(P);
			    elim(P);
			    if( P1 <= PtsArray[0].xc ) {
				P = P1 ;
			    }
			    status = 2 ;
			}
		    } else {
			outputP(P);
			elim(P);
			if( P1 <= PtsArray[0].xc ) {
			    P = P1 ;
			}
			status = 4 ;
		    }
		    break ;
	    case 3 :
		    if( (P1 = u(P)) == 0 ) {
			if( (P1 = r(P)) == 0 ) {
			    if( (P1 = b(P)) == 0 ) {
				outputP(P);
				condense() ;
				return ;
			    } else {
				outputP(P);
				elim(P);
				if( P1 <= PtsArray[0].xc ) {
				    P = P1 ;
				}
				status = 4 ;
			    }
			} else {
			    outputP(P);
			    elim(P);
			    if( P1 <= PtsArray[0].xc ) {
				P = P1 ;
			    }
			    status = 3 ;
			}
		    } else {
			outputP(P);
			elim(P);
			if( P1 <= PtsArray[0].xc ) {
			    P = P1 ;
			}
			status = 1 ;
		    }
		    break ;
	    case 4 :
		    if( (P1 = r(P)) == 0 ) {
			if( (P1 = b(P)) == 0 ) {
			    if( (P1 = l(P)) == 0 ) {
				outputP(P);
				condense() ;
				return ;
			    } else {
				outputP(P);
				elim(P);
				if( P1 <= PtsArray[0].xc ) {
				    P = P1 ;
				}
				status = 2 ;
			    }
			} else {
			    outputP(P);
			    elim(P);
			    if( P1 <= PtsArray[0].xc ) {
				P = P1 ;
			    }
			    status = 4 ;
			}
		    } else {
			outputP(P);
			elim(P);
			if( P1 <= PtsArray[0].xc ) {
			    P = P1 ;
			}
			status = 3 ;
		    }
		    break ;
	}
    }
}

void addPts( int l , int r , int b , int t )
{

    PtsArray[ ++PtsArray[0].xc ].xc = l ;
    PtsArray[   PtsArray[0].xc ].yc = b ;
    PtsArray[ ++PtsArray[0].xc ].xc = l ;
    PtsArray[   PtsArray[0].xc ].yc = t ;
    PtsArray[ ++PtsArray[0].xc ].xc = r ;
    PtsArray[   PtsArray[0].xc ].yc = b ;
    PtsArray[ ++PtsArray[0].xc ].xc = r ;
    PtsArray[   PtsArray[0].xc ].yc = t ;
    rectang[ ++rectang[0].rl ].rl = l ;
    rectang[   rectang[0].rl ].rr = r ;
    rectang[   rectang[0].rl ].rb = b ;
    rectang[   rectang[0].rl ].rt = t ;
    return ;
}

/*ARGSUSED*/
void initPts( int cell , char *name , int orient )
{
    int i ;

#ifdef DEBUG
    printf("cell:%d  name: %s  orient:%d\n",cell,name,orient);
    fflush(stdout);
#endif
    for( i = 0 ; i <= 100 ; i++ ) {
	PtsArray[i].xc = 0 ;
	PtsArray[i].yc = 0 ;
	PtsOut[i].xc = 0 ;
	PtsOut[i].xc = 0 ;
    }
    for( i = 0 ; i <= 25 ; i++ ) {
	rectang[i].rl = 0 ;
	rectang[i].rr = 0 ;
	rectang[i].rb = 0 ;
	rectang[i].rt = 0 ;
    }
    return ;
}

int recCheck( int Pt1 , int Pt2 ) 
{
    int Px1 , Py1 , Px2 , Py2 , i , rl , rr , rb , rt ;
    
    Px1 = PtsArray[Pt1].xc ;
    Py1 = PtsArray[Pt1].yc ;
    Px2 = PtsArray[Pt2].xc ;
    Py2 = PtsArray[Pt2].yc ;
    for( i = 1 ; i <= rectang[0].rl ; i++ ) {
	rl = rectang[i].rl ;
	rr = rectang[i].rr ;
	rb = rectang[i].rb ;
	rt = rectang[i].rt ;
	if( Px1 >= rl && Px1 <= rr && Py1 >= rb && Py1 <= rt ) {
	    /*  Pt1 is contained by this rectangle */
	    if( Px2 >= rl && Px2 <= rr && Py2 >= rb && Py2 <= rt ) {
		/*  Pt2 is also contained by this rectangle */
		return(1) ;
	    }
	}
    }
    return(0) ;
}

int u(int Pt)
{
    int i , ptx , pty , small , smallP ;

    small = 100000000 ;
    smallP = 0 ;
    ptx = PtsArray[Pt].xc ;
    pty = PtsArray[Pt].yc ;
    for( i = 1 ; i <= PtsArray[0].xc ; i++ ) {
	if( i == Pt ) {
	    continue ;
	}
	if( PtsArray[i].xc != ptx ) {
	    continue ;
	}
	/*  Now we have another point on the same vert. line */
	if( PtsArray[i].yc > pty ) {
	    if( PtsArray[i].yc - pty < small ) {
		small = PtsArray[i].yc - pty ;
		smallP = i ;
	    }
	}
    }
    if( smallP != 0 ) {
	/* check if two endpoints belong to same rectangle */
	if( recCheck( Pt , smallP ) == 0 ) {
	    /* do not belong to same rectangle */
	    smallP = 0 ;
	}
    }
    return( smallP ) ;
}


int l(int Pt)
{
    int i , ptx , pty , small , smallP ;

    small = 100000000 ;
    smallP = 0 ;
    ptx = PtsArray[Pt].xc ;
    pty = PtsArray[Pt].yc ;
    for( i = 1 ; i <= PtsArray[0].xc ; i++ ) {
	if( i == Pt ) {
	    continue ;
	}
	if( PtsArray[i].yc != pty ) {
	    continue ;
	}
	/*  Now we have another point on the same horz. line */
	if( PtsArray[i].xc < ptx ) {
	    if( ptx - PtsArray[i].xc < small ) {
		small = ptx - PtsArray[i].xc ;
		smallP = i ;
	    }
	}
    }
    if( smallP != 0 ) {
	/* check if two endpoints belong to same rectangle */
	if( recCheck( Pt , smallP ) == 0 ) {
	    /* do not belong to same rectangle */
	    smallP = 0 ;
	}
    }
    return( smallP ) ;
}


int r(int Pt)
{
    int i , ptx , pty , small , smallP ;

    small = 100000000 ;
    smallP = 0 ;
    ptx = PtsArray[Pt].xc ;
    pty = PtsArray[Pt].yc ;
    for( i = 1 ; i <= PtsArray[0].xc ; i++ ) {
	if( i == Pt ) {
	    continue ;
	}
	if( PtsArray[i].yc != pty ) {
	    continue ;
	}
	/*  Now we have another point on the same horz. line */
	if( PtsArray[i].xc > ptx ) {
	    if( PtsArray[i].xc - ptx < small ) {
		small = PtsArray[i].xc - ptx ;
		smallP = i ;
	    }
	}
    }
    if( smallP != 0 ) {
	/* check if two endpoints belong to same rectangle */
	if( recCheck( Pt , smallP ) == 0 ) {
	    /* do not belong to same rectangle */
	    smallP = 0 ;
	}
    }
    return( smallP ) ;
}


int b(int Pt)
{
    int i , ptx , pty , small , smallP ;

    small = 100000000 ;
    smallP = 0 ;
    ptx = PtsArray[Pt].xc ;
    pty = PtsArray[Pt].yc ;
    for( i = 1 ; i <= PtsArray[0].xc ; i++ ) {
	if( i == Pt ) {
	    continue ;
	}
	if( PtsArray[i].xc != ptx ) {
	    continue ;
	}
	/*  Now we have another point on the same vert. line */
	if( PtsArray[i].yc < pty ) {
	    if( pty - PtsArray[i].yc < small ) {
		small = pty - PtsArray[i].yc ;
		smallP = i ;
	    }
	}
    }
    if( smallP != 0 ) {
	/* check if two endpoints belong to same rectangle */
	if( recCheck( Pt , smallP ) == 0 ) {
	    /* do not belong to same rectangle */
	    smallP = 0 ;
	}
    }
    return( smallP ) ;
}


void elim( int Pt )
{
    PtsArray[Pt].xc = PtsArray[ PtsArray[0].xc   ].xc ;
    PtsArray[Pt].yc = PtsArray[ PtsArray[0].xc-- ].yc ;
    return ;
}

void outputP( int Pt )
{
    PtsOut[ ++PtsOut[0].xc ].xc = PtsArray[Pt].xc ;
    PtsOut[   PtsOut[0].xc ].yc = PtsArray[Pt].yc ;
    return ;
}

void condense(void)
{
    int P1x , P1y , P2x , P2y , P3x , P3y , j , P1 , P2 , P3 ;

    if( PtsArray[0].xc != 1 ) {
	printf("Fatal error in unbust.c\n");
	dumpError() ;
	exit(0) ;
    }
    P1  = 1 ;
    P2  = 2 ;
    for( P3 = 3 ; P3 <= PtsOut[0].xc ; P3++ ) {
	P1x = PtsOut[P1].xc ;
	P1y = PtsOut[P1].yc ;
	P2x = PtsOut[P2].xc ;
	P2y = PtsOut[P2].yc ;
	P3x = PtsOut[P3].xc ;
	P3y = PtsOut[P3].yc ;
	if( P1x == P2x ) {  /* V segment */
	    if( P3x == P1x ) {  /* P2 is extra */
		for( j = P3 ; j <= PtsOut[0].xc ; j++ ) {
		    PtsOut[j - 1].xc = PtsOut[j].xc ;
		    PtsOut[j - 1].yc = PtsOut[j].yc ;
		}
		PtsOut[0].xc-- ;
		P2 = --P3 ;
	    } else {
		P1 = P2 ;
		P2 = P3 ;
	    }
	} else if( P1y == P2y ) {  /* H segment */
	    if( P3y == P1y ) {  /* P2 is extra */
		for( j = P3 ; j <= PtsOut[0].xc ; j++ ) {
		    PtsOut[j - 1].xc = PtsOut[j].xc ;
		    PtsOut[j - 1].yc = PtsOut[j].yc ;
		}
		PtsOut[0].xc-- ;
		P2 = --P3 ;
	    } else {
		P1 = P2 ;
		P2 = P3 ;
	    }
	} else {
	    fprintf(fpo,"Two points in outline not rectilinear ");
	    fprintf(fpo,"and this constitutes death\n");
	    exit(0);
	}
    }
    return ;
}


int firstP(void)
{
    int k , ymin , xmin , P ;

    ymin = 100000000 ;
    for( k = 1 ; k <= PtsArray[0].xc ; k++ ) {
	if( PtsArray[k].yc < ymin ) {
	    ymin = PtsArray[k].yc ;
	}
    }  /* we have the lowest y coordinate  */
    xmin = 100000000 ;
    for( k = 1 ; k <= PtsArray[0].xc ; k++ ) {
	if( PtsArray[k].yc == ymin ) {
	    if( PtsArray[k].xc < xmin ) {
		xmin = PtsArray[k].xc ;
		P = k ;
	    }
	}
    }  /*  we have the leftmost of the lowest vertices  */
    return( P );
}


void coincide(void)
{
    int i , x0 , y0 , j ;
    /*
     *  Now eliminate coincidental points
     */
    for( i = 1 ; i <= PtsArray[0].xc ; i++ ) {
	x0 = PtsArray[i].xc ;
	y0 = PtsArray[i].yc ;
	for( j = i + 1 ; j <= PtsArray[0].xc ; j++ ) {
	    if( x0 == PtsArray[j].xc  &&  y0 == PtsArray[j].yc ) {
		/* eliminate point j */
		PtsArray[j  ].xc = PtsArray[ PtsArray[0].xc   ].xc ;
		PtsArray[j--].yc = PtsArray[ PtsArray[0].xc-- ].yc ;
	    }
	}
    }
}


void dumpError(void)
{

int i , x , y , l , r , b , t ;

printf("PtsOut:\n");
for( i = 1 ; i <= PtsOut[0].xc ; i++ ) {
    x = PtsOut[i].xc ;
    y = PtsOut[i].yc ;
    printf("x:%d  y:%d\n", x , y ) ;
}

printf("PtsArray:\n");
for( i = 1 ; i <= PtsArray[0].xc ; i++ ) {
    x = PtsArray[i].xc ;
    y = PtsArray[i].yc ;
    printf("x:%d  y:%d\n", x , y ) ;
}

printf("Rectangles:\n");
for( i = 1 ; i <= rectang[0].rl ; i++ ) {
    l = rectang[i].rl ;
    r = rectang[i].rr ;
    b = rectang[i].rb ;
    t = rectang[i].rt ;
    printf("l:%d  r:%d  b:%d  t:%d\n", l , r , b , t ) ;
}

fflush(stdout) ;
return ;
}
