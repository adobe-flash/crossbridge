#include "custom.h"
extern int HorV ;
extern int nPinLocs ;
extern UNCOMBOX *UCptr ;
typedef struct flogbox {
    int pin ;
    int placed ;
    int finalx ;
    int finaly ;
}
FBOX ,
*FBOXPTR ;
extern FBOXPTR lArray ;



/*   
 *   We have here a little function which 
 *   takes as an argument the given UCpin and returns
 *   the best possible location in the site.
 */

int findLoc( int pin ) 
{

int bigoX , bigoY , litoX , litoY ;
int aPin , bestPos , meanx , meany , distant , i ;
NETBOXPTR netptr ;

bigoX = 0 ;
bigoY = 0 ;
litoX = 1000000000 ;
litoY = 1000000000 ;

aPin = UCptr[ pin ].terminal ;

netptr = netarray[ termarray[aPin]->net ]->netptr ;
for( ; netptr != NETNULL; netptr = netptr->nextterm ) {
    if( netptr->terminal != aPin ) {
	if( netptr->xpos < litoX ) {
	    litoX = netptr->xpos ;
	} 
	if( netptr->xpos > bigoX ) {
	    bigoX = netptr->xpos ;
	}
	if( netptr->ypos < litoY ) {
	    litoY = netptr->ypos ;
	} 
	if( netptr->ypos > bigoY ) {
	    bigoY = netptr->ypos ;
	}
    }
}
if( HorV ) {
    if( lArray[ nPinLocs ].finaly > lArray[1].finaly ) {
	if( litoY >= lArray[ nPinLocs ].finaly ) {
	    bestPos = nPinLocs ;
	} else if( bigoY <= lArray[1].finaly ) {
	    bestPos = 1 ;
	} else if( bigoY <= lArray[ nPinLocs ].finaly &&
				   litoY >= lArray[1].finaly ) {
		/*
		 *   bounding box of net (sans aPin) is
		 *   within span of site.  Find nearest loc
		 *   (in the site) to the center of b.b.
		 */
	    meany = (bigoY + litoY) / 2 ;
	    distant = ABS( meany - lArray[1].finaly ) ;
	    for( i = 2 ; i <= nPinLocs ; i++ ) {
		if( ABS( meany - lArray[i].finaly ) < distant ) {
		    distant = ABS(meany - lArray[i].finaly);
		} else {
		    break ;
		}
	    }
	    bestPos = i - 1 ;
	} else if( bigoY >= lArray[ nPinLocs ].finaly &&
				   litoY <= lArray[1].finaly ) {
		/*
		 *   Site is spanned (contained) by the b.b.
		 *   Select middle loc.
		 */
	    bestPos = (nPinLocs + 1) / 2 ;
	} else {  /*  only some overlapping exists  */
		/*
		 *   Find edge of b.b. inside site.
		 */
	    if( litoY < lArray[ nPinLocs ].finaly &&
				litoY > lArray[ 1 ].finaly ) {
		    /*
		     *   Find nearest loc to litoY
		     */
		distant = ABS( litoY - lArray[1].finaly ) ;
		for( i = 2 ; i <= nPinLocs ; i++ ) {
		    if( ABS( litoY - lArray[i].finaly ) < distant ){
			distant = ABS(litoY - lArray[i].finaly) ;
		    } else {
			break ;
		    }
		}
		bestPos = ( i + nPinLocs ) / 2 ;
	    } else {
		    /*
		     *   Find nearest loc to bigoY
		     */
		distant = ABS( bigoY - lArray[1].finaly ) ;
		for( i = 2 ; i <= nPinLocs ; i++ ) {
		    if( ABS( bigoY - lArray[i].finaly ) < distant ){
			distant = ABS(bigoY - lArray[i].finaly) ;
		    } else {
			break ;
		    }
		}
		bestPos = i / 2 ;
	    }
	}
    } else {
	if( litoY >= lArray[1].finaly ) {
	    bestPos = 1 ;
	} else if( bigoY <= lArray[ nPinLocs ].finaly ) {
	    bestPos = nPinLocs ;
	} else if( bigoY <= lArray[1].finaly &&
			     litoY >= lArray[ nPinLocs ].finaly ) {
		/*
		 *   bounding box of net (sans aPin) is
		 *   within span of site.  Find nearest loc
		 *   (in the site) to the center of b.b.
		 */
	    meany = (bigoY + litoY) / 2 ;
	    distant = ABS( meany - lArray[1].finaly ) ;
	    for( i = 2 ; i <= nPinLocs ; i++ ) {
		if( ABS( meany - lArray[i].finaly ) < distant ) {
		    distant = ABS(meany - lArray[i].finaly);
		} else {
		    break ;
		}
	    }
	    bestPos = i - 1 ;
	} else if( bigoY >= lArray[1].finaly &&
			      litoY <= lArray[ nPinLocs ].finaly ) {
		/*
		 *   Site is spanned (contained) by the b.b.
		 *   Select middle loc.
		 */
	    bestPos = (nPinLocs + 1) / 2 ;
	} else {  /*  only some overlapping exists  */
		/*
		 *   Find edge of b.b. inside site.
		 */
	    if( litoY < lArray[1].finaly &&
			    litoY > lArray[ nPinLocs ].finaly ) {
		    /*
		     *   Find nearest loc to litoY
		     */
		distant = ABS( litoY - lArray[1].finaly ) ;
		for( i = 2 ; i <= nPinLocs ; i++ ) {
		    if( ABS( litoY - lArray[i].finaly ) < distant ){
			distant = ABS(litoY - lArray[i].finaly) ;
		    } else {
			break ;
		    }
		}
		bestPos = i / 2 ;
	    } else {
		    /*
		     *   Find nearest loc to bigoY
		     */
		distant = ABS( bigoY - lArray[1].finaly ) ;
		for( i = 2 ; i <= nPinLocs ; i++ ) {
		    if( ABS( bigoY - lArray[i].finaly ) < distant ){
			distant = ABS(bigoY - lArray[i].finaly) ;
		    } else {
			break ;
		    }
		}
		bestPos = ( i + nPinLocs ) / 2 ;
	    }
	}
    }
} else {  /*  a horizontal site  */
    if( lArray[ nPinLocs ].finalx > lArray[1].finalx ) {
	if( litoX >= lArray[ nPinLocs ].finalx ) {
	    bestPos = nPinLocs ;
	} else if( bigoX <= lArray[1].finalx ) {
	    bestPos = 1 ;
	} else if( bigoX <= lArray[ nPinLocs ].finalx &&
				   litoX >= lArray[1].finalx ) {
		/*
		 *   bounding box of net (sans aPin) is
		 *   within span of site.  Find nearest loc
		 *   (in the site) to the center of b.b.
		 */
	    meanx = (bigoX + litoX) / 2 ;
	    distant = ABS( meanx - lArray[1].finalx ) ;
	    for( i = 2 ; i <= nPinLocs ; i++ ) {
		if( ABS( meanx - lArray[i].finalx ) < distant ) {
		    distant = ABS(meanx - lArray[i].finalx);
		} else {
		    break ;
		}
	    }
	    bestPos = i - 1 ;
	} else if( bigoX >= lArray[ nPinLocs ].finalx &&
				   litoX <= lArray[1].finalx ) {
		/*
		 *   Site is spanned (contained) by the b.b.
		 *   Select middle loc.
		 */
	    bestPos = (nPinLocs + 1) / 2 ;
	} else {  /*  only some overlapping exists  */
		/*
		 *   Find edge of b.b. inside site.
		 */
	    if( litoX < lArray[ nPinLocs ].finalx &&
				litoX > lArray[ 1 ].finalx ) {
		    /*
		     *   Find nearest loc to litoX
		     */
		distant = ABS( litoX - lArray[1].finalx ) ;
		for( i = 2 ; i <= nPinLocs ; i++ ) {
		    if( ABS( litoX - lArray[i].finalx ) < distant ){
			distant = ABS(litoX - lArray[i].finalx) ;
		    } else {
			break ;
		    }
		}
		bestPos = ( i + nPinLocs ) / 2 ;
	    } else {
		    /*
		     *   Find nearest loc to bigoX
		     */
		distant = ABS( bigoX - lArray[1].finalx ) ;
		for( i = 2 ; i <= nPinLocs ; i++ ) {
		    if( ABS( bigoX - lArray[i].finalx ) < distant ){
			distant = ABS(bigoX - lArray[i].finalx) ;
		    } else {
			break ;
		    }
		}
		bestPos = i / 2 ;
	    }
	}
    } else {
	if( litoX >= lArray[1].finalx ) {
	    bestPos = 1 ;
	} else if( bigoX <= lArray[ nPinLocs ].finalx ) {
	    bestPos = nPinLocs ;
	} else if( bigoX <= lArray[1].finalx &&
			     litoX >= lArray[ nPinLocs ].finalx ) {
		/*
		 *   bounding box of net (sans aPin) is
		 *   within span of site.  Find nearest loc
		 *   (in the site) to the center of b.b.
		 */
	    meanx = (bigoX + litoX) / 2 ;
	    distant = ABS( meanx - lArray[1].finalx ) ;
	    for( i = 2 ; i <= nPinLocs ; i++ ) {
		if( ABS( meanx - lArray[i].finalx ) < distant ) {
		    distant = ABS(meanx - lArray[i].finalx);
		} else {
		    break ;
		}
	    }
	    bestPos = i - 1 ;
	} else if( bigoX >= lArray[1].finalx &&
			      litoX <= lArray[ nPinLocs ].finalx ) {
		/*
		 *   Site is spanned (contained) by the b.b.
		 *   Select middle loc.
		 */
	    bestPos = (nPinLocs + 1) / 2 ;
	} else {  /*  only some overlapping exists  */
		/*
		 *   Find edge of b.b. inside site.
		 */
	    if( litoX < lArray[1].finalx &&
			    litoX > lArray[ nPinLocs ].finalx ) {
		    /*
		     *   Find nearest loc to litoX
		     */
		distant = ABS( litoX - lArray[1].finalx ) ;
		for( i = 2 ; i <= nPinLocs ; i++ ) {
		    if( ABS( litoX - lArray[i].finalx ) < distant ){
			distant = ABS(litoX - lArray[i].finalx) ;
		    } else {
			break ;
		    }
		}
		bestPos = i / 2 ;
	    } else {
		    /*
		     *   Find nearest loc to bigoX
		     */
		distant = ABS( bigoX - lArray[1].finalx ) ;
		for( i = 2 ; i <= nPinLocs ; i++ ) {
		    if( ABS( bigoX - lArray[i].finalx ) < distant ){
			distant = ABS(bigoX - lArray[i].finalx) ;
		    } else {
			break ;
		    }
		}
		bestPos = ( i + nPinLocs ) / 2 ;
	    }
	}
    }
}
return( bestPos ) ;
}  /* end of findLoc */
