#include "custom.h"
extern char **spots ;
extern int spotXhash ;
extern int spotYhash ;
extern int numXspots ;
extern int numYspots ;
extern int spotSize  ;
extern int choose ;
extern int bigcell ;
extern int toobig  ;
int flips , flipp , attp ;
int newxx , newyy ;
extern double Tsave ;
extern int rangeLimit ;
extern int count ;

extern int newOrient( CELLBOXPTR cellptr , int range );
extern int usite0( int a , int newaor );
extern int wireestx( int xc , int yy1 , int yy2 , double factor3 );
extern int wireesty( int yc , int xx1 , int xx2 , double factor3 );
extern int uaspect( int a , double newAspect );
extern int usite1( int a , int xb , int yb );
extern int usiteo1( int a , int xb , int yb , int newaor );
extern int usite2( int a , int b , int ax , int ay , int bx , int by );
extern int usiteo2( int a , int b , int ax , int ay , int bx , int by, 
		   int newaor ,int newbor );
extern void selectpin( CELLBOXPTR acellptr );

void forceGrid( int x , int y );
void pickSpot( CELLBOXPTR cellptr , int orient , int ll , int rr , int bb , 
	      int tt , int *x , int *y );
void fixSpot( CELLBOXPTR cellptr , int oldx , int oldy , int oldorient );
void fixSpotAsp(CELLBOXPTR cellptr,int lft,int rte,int bot,int top);

