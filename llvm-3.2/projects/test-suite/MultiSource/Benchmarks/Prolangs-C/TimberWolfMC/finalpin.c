#include "custom.h"
int HorV ;
int nPinLocs ;
UNCOMBOX *UCptr ;
typedef struct flogbox {
    int pin ;
    int placed ;
    int finalx ;
    int finaly ;
}
FBOX ,
*FBOXPTR ;
FBOXPTR lArray ;

extern int findLoc( int pin );

void finalpin(void)
{

CELLBOXPTR ptr ;
CONTENTBOX *SCptr ;
LOCBOX *SLptr ;
double aspFactor ;
int cell , i , j , k , site , span , trueSpan , pin1 , overlapping ;
int xprev , yprev , totalPin , loc , xnext , ynext , sum ;
int firstLoc , lastLoc , val , isoNum , loFill , hiFill , hit ;
int totLeft , first , second , secLoc , prefer , lastPin ;
int endSeqs[101] , begSeqs[101] , conSeqs[11][101] , isolated[201] ;
int nextS , nextUp ;


for( cell = 1 ; cell <= numcells ; cell++ ) {
    ptr = cellarray[ cell ] ;
    if( ptr->numUnComTerms == 0 ) {
	continue ;
    }
    SCptr = ptr->siteContent ;
    SLptr = ptr->config[ ptr->orient ]->siteLocArray ;
    UCptr = ptr->unComTerms ;
    for( i = 1 ; i <= ptr->numUnComTerms ; i++ ) {
	UCptr[i].finalx = -100000000 ;
	UCptr[i].finaly = -100000000 ;
    }
    for( site = 1 ; site <= ptr->numsites ; site++ ) {
	if( SCptr[ site ].contents <= 0 ) {
	    continue ;
	}
	/* 
	 *  Now we have a site with nonzero contents.
	 */
	span = SCptr[ site ].span ;
	HorV = SCptr[ site ].HorV ;
	if( ptr->orient <= 3 ) {
	    aspFactor = sqrt( ptr->aspect / ptr->aspectO ) ;
	} else {
	    aspFactor = sqrt( ptr->aspectO / ptr->aspect ) ;
	    if( HorV == 1 ) {
		HorV = 0 ;
	    } else {
		HorV = 1 ;
	    }
	}
	if( HorV ) {
	    trueSpan = (int) ( (double) span * aspFactor ) ;
	    if( (double) span - (double) trueSpan / aspFactor >
		(double) (trueSpan + 1) / aspFactor -
						(double) span ) {
		trueSpan++ ;
	    }
	} else {
	    trueSpan = (int) ( (double) span / aspFactor ) ;
	    if( (double) span - (double) trueSpan * aspFactor >
		(double) (trueSpan + 1) * aspFactor -
						(double) span ) {
		trueSpan++ ;
	    }
	}
	nPinLocs = trueSpan / pinSpacing ;
	if( trueSpan - nPinLocs * pinSpacing > (nPinLocs + 1) *
				    pinSpacing - trueSpan ) {
	    nPinLocs++ ;
	}
	/*  
	 *   prepare the location array for the pins  
	 */
	lArray = (FBOXPTR) malloc( (nPinLocs + 1) * sizeof( FBOX ));
	for( i = 1 ; i <= nPinLocs ; i++ ) {
	    lArray[ i ].placed = 0 ;
	    lArray[ i ].pin = 0 ;
	    lArray[ i ].finalx = 0 ;
	    lArray[ i ].finaly = 0 ;
	}
	/*
	 *  center of trueSpan is at:
	 *  SLptr[ site ].xpos  and  SLptr[ site ].ypos
	 */
	if( HorV ) {      /*  a vertical trueSpan  */
	    if( site == ptr->numsites ) {
		nextS = 1 ;
	    } else {
		nextS = site + 1 ;
	    }
	    if( SLptr[nextS].ypos > SLptr[site].ypos ) {
		nextUp = 1 ;
	    } else {
		nextUp = 0 ;
	    }
	    for( i = 1 ; i <= nPinLocs ; i++ ) {
		lArray[ i ].finalx = SLptr[ site ].xpos ;
		if( nextUp == 0 ) {
		    lArray[ i ].finaly = SLptr[ site ].ypos - 
			trueSpan / 2 + (i - 1) * pinSpacing ;
		} else {
		    lArray[ i ].finaly = SLptr[ site ].ypos - 
			(trueSpan + 1) / 2 + i * pinSpacing ;
		}
	    }
	} else {       /* a horizontal trueSpan  */
	    if( site == ptr->numsites ) {
		nextS = 1 ;
	    } else {
		nextS = site + 1 ;
	    }
	    if( SLptr[nextS].xpos > SLptr[site].xpos ) {
		nextUp = 1 ;
	    } else {
		nextUp = 0 ;
	    }
	    for( i = 1 ; i <= nPinLocs ; i++ ) {
		lArray[ i ].finaly = SLptr[ site ].ypos ;
		if( nextUp == 0 ) {
		    lArray[ i ].finalx = SLptr[ site ].xpos - 
			    trueSpan / 2 + (i - 1) * pinSpacing ;
		} else {
		    lArray[ i ].finalx = SLptr[ site ].xpos - 
			(trueSpan + 1) / 2 + i * pinSpacing ;
		}
	    }
	}

	/*  initialize the storage arrays  */
	for( j = 0 ; j <= 100 ; j++ ) {
	    endSeqs[ j ] = 0 ;
	    begSeqs[ j ] = 0 ;
	    for( i = 0 ; i <= 10 ; i++ ) {
		conSeqs[ i ][ j ] = 0 ;
	    }
	}
	for( j = 0 ; j <= 200 ; j++ ) {
	    isolated[ j ] = 0 ;
	}

	/*  Now let's look at the pins belonging to this site */

	for( pin1 = 1 ; pin1 <= ptr->numUnComTerms ; pin1++ ) {
	    if( UCptr[ pin1 ].site == site ) {
		/*
		 *  We have found a pin in this site
		 */
		if( UCptr[ pin1 ].sequence == 0 ) {
		    /*
		     *  This pin is the continuation of a
		     *  sequence started in a previous site.
		     *  Look at the previous pin to see
		     *  what its site is.
		     */
		    if( endSeqs[0] == 0 ) {
			xprev = SLptr[ UCptr[pin1 - 1].site ].xpos ;
			yprev = SLptr[ UCptr[pin1 - 1].site ].ypos ;
			if( ABS( lArray[ 1 ].finalx - xprev ) +
			    ABS( lArray[ 1 ].finaly - yprev )  <
			    ABS( lArray[nPinLocs].finalx - xprev ) +
			    ABS( lArray[nPinLocs].finaly - yprev )){
			    /*
			     *  Then the previous site is nearer
			     *  pin location number 1. So place this
			     *  pin in location number 1.
			     */
			    endSeqs[0] = 1 ;
			    endSeqs[1] = 1 ;
			    endSeqs[2] = pin1 ;
			} else {
			    endSeqs[0] = 1 ;
			    endSeqs[1] = nPinLocs ;
			    endSeqs[2] = pin1 ;
			}
			for(++pin1; pin1 <= ptr->numUnComTerms; 
						    pin1++){
			    if( UCptr[ pin1 ].sequence != 0 ||
				UCptr[ pin1 ].site != site ) {
				break ;
			    }
			    endSeqs[ ++endSeqs[0] + 1 ] = pin1 ;
			}
		    } else {
			fprintf(fpo,"OOPs: a endSeqs ");
			fprintf(fpo,"cannot fit in the");
			fprintf(fpo," site:%d\n", site );
			fprintf(fpo,"TimberWolf has ");
			fprintf(fpo,"to leave it out\n");
			fprintf(fpo,"Current cell: %d\n", cell ) ;

			for(++pin1; pin1 <= ptr->numUnComTerms; 
							pin1++){
			    if( UCptr[ pin1 ].sequence != 0 ||
				UCptr[ pin1 ].site != site ) {
				break ;
			    }
			}
		    }
		    pin1-- ;
		} else if( UCptr[ pin1 ].sequence > 1 ) {
		    conSeqs[ ++conSeqs[0][0] ][0] = 1 ;
		    conSeqs[ conSeqs[0][0] ][2] = pin1 ;
		    for(++pin1; pin1 <= ptr->numUnComTerms; pin1++){
			if( UCptr[ pin1 ].sequence != 0 ||
				      UCptr[ pin1 ].site != site ) {
			    if( UCptr[ pin1 ].sequence == 0 ) {
				/*
				 *  Lo and behold, a begSeqs here.
				 *  Transfer this entry in conSeqs
				 *  to begSeqs and zero out the
				 *  latest conSeqs entry.
				 */
				i = conSeqs[ conSeqs[0][0] ][0] + 1;
				if( begSeqs[0] == 0 ) {
				    for( ; i >= 0 ; i-- ) {
					begSeqs[ i ] = conSeqs[
					      conSeqs[0][0] ][ i ] ;
					conSeqs[ conSeqs[0][0] ][i] 
							       = 0 ;
				    }
				} else {
				    fprintf(fpo,"OOPs: a begSeqs ");
				    fprintf(fpo,"cannot fit in ");
				    fprintf(fpo," site:%d\n", site);
				    fprintf(fpo,"TimberWolf has t");
				    fprintf(fpo,"o leave it out\n");
				    fprintf(fpo,"Current cell:%d\n",
							    cell ) ;
				    for( ; i >= 0 ; i-- ) {
					conSeqs[ conSeqs[0][0] ][i] 
							       = 0 ;
				    }
				}
				conSeqs[0][0]-- ;
			    }
			    break ;
			}
			conSeqs[ conSeqs[0][0] ]
			       [ ++conSeqs[ conSeqs[0][0] ][0] + 1 ]
			       = pin1 ;
		    }
		    pin1-- ;
		} else {  /* sequence # was numero uno  */
		    isolated[ 2 * ++isolated[0] ] = pin1 ;
		}
	    }
	}
	/*
	 *   At this point, all the pins for a given site
	 *   are in the "arrays" endSeqs, begSeqs, conSeqs,
	 *   and isolated.  The next thing to do is to sort
	 *   all this info out.
	 */
	/*
	 *   First of all, do the ending sequences --- since we
	 *   already know which end of the site they are
	 *   coming from.  Of course, if we have any such.
	 */
	totalPin = endSeqs[0] ;
	if( totalPin > nPinLocs ) {
	    fprintf(fpo,"OOPs: an endSeqs cannot fit in site:%d\n",
							  site ) ;
	    fprintf(fpo,"TimberWolf will have to leave it out\n") ;
	    fprintf(fpo,"Current cell: %d\n", cell ) ;
	}
	if( endSeqs[0] > 0 && totalPin <= nPinLocs ) {
	    if( endSeqs[1] == 1 ) {
		lArray[1].pin = endSeqs[2] ;
		lArray[1].placed = 1 ;
		loc = 1 ;
		for( i = 2 ; i <= endSeqs[0] ; i++ ) {
		    lArray[ ++loc ].pin = endSeqs[ i + 1 ] ;
		    lArray[ loc ].placed = 1 ;
		}
	    } else {
		lArray[ nPinLocs ].pin = endSeqs[2] ;
		lArray[ nPinLocs ].placed = 1 ;
		loc = nPinLocs ;
		for( i = 2 ; i <= endSeqs[0] ; i++ ) {
		    lArray[ --loc ].pin = endSeqs[ i + 1 ] ;
		    lArray[ loc ].placed = 1 ;
		}
	    }
	}
	/*
	 *  Now we have to deal with beginning sequences
	 *  which begin in this site and carry on to the
	 *  next site.
	 */
	if( begSeqs[0] > 0 && totalPin + begSeqs[0] <= nPinLocs ) {
	    totalPin += begSeqs[0] ;
	    lastPin = begSeqs[ begSeqs[0] + 1 ] ;
	    /*
	     *
	     *  Look at the next pin to see
	     *  what its site is.
	     */
	    xnext = SLptr[ UCptr[ lastPin + 1 ].site ].xpos ;
	    ynext = SLptr[ UCptr[ lastPin + 1 ].site ].ypos ;
	    if( ABS( lArray[ 1 ].finalx - xnext ) +
		ABS( lArray[ 1 ].finaly - ynext )  <
		ABS( lArray[ nPinLocs ].finalx - xnext ) +
		ABS( lArray[ nPinLocs ].finaly - ynext )) {
		/*
		 *  Then the next site is nearer
		 *  pin location number 1.  So place this
		 *  pin in location number 1.
		 */
		lArray[1].pin = begSeqs[ begSeqs[0] + 1 ] ;
		lArray[1].placed = 1 ;
		loc = 1 ;
		for( i = begSeqs[0] - 1 ; i >= 1 ; i-- ) {
		    lArray[ ++loc ].pin = begSeqs[ i + 1 ] ;
		    lArray[ loc ].placed = 1 ;
		}
	    } else {
		lArray[ nPinLocs ].pin = begSeqs[ begSeqs[0] + 1 ] ;
		lArray[ nPinLocs ].placed = 1 ;
		loc = nPinLocs ;
		for( i = begSeqs[0] - 1 ; i >= 1 ; i-- ) {
		    lArray[ --loc ].pin = begSeqs[ i + 1 ] ;
		    lArray[ loc ].placed = 1 ;
		}
	    }
	} else if( totalPin + begSeqs[0] > nPinLocs ) {
	    fprintf(fpo,"OOPs: a begSeqs cannot fit in site:%d\n",
							  site ) ;
	    fprintf(fpo,"TimberWolf will have to leave it out\n") ;
	    fprintf(fpo,"Current cell: %d\n", cell ) ;
	} 
	/*
	 *   Now we have a bit more work to do.  We have
	 *   to examine the contained sequences and the
	 *   isolated pins.  For each pin, we have to find
	 *   its most desirable position in the site
	 *   from location 1 thru nPinLocs.
	 */
	for( i = 1 ; i <= conSeqs[0][0] ; i++ ) {
	    if( totalPin + conSeqs[ i ][0] > nPinLocs ) {
		fprintf(fpo,"OOPs: TimberWolf has to ignore a \n");
		fprintf(fpo,"contained sequence in site:%d\n",site);
		fprintf(fpo,"Current cell: %d\n", cell ) ;
		continue ;
	    } else {
		totalPin += conSeqs[ i ][0] ;
	    }
	    sum = 0 ;
	    for( j = 1 ; j <= conSeqs[ i ][0] ; j++ ) {
		if( j == 1 ) {
		    firstLoc = findLoc( conSeqs[ i ][ j + 1 ] ) ;
		    sum += firstLoc ;
		} else if( j == conSeqs[ i ][0] ) {
		    lastLoc = findLoc( conSeqs[ i ][ j + 1 ] ) ;
		    sum += lastLoc ;
		} else {
		    sum += findLoc( conSeqs[ i ][ j + 1 ] ) ;
		}
	    }
	    /*
	     *  The average best place to go for this sequence
	     *  is sum divided by conSeqs[ i ][0].
	     */
	    val = sum / conSeqs[ i ][0] - conSeqs[ i ][0] / 2 ; 
	    if( val < 1 ) {
		val = 1 ;
	    } else if( val > nPinLocs - conSeqs[ i ][0] + 1 ) {
		val = nPinLocs - conSeqs[ i ][0] + 1 ;
	    }
	    if( firstLoc > lastLoc ) {
		conSeqs[ i ][1] = -val ;
	    } else {
		conSeqs[ i ][1] = val ;
	    }
	}
	/*
	 *   And now its finally time to deal with the isolated
	 *   pins.  First, let's see if we can deal with all
	 *   of them.
	 */
       
	if( totalPin + isolated[0] > nPinLocs ) {
	    isoNum = nPinLocs - totalPin ;
	    fprintf(fpo,"OOPs: isolated pin(s) cannot fit in ");
	    fprintf(fpo,"the site: %d\n", site ) ;
	    fprintf(fpo,"TimberWolf will have to ignore this ");
	    fprintf(fpo,"number of pins: %d\n", 
				totalPin + isolated[0] - nPinLocs );
	    fprintf(fpo,"Current cell: %d\n", cell ) ;
	} else {
	    isoNum = isolated[0] ;
	}
	/*
	 *   Here we go
	 */
	for( i = 1 ; i <= isoNum ; i++ ) {
	    isolated[ 2 * i - 1 ] = findLoc( isolated[ 2 * i ] ) ;
	}

	/*
	 *   And ... Now ... Here's ...... the Pin Placer
	 */
	
	/*
	 *   One would surmise that since the beginning and
	 *   ending sequences are already placed , then the
	 *   next thing to do is to place the contained 
	 *   sequences.  Finally, of course, the isolated pins.
	 */
	/*
	 *   The basic method will be to establish two
	 *   endpoints ( say, "left" and "right", if HorV is 0 )
	 *   such that all locs to the left of "left" are
	 *   filled and all locs to the right of "right" are
	 *   also filled.  Then, any conSeqs wanting to start
	 *   to the left of "left" will be packed tight to
	 *   "left" and "left" will be updated accordingly.
	 *   Similarly, all conSeqs wanting to extend beyond
	 *   right will be packed tight to "right" and "right"
	 *   will be updated.
	 */
	loFill = 0 ;
	for( i = 1 ; i <= nPinLocs ; i++ ) {
	    if( lArray[ i ].placed == 1 ) {
		loFill = i ;
	    } else {
		break ;
	    }
	}
	hiFill = nPinLocs + 1 ;
	for( i = nPinLocs ; i >= 1 ; i-- ) {
	    if( lArray[ i ].placed == 1 ) {
		hiFill = i ;
	    } else {
		break ;
	    }
	}

	hit = 1 ;
	while( hit == 1 ) {
	    hit = 0 ;
	    for( i = 1 ; i <= conSeqs[0][0] ; i++ ) {
		if( conSeqs[i][1] == 0 ) {
		    continue ;
		}
		if( ABS( conSeqs[i][1] ) <= loFill + 1 ) {
		    /*
		     *  Place sequence i starting from loFill + 1
		     */
		    if( conSeqs[i][1] > 0 ) {
			for( j = 1 ; j <= conSeqs[i][0] ; j++ ) {
			    lArray[ loFill + j ].pin = 
						  conSeqs[i][j+1] ;
			    lArray[ loFill + j ].placed = 1 ;
			}
		    } else {
			for( j = 1 , k = conSeqs[i][0] ; 
				   j <= conSeqs[i][0] ; j++ , k--) {
			    lArray[ loFill + j ].pin = 
						  conSeqs[i][k+1] ;
			    lArray[ loFill + j ].placed = 1 ;
			}
		    }
		    loFill += conSeqs[i][0] ;
		    conSeqs[i][1] = 0 ;
		    hit = 1 ;
		} else if( ABS( conSeqs[i][1] ) + 
				    conSeqs[i][0] >= hiFill ) {
		    /*
		     *  Place sequence i starting from 
		     *  hiFill - conSeqs[i][0]
		     */
		    if( conSeqs[i][1] > 0 ) {
			for( j = hiFill - conSeqs[i][0] , k = 1 ; 
				    j < hiFill ; j++ , k++ ) {
			    lArray[ j ].pin = conSeqs[i][k+1] ;
			    lArray[ j ].placed = 1 ;
			}
		    } else {
			for( j = hiFill - conSeqs[i][0] , 
				    k = conSeqs[i][0] ; 
				    j < hiFill ; j++ , k-- ) {
			    lArray[ j ].pin = conSeqs[i][k+1] ;
			    lArray[ j ].placed = 1 ;
			}
		    }
		    hiFill -= conSeqs[i][0] ;
		    conSeqs[i][1] = 0 ;
		    hit = 1 ;
		}
	    }
	}
	/*
	 *   Ok, that was the easy placement.  Now we have to
	 *   deal with any other conSeqs that may be left
	 *   which apparently want to sit in the middle of
	 *   the site.
	 */
	/*
	 *   Now select the lowest-most remaining sequence.
	 *   Put it in its bestPos if the other remaining
	 *   sequences can fit in the remaining space above.
	 *   Otherwise, back it off as necessary.
	 */
	totLeft = 0 ;
	for( i = 1 ; i <= conSeqs[0][0] ; i++ ) {
	    if( conSeqs[i][1] == 0 ) {
		continue ;
	    }
	    totLeft += conSeqs[i][0] ;
	}

	while( totLeft > 0 ) {
	    first = 0 ;
	    second = 0 ;
	    firstLoc = nPinLocs + 1 ;
	    secLoc = nPinLocs + 1 ;
	    for( i = 1 ; i <= conSeqs[0][0] ; i++ ) {
		if( conSeqs[i][1] == 0 ) {
		    continue ;
		}
		if( ABS( conSeqs[i][1] ) < firstLoc ) {
		    if( first != 0 ) {
			second = first ;
			secLoc = firstLoc ;
		    }
		    firstLoc = ABS( conSeqs[i][1] ) ;
		    first = i ;
		} else if( ABS( conSeqs[i][1] ) < secLoc ) {
		    secLoc = ABS( conSeqs[i][1] ) ;
		    second = i ;
		}
	    }
	    while( firstLoc + totLeft > hiFill ) {
		    /*
		     *   Insufficient room if we start at firstLoc
		     */
		firstLoc-- ;
	    }
		/*
		 *   If secLoc intersects firstLoc to firstLoc +
		 *   ( length of firstseq - 1 ) then back off
		 *   firstLoc ( if possible ) by an amount
		 *   equal to 1/2 of the amount of overlapping
		 */
	    if( secLoc < firstLoc + conSeqs[ first ][0] - 1 ) {
		overlapping = firstLoc + conSeqs[ first ][0] - 1 -
							secLoc ;
		while( (firstLoc + 
				conSeqs[ first ][0] - 1 - secLoc) > 
						    overlapping / 2 ) {
		    if( firstLoc - 1 > loFill ) {
			firstLoc-- ;
		    } else {
			break ;
		    }
		}
		    /*
		     *  We want to place both of these sequences
		     */
		if( conSeqs[ first ][1] > 0 ) {
		    for( j = 1 ; j <= conSeqs[ first ][0] ; j++ ) {
			lArray[ firstLoc - 1 + j ].pin = 
					     conSeqs[ first ][j+1] ;
			lArray[ firstLoc - 1 + j ].placed = 1 ;
		    }
		} else {
		    for( j = 1 , k = conSeqs[ first ][0] ; 
			   j <= conSeqs[ first ][0] ; j++ , k--) {
			lArray[ firstLoc - 1 + j ].pin = 
					     conSeqs[ first ][k+1] ;
			lArray[ firstLoc - 1 + j ].placed = 1 ;
		    }
		}
		loFill = firstLoc + conSeqs[ first ][0] - 1 ;
		conSeqs[ first ][1] = 0 ;
		totLeft -= conSeqs[ first ][0] ;

		if( conSeqs[ second ][1] > 0 ) {
		    for( j = 1 ; j <= conSeqs[ second ][0] ; j++ ) {
			lArray[ loFill + j ].pin = 
				     conSeqs[ second ][j+1] ;
			lArray[ loFill + j ].placed = 1 ;
		    }
		} else {
		    for( j = 1 , k = conSeqs[ second ][0] ; 
			   j <= conSeqs[ second ][0] ; j++ , k--) {
			lArray[ loFill + j ].pin = 
				     conSeqs[ second ][k+1] ;
			lArray[ loFill + j ].placed = 1 ;
		    }
		}
		loFill = loFill + conSeqs[ second ][0] ;
		conSeqs[ second ][1] = 0 ;
		totLeft -= conSeqs[ second ][0] ;
	    } else {  /*  place only the "first" sequence  */
		if( conSeqs[ first ][1] > 0 ) {
		    for( j = 1 ; j <= conSeqs[ first ][0] ; j++ ) {
			lArray[ firstLoc - 1 + j ].pin = 
					     conSeqs[ first ][j+1] ;
			lArray[ firstLoc - 1 + j ].placed = 1 ;
		    }
		} else {
		    for( j = 1 , k = conSeqs[ first ][0] ; 
			   j <= conSeqs[ first ][0] ; j++ , k--) {
			lArray[ firstLoc - 1 + j ].pin = 
					     conSeqs[ first ][k+1] ;
			lArray[ firstLoc - 1 + j ].placed = 1 ;
		    }
		}
		loFill = firstLoc + conSeqs[ first ][0] - 1 ;
		conSeqs[ first ][1] = 0 ;
		totLeft -= conSeqs[ first ][0] ;
	    }
	}

	/*
	 *   Whew!!!!  Finally to the isolated pins!
	 */
	/*
	 *  For the first pass, any pins which can be
	 *  placed exactly in their most desirable places
	 *  are so placed.
	 */
	for( i = 1 ; i <= isoNum ; i++ ) {
	    if( lArray[ isolated[ 2 * i - 1 ] ].placed == 0 ) {
		lArray[ isolated[ 2 * i - 1 ] ].placed = 1 ;
		lArray[ isolated[ 2 * i - 1 ] ].pin =
						 isolated[ 2 * i ] ;
		isolated[ 2 * i - 1 ] = 0 ;
	    }
	}
	/*
	 *  Now comes the trickier stuff for those pins which
	 *  cannot go exactly where they would like to go
	 */
	for( i = 1 ; i <= isoNum ; i++ ) {
	    prefer = isolated[ 2 * i - 1 ] ;
	    if( prefer == 0 ) {
		continue ;
	    }
	    for( k = 1 ; k < nPinLocs ; k++ ) {
		if( prefer + k <= nPinLocs ) {
		    if( lArray[ prefer + k ].placed == 0 ) {
			lArray[ prefer + k ].placed = 1 ;
			lArray[ prefer + k ].pin =
						 isolated[ 2 * i ] ;
			isolated[ 2 * i - 1 ] = 0 ;
			break ;
		    }
		}
		if( prefer - k >= 1 ) {
		    if( lArray[ prefer - k ].placed == 0 ) {
			lArray[ prefer - k ].placed = 1 ;
			lArray[ prefer - k ].pin =
						 isolated[ 2 * i ] ;
			isolated[ 2 * i - 1 ] = 0 ;
			break ;
		    }
		}
	    }
	}
	/*
	 *  Believe it or not, all the pins for this site
	 *  have been placed.  Now all we have to do is
	 *  put the placement information into the array
	 *  pointed to by UCptr.
	 */
	for( i = 1 ; i <= nPinLocs ; i++ ) {
	    if( lArray[ i ].placed == 1 ) {
	       UCptr[ lArray[ i ].pin ].finalx = lArray[ i ].finalx;
	       UCptr[ lArray[ i ].pin ].finaly = lArray[ i ].finaly;
	    }
	}
	/*
	 *   And that's it folks, for this particular site.
	 *   Prep for the next one.
	 */
	free( lArray ) ;
    }
    for( i = 1 ; i <= ptr->numUnComTerms ; i++ ) {
	if( UCptr[i].finalx == -100000000  && 
				UCptr[i].finaly == -100000000 ) {
	    /* 
	     *  If at first you don't succeed, take a wild guess
	     */
	    UCptr[i].finalx = SLptr[ UCptr[i].site ].xpos ;
	    UCptr[i].finaly = SLptr[ UCptr[i].site ].ypos ;
	}
    }
}
return ;
}