void uloop(void)
{

CELLBOXPTR acellptr , bcellptr ;
TILEBOXPTR tileptr ;
TERMBOXPTR termptr , btermptr ;
double range , newAspect;
int a , b , numberx , numbery ;
int attempts , i ;
int xb , yb , axcenter , aycenter ;
int bxcenter , bycenter ;
int aorient , borient , newaor , newbor ;
int flip0, flipo, flip2, flipo2, flipa, flip1 ;
int att0, atto, att2, atto2, atta, att1 ;
int ll , rr , bb , tt , ax , ay , bx , by ;
int reject ;
int al , ar , ab , at ;
int bl , br , bt , min , minstep ;
int delt1 , delt2 ;
int lft, rte, bot, top, xcenter, ycenter ;

flips    = 0 ;
attempts = 0 ;
flip0    = 0 ;
att0     = 0 ;
flipo    = 0 ;
atto     = 0 ;
flip2    = 0 ;
att2     = 0 ;
flipo2   = 0 ;
atto2    = 0 ;
flipa    = 0 ;
atta     = 0 ;
flip1    = 0 ;
att1     = 0 ;
flipp    = 0 ;
attp     = 0 ;


numberx = rangeLimit ;
numbery = rangeLimit ;
if( count < 1 ) {
    fprintf(fpo,"range limiter:%d units either way in x",numberx);
    fprintf(fpo,"   compared to bdxlength:%d\n",bdxlength);
    fprintf(fpo,"range limiter:%d units either way in y",numbery);
    fprintf(fpo,"   compared to bdylength:%d\n",bdylength);
}

while( attempts < attmax ) {
    do {
	a = (int) ( (double) choose * ( (double) RAND / 
				    (double) 0x7fffffff ) ) + 1 ;
    } while( a == choose + 1 ) ;

    do {
	b = (int) ( (double) bigcell * ( (double) RAND / 
				    (double) 0x7fffffff ) ) + 1 ;
    } while( b == toobig ) ;

    if( a == b ) {
	continue ;
    }

    acellptr = cellarray[ a ]   ;
    aorient  = acellptr->orient ;

    if( a > numcells && b <= numcells ) {
	newaor = newOrient( acellptr , 8 ) ;
	if( newaor >= 0 ) {
	    if( usite0( a , newaor ) ) {
		flips++ ;
		flip0++ ;
	    }
	    att0++  ;
	}
    } else if( a <= numcells && b > numcells ) {
	if( acellptr->class != 0 ) {
	    continue ;
	}
	axcenter = acellptr->xcenter ;
	aycenter = acellptr->ycenter ;

	ll = (axcenter-numberx < blockl) ? blockl :
	     (axcenter - numberx > blockr ? blockr : axcenter - numberx);
	rr = (axcenter+numberx > blockr) ? blockr :
	     (axcenter + numberx < blockl ? blockl : axcenter + numberx);
	bb = (aycenter-numbery < blockb) ? blockb :
	     (aycenter - numbery > blockt ? blockt : aycenter - numbery);
	tt = (aycenter+numbery > blockt) ? blockt :
	     (aycenter + numbery < blockb ? blockb : aycenter + numbery);

	pickSpot( acellptr , aorient , ll , rr , bb , tt , &xb , &yb ) ;

	if( gridGiven ) {
	    termptr  = acellptr->config[aorient]->termptr ;
	    forceGrid( xb + termptr->xpos , yb + termptr->ypos ) ;
	    xb = newxx - termptr->xpos ;
	    yb = newyy - termptr->ypos ;
	}

        if( usite1( a , xb , yb ) ) {
	    flips++ ;
	    flip1++ ;
	    att1++ ;
	    attempts++ ;

	    fixSpot( acellptr , axcenter , aycenter , aorient ) ;

	} else {
	    attempts++ ;
	    att1++ ;
	    newaor = newOrient( acellptr , 4 ) ;
	    reject = 1 ;
	    if( newaor >= 0 ) {
		if( usiteo1( a , xb , yb , newaor ) ) {
		    flips++ ;
		    flipo++ ;
		    atto++  ;
		    reject = 0 ;

		    fixSpot( acellptr , axcenter , aycenter , aorient ) ;

		} else {
		    atto++  ;
		}
	    }
	    if( reject ) {
		newaor = newOrient( acellptr , 8 ) ;
		reject = 1 ;
		if( newaor >= 0 ) {
		    if( usite0( a , newaor ) ) {
			flips++ ;
			flip0++ ;
			att0++  ;
			reject = 0 ;

			fixSpot( acellptr, axcenter, aycenter, aorient);

		    } else {
			att0++  ;
		    }
		}
	    }
	}
	if( acellptr->numUnComTerms != 0 ){
	    for( i = 1 ; i <= acellptr->numgroups ; i++ ) {
		selectpin( acellptr ) ;
	    }
	}
	/*
	 *   The goal here is to generate a new
	 *   aspect ratio for the cell if such
	 *   a thing is permitted.  First test
	 *   for permission.
	 */
	if( acellptr->softflag != 0 &&
		(acellptr->aspUB > 0.01 + acellptr->aspLB)){
	    /*
	     *   We have clearance for an aspect
	     *   ratio change
	     */
	    range = acellptr->aspUB - acellptr->aspLB;
	    newAspect = range * ((double)RAND / 
		    (double) 0x7fffffff) + acellptr->aspLB ;

	    xcenter = acellptr->xcenter ;
	    ycenter = acellptr->ycenter ;
	    lft = xcenter + acellptr->config[acellptr->orient]->left   ;
	    rte = xcenter + acellptr->config[acellptr->orient]->right  ;
	    bot = ycenter + acellptr->config[acellptr->orient]->bottom ;
	    top = ycenter + acellptr->config[acellptr->orient]->top    ;

	    if( uaspect( a , newAspect ) ) {
		flips++ ;
		flipa++ ;

		fixSpotAsp(acellptr,lft,rte,bot,top) ;
	    } 
	    atta++ ;
	}
    } else { /*  a & b <= numcells or a & b > numcells  */
	bcellptr = cellarray[b] ;
	borient = bcellptr->orient ;
	if( acellptr->class == -1 || bcellptr->class == -1 ||
			    acellptr->class != bcellptr->class ) {
	    continue ;
	}
	if( gridGiven ) {
	    /*
	     *   Force pin 1 to lie on the underlying grid, specified
	     *   by:  n * (grid) + offset , in each direction.
	     */
	    btermptr = bcellptr->config[bcellptr->orient]->termptr ;
	    termptr = acellptr->config[aorient]->termptr ;
	    forceGrid( bcellptr->xcenter + termptr->xpos , 
		       bcellptr->ycenter + termptr->ypos ) ;
	    ax = newxx - termptr->xpos ;
	    ay = newyy - termptr->ypos ;
	    forceGrid( acellptr->xcenter + btermptr->xpos , 
		       acellptr->ycenter + btermptr->ypos ) ;
	    bx = newxx - btermptr->xpos ;
	    by = newyy - btermptr->ypos ;
	} else {
	    ax = bcellptr->xcenter ;
	    ay = bcellptr->ycenter ;
	    bx = acellptr->xcenter ;
	    by = acellptr->ycenter ;
	}
	axcenter = bx ;
	aycenter = by ;
	bxcenter = ax ;
	bycenter = ay ;
	/* ****************************************************** */
	/*  SOMETHING NEW  */
	tileptr = acellptr->config[ aorient ]  ;
	al = ax + tileptr->left    ;
	ar = ax + tileptr->right   ;
	ab = ay + tileptr->bottom  ;
	at = ay + tileptr->top     ;
	if( a <= numcells ) {
	    al -= wireestx(al,ab,at,tileptr->lweight ) ;
	    ar += wireestx(ar,ab,at,tileptr->rweight ) ;
	    ab -= wireesty(ab,al,ar,tileptr->bweight ) ;
	    at += wireesty(at,al,ar,tileptr->tweight ) ;
	}
	tileptr = bcellptr->config[ bcellptr->orient ]  ;
	bl = bx + tileptr->left    ;
	br = bx + tileptr->right   ;
	bb = by + tileptr->bottom  ;
	bt = by + tileptr->top     ;
	if( b <= numcells ) {
	    bl -= wireestx(bl,bb,bt,tileptr->lweight ) ;
	    br += wireestx(br,bb,bt,tileptr->rweight ) ;
	    bb -= wireesty(bb,bl,br,tileptr->bweight ) ;
	    bt += wireesty(bt,bl,br,tileptr->tweight ) ;
	}
	min = 1000000 ;
	minstep = 0 ;
	if( bl >= ar || al >= br || bb >= at || ab >= bt ) {
	    if( bl < ar ) {
		if( ar - bl < min ) { 
		    minstep = 1 ;
		    min = ar - bl ;
		}
	    }
	    if( al < br ) {
		if( br - al < min ) { 
		    minstep = 2 ;
		    min = br - al ;
		}
	    }
	    if( bb < at ) {
		if( at - bb < min ) { 
		    minstep = 3 ;
		    min = at - bb ;
		}
	    }
	    if( ab < bt ) {
		if( bt - ab < min ) { 
		    minstep = 4 ;
		    min = bt - ab ;
		}
	    }
	}
	if( minstep != 0 ) {
	    delt1 = min / 2 ;
	    delt2 = min - min / 2 ;
	    if( minstep == 1 ) {
		bx += delt1 ;
		ax -= delt2 ;
		if( br + delt1 > blockr ) {
		    bx -= br + delt1 - blockr ;
		    ax -= br + delt1 - blockr ;
		} else if( al - delt2 < blockl ) {
		    bx += blockl - (al - delt2) ;
		    ax += blockl - (al - delt2) ;
		}
	    } else if( minstep == 2 ) {
		ax += delt1 ;
		bx -= delt2 ;
		if( ar + delt1 > blockr ) {
		    bx -= ar + delt1 - blockr ;
		    ax -= ar + delt1 - blockr ;
		} else if( bl - delt2 < blockl ) {
		    bx += blockl - (bl - delt2) ;
		    ax += blockl - (bl - delt2) ;
		}
	    } else if( minstep == 3 ) {
		by += delt1 ;
		ay -= delt2 ;
		if( bt + delt1 > blockt ) {
		    by -= bt + delt1 - blockt ;
		    ay -= bt + delt1 - blockt ;
		} else if( ab - delt2 < blockb ) {
		    by += blockb - (ab - delt2) ;
		    ay += blockb - (ab - delt2) ;
		}
	    } else {
		ay += delt1 ;
		by -= delt2 ;
		if( at + delt1 > blockt ) {
		    by -= at + delt1 - blockt ;
		    ay -= at + delt1 - blockt ;
		} else if( bb - delt2 < blockb ) {
		    by += blockb - (bb - delt2) ;
		    ay += blockb - (bb - delt2) ;
		}
	    }
	}
	/* ****************************************************** */
	if( usite2( a , b , ax , ay , bx , by )  ) {
	    flips++ ;
	    flip2++ ;
	    att2++  ;
	    attempts++ ;

	    fixSpot( acellptr , axcenter , aycenter , aorient ) ;
	    fixSpot( bcellptr , bxcenter , bycenter , borient ) ;

	} else {
	    att2++  ;
	    attempts++ ;
	    /*
		try again with opposite orientation types
	    */
	    newaor = newOrient( acellptr , 4 ) ;
	    newbor = newOrient( bcellptr , 4 ) ;
	    if( newaor >= 0 || newbor >= 0 ) {
		if( newaor < 0 ) {
		    newaor = aorient ;
		} else if( newbor < 0 ) {
		    newbor = borient ;
		}
		if( gridGiven ) {
		    btermptr = bcellptr->config[newbor]-> termptr ;
		    termptr = acellptr->config[newaor]->termptr ;
		    forceGrid( bcellptr->xcenter + termptr->xpos , 
			       bcellptr->ycenter + termptr->ypos ) ;
		    ax = newxx - termptr->xpos ;
		    ay = newyy - termptr->ypos ;
		    forceGrid( acellptr->xcenter + btermptr->xpos , 
			       acellptr->ycenter + btermptr->ypos ) ;
		    bx = newxx - btermptr->xpos ;
		    by = newyy - btermptr->ypos ;
		} else {
		    ax = bcellptr->xcenter ;
		    ay = bcellptr->ycenter ;
		    bx = acellptr->xcenter ;
		    by = acellptr->ycenter ;
		}
		/* ************************************************** */
		/*  SOMETHING NEW  */
		tileptr = acellptr->config[ newaor ]  ;
		al = ax + tileptr->left    ;
		ar = ax + tileptr->right   ;
		ab = ay + tileptr->bottom  ;
		at = ay + tileptr->top     ;
		if( a <= numcells ) {
		    al -= wireestx(al,ab,at,tileptr->lweight ) ;
		    ar += wireestx(ar,ab,at,tileptr->rweight ) ;
		    ab -= wireesty(ab,al,ar,tileptr->bweight ) ;
		    at += wireesty(at,al,ar,tileptr->tweight ) ;
		}
		tileptr = bcellptr->config[ newbor ]  ;
		bl = bx + tileptr->left    ;
		br = bx + tileptr->right   ;
		bb = by + tileptr->bottom  ;
		bt = by + tileptr->top     ;
		if( b <= numcells ) {
		    bl -= wireestx(bl,bb,bt,tileptr->lweight ) ;
		    br += wireestx(br,bb,bt,tileptr->rweight ) ;
		    bb -= wireesty(bb,bl,br,tileptr->bweight ) ;
		    bt += wireesty(bt,bl,br,tileptr->tweight ) ;
		}
		min = 1000000 ;
		minstep = 0 ;
		if( bl >= ar || al >= br || bb >= at || ab >= bt ) {
		    if( bl < ar ) {
			if( ar - bl < min ) { 
			    minstep = 1 ;
			    min = ar - bl ;
			}
		    }
		    if( al < br ) {
			if( br - al < min ) { 
			    minstep = 2 ;
			    min = br - al ;
			}
		    }
		    if( bb < at ) {
			if( at - bb < min ) { 
			    minstep = 3 ;
			    min = at - bb ;
			}
		    }
		    if( ab < bt ) {
			if( bt - ab < min ) { 
			    minstep = 4 ;
			    min = bt - ab ;
			}
		    }
		}
		if( minstep != 0 ) {
		    delt1 = min / 2 ;
		    delt2 = min - min / 2 ;
		    if( minstep == 1 ) {
			bx += delt1 ;
			ax -= delt2 ;
			if( br + delt1 > blockr ) {
			    bx -= br + delt1 - blockr ;
			    ax -= br + delt1 - blockr ;
			} else if( al - delt2 < blockl ) {
			    bx += blockl - (al - delt2) ;
			    ax += blockl - (al - delt2) ;
			}
		    } else if( minstep == 2 ) {
			ax += delt1 ;
			bx -= delt2 ;
			if( ar + delt1 > blockr ) {
			    bx -= ar + delt1 - blockr ;
			    ax -= ar + delt1 - blockr ;
			} else if( bl - delt2 < blockl ) {
			    bx += blockl - (bl - delt2) ;
			    ax += blockl - (bl - delt2) ;
			}
		    } else if( minstep == 3 ) {
			by += delt1 ;
			ay -= delt2 ;
			if( bt + delt1 > blockt ) {
			    by -= bt + delt1 - blockt ;
			    ay -= bt + delt1 - blockt ;
			} else if( ab - delt2 < blockb ) {
			    by += blockb - (ab - delt2) ;
			    ay += blockb - (ab - delt2) ;
			}
		    } else {
			ay += delt1 ;
			by -= delt2 ;
			if( at + delt1 > blockt ) {
			    by -= at + delt1 - blockt ;
			    ay -= at + delt1 - blockt ;
			} else if( bb - delt2 < blockb ) {
			    by += blockb - (bb - delt2) ;
			    ay += blockb - (bb - delt2) ;
			}
		    }
		}
		/* ************************************************** */

		if( usiteo2( a, b, ax, ay, bx, by, newaor, newbor) ) {
		    flips++  ;
		    flipo2++ ;
		    atto2++  ;
		    attempts++ ;

		    fixSpot( acellptr , axcenter , aycenter , aorient ) ;
		    fixSpot( bcellptr , bxcenter , bycenter , borient ) ;

		} else {
		    atto2++  ;
		    attempts++ ;
		}
	    }
	}
    }
}
fprintf(fpo,"acceptance breakdown:\n"); 
fprintf(fpo,"              single cell: %d / %d\n", flip1 , att1) ;
fprintf(fpo,"     single w/ orient chg: %d / %d\n", flipo , atto) ;
fprintf(fpo,"               orient chg: %d / %d\n", flip0 , att0) ;
fprintf(fpo,"                pin moves: %d / %d\n", flipp , attp) ;
fprintf(fpo,"        aspect ratio chgs: %d / %d\n", flipa , atta) ;
fprintf(fpo,"        cell interchanges: %d / %d\n", flip2 , att2) ;
fprintf(fpo,"      interchgs w/ orient: %d / %d\n", flipo2 ,atto2);
fprintf(fpo,"\tno. of accepted flips: %d\n", flips ) ;
/*
doublecheck() ;
fprintf(fpo,"TimberWolf Wire Cost: %d   Penalty: %d\n\n",funccost,
							    penalty);
*/
return ;
}


void forceGrid( int x , int y )
{

newxx = ( (x - gOffsetX) / gridX ) * gridX + gOffsetX ;
if( ABS(newxx + gridX - x) < ABS(newxx - x) ) {
    newxx += gridX ;
}
newyy = ( (y - gOffsetY) / gridY ) * gridY + gOffsetY ;
if( ABS(newyy + gridY - y) < ABS(newyy - y) ) {
    newyy += gridY ;
}
return ;
}



void pickSpot( CELLBOXPTR cellptr , int orient , int ll , int rr , int bb , 
               int tt , int *x , int *y )
{

TILEBOXPTR tileptr ;
int lspot , rspot , bspot , tspot ;
int l , r , b , t ;
int llspot , rrspot , bbspot , ttspot ;
int xrange , yrange ;
int xspot , yspot ;
int xc , yc ;

llspot = (ll - spotXhash) / spotSize ;
rrspot = (rr - spotXhash) / spotSize ;
bbspot = (bb - spotYhash) / spotSize ;
ttspot = (tt - spotYhash) / spotSize ;
xrange = rrspot - llspot + 1 ;
yrange = ttspot - bbspot + 1 ;

tileptr = cellptr->config[orient] ;
xc = cellptr->xcenter    ;
yc = cellptr->ycenter    ;
l = xc + tileptr->left   ;
r = xc + tileptr->right  ;
b = yc + tileptr->bottom ;
t = yc + tileptr->top    ;

l -= wireestx(l,b,t,tileptr->lweight ) ;
r += wireestx(r,b,t,tileptr->rweight ) ;
b -= wireesty(b,l,r,tileptr->bweight ) ;
t += wireesty(t,l,r,tileptr->tweight ) ;

lspot = (l - spotXhash) / spotSize ;
rspot = (r - spotXhash) / spotSize ;
bspot = (b - spotYhash) / spotSize ;
tspot = (t - spotYhash) / spotSize ;

xspot = (cellptr->xcenter - spotXhash) / spotSize ;
yspot = (cellptr->ycenter - spotYhash) / spotSize ;


for( ; ; ) {
    *x = (int) ( (double) xrange * ( (double) RAND / 
				(double) 0x7fffffff ) ) + llspot ;
    *y = (int) ( (double) yrange * ( (double) RAND / 
				(double) 0x7fffffff ) ) + bbspot ;
    if( spots[*x][*y] == 0 ) {
	break ;
    }
    if( *x >= lspot && *x <= rspot && *y >= bspot && *y <= tspot 
		&& ((ABS(*x - xspot) > 1) || (ABS(*y - yspot) > 1)) ) {
	break ;
    }
}
/*
 *   Randomly select (*x,*y) location within spot [*x,*y]
 */
*x = *x * spotSize + spotXhash ;
*y = *y * spotSize + spotYhash ;

*x += (int)((double) spotSize * ((double) RAND / (double) 0x7fffffff));
*y += (int)((double) spotSize * ((double) RAND / (double) 0x7fffffff));

return ;
}


void fixSpot( CELLBOXPTR cellptr , int oldx , int oldy , int oldorient )
{

TILEBOXPTR tileptr ;
int lspot , rspot , bspot , tspot ;
int xc , yc , x , y ;
int l , r , b , t ;
register char *sarray ;

tileptr = cellptr->config[oldorient] ;
l = oldx + tileptr->left   ;
r = oldx + tileptr->right  ;
b = oldy + tileptr->bottom ;
t = oldy + tileptr->top    ;

l -= wireestx(l,b,t,tileptr->lweight ) ;
r += wireestx(r,b,t,tileptr->rweight ) ;
b -= wireesty(b,l,r,tileptr->bweight ) ;
t += wireesty(t,l,r,tileptr->tweight ) ;

lspot = (l - spotXhash) / spotSize ;
rspot = (r - spotXhash) / spotSize ;
bspot = (b - spotYhash) / spotSize ;
tspot = (t - spotYhash) / spotSize ;
for( x = lspot + 1 ; x < rspot ; x++ ) {
    sarray = spots[x] + bspot ;
    for( y = bspot + 1 ; y < tspot ; y++ ) {
	(*(++sarray))-- ;
    }
}

tileptr = cellptr->config[cellptr->orient] ;
xc = cellptr->xcenter    ;
yc = cellptr->ycenter    ;
l = xc + tileptr->left   ;
r = xc + tileptr->right  ;
b = yc + tileptr->bottom ;
t = yc + tileptr->top    ;

l -= wireestx(l,b,t,tileptr->lweight ) ;
r += wireestx(r,b,t,tileptr->rweight ) ;
b -= wireesty(b,l,r,tileptr->bweight ) ;
t += wireesty(t,l,r,tileptr->tweight ) ;

lspot = (l - spotXhash) / spotSize ;
rspot = (r - spotXhash) / spotSize ;
bspot = (b - spotYhash) / spotSize ;
tspot = (t - spotYhash) / spotSize ;

for( x = lspot + 1 ; x < rspot ; x++ ) {
    sarray = spots[x] + bspot ;
    for( y = bspot + 1 ; y < tspot ; y++ ) {
	(*(++sarray))++ ;
    }
}

return ;
}


void fixSpotAsp(CELLBOXPTR cellptr,int lft,int rte,int bot,int top)
{

TILEBOXPTR tileptr ;
int lspot , rspot , bspot , tspot ;
int xc , yc , x , y ;
int l , r , b , t ;
register char *sarray ;


/* XXX what the HELL is going on here, haven't you ever heard of lint! */
/* tileptr has not value at this point in time! */
l = lft - wireestx(lft,bot,top,tileptr->lweight ) ;
r = rte + wireestx(rte,bot,top,tileptr->rweight ) ;
b = bot - wireesty(bot,lft,rte,tileptr->bweight ) ;
t = top - wireesty(top,lft,rte,tileptr->tweight ) ;

lspot = (l - spotXhash) / spotSize ;
rspot = (r - spotXhash) / spotSize ;
bspot = (b - spotYhash) / spotSize ;
tspot = (t - spotYhash) / spotSize ;

for( x = lspot + 1 ; x < rspot ; x++ ) {
    sarray = spots[x] + bspot ;
    for( y = bspot + 1 ; y < tspot ; y++ ) {
	(*(++sarray))-- ;
    }
}

tileptr = cellptr->config[cellptr->orient] ;
xc = cellptr->xcenter    ;
yc = cellptr->ycenter    ;
l = xc + tileptr->left   ;
r = xc + tileptr->right  ;
b = yc + tileptr->bottom ;
t = yc + tileptr->top    ;

l -= wireestx(l,b,t,tileptr->lweight ) ;
r += wireestx(r,b,t,tileptr->rweight ) ;
b -= wireesty(b,l,r,tileptr->bweight ) ;
t += wireesty(t,l,r,tileptr->tweight ) ;

lspot = (l - spotXhash) / spotSize ;
rspot = (r - spotXhash) / spotSize ;
bspot = (b - spotYhash) / spotSize ;
tspot = (t - spotYhash) / spotSize ;

for( x = lspot + 1 ; x < rspot ; x++ ) {
    sarray = spots[x] + bspot ;
    for( y = bspot + 1 ; y < tspot ; y++ ) {
	(*(++sarray))++ ;
    }
}

return ;
}
