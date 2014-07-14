#include "geo.h"

extern void tinsert( TNODEPTR *root , int value , int property );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property );

void reduceg(void)
{

int i , j , min , minc , WCcount , guideline , twoTimes ;
int c , count , c1 , c2 , hit , flag , k ;
int length , index1 , index2 , i1 , i2 , c2i1 , vertical ;
int xmin , xmax , ymin , ymax , end , start , edge ;
TNODEPTR node , Qroot ;
WCPTR ptr , *WCarray ;


Qroot = (TNODEPTR) NULL ;
WCarray = (WCPTR *) malloc( 101 * sizeof( WCPTR ) ) ;
WCcount = 0 ;

for( i = 1 ; i <= eNum ; i++ ) {
    eArray[i].notActive = 0 ;
}
for( i = 1 ; i <= numRects ; i++ ) {
    rectArray[i].ur = 0 ;
    rectArray[i].lr = 0 ;
    rectArray[i].ul = 0 ;
    rectArray[i].ll = 0 ;
}

for( i = 1 ; i <= numXnodes ; i++ ) {
    for( ptr = xNodeArray[i] ; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	if( ptr->fixedWidth == 1 ) {
	    continue ;
	}
	count = ptr->channels[0] ;
	
	WCcount++ ;
	if( WCcount % 100 == 0 ) {
	    WCarray = (WCPTR *) realloc( WCarray , (WCcount + 100) *
						sizeof( WCPTR ) ) ;
	}
	length = 0 ;
	for( c = 1 ; c <= count ; c++ ) {
	    length += eArray[ ptr->channels[c] ].length ;
	}
	length /= count ;
	WCarray[WCcount] = ptr ;
	tinsert( &Qroot , -length , WCcount ) ;

	index1 = eArray[ ptr->channels[1] ].index1 ;
	index2 = eArray[ ptr->channels[1] ].index2 ;
	rectArray[index2].ll = 1 ;
	rectArray[index1].ul = 1 ;

	index1 = eArray[ ptr->channels[count] ].index1 ;
	index2 = eArray[ ptr->channels[count] ].index2 ;
	rectArray[index2].lr = 1 ;
	rectArray[index1].ur = 1 ;
    }
}

for( i = 1 ; i <= numYnodes ; i++ ) {
    for( ptr = yNodeArray[i] ; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	if( ptr->fixedWidth == 1 ) {
	    continue ;
	}
	count = ptr->channels[0] ;

	WCcount++ ;
	if( WCcount % 100 == 0 ) {
	    WCarray = (WCPTR *) realloc( WCarray , (WCcount + 100) *
						sizeof( WCPTR ) ) ;
	}
	length = 0 ;
	for( c = 1 ; c <= count ; c++ ) {
	    length += eArray[ ptr->channels[c] ].length ;
	}
	length /= count ;
	WCarray[WCcount] = ptr ;
	tinsert( &Qroot , -length , WCcount ) ;

	index1 = eArray[ ptr->channels[1] ].index1 ;
	index2 = eArray[ ptr->channels[1] ].index2 ;
	rectArray[index2].ll = 1 ;
	rectArray[index1].lr = 1 ;

	index1 = eArray[ ptr->channels[count] ].index1 ;
	index2 = eArray[ ptr->channels[count] ].index2 ;
	rectArray[index2].ul = 1 ;
	rectArray[index1].ur = 1 ;
    }
}

for( ; ; ) {
    tpop( &Qroot , &node , &length , &WCcount ) ;
    if( node == (TNODEPTR) NULL ) {
	break ;
    }
    ptr = WCarray[WCcount] ;
    count = ptr->channels[0] ;
    if( count == 1 ) {
	continue ;
    }
    vertical = (ptr->channels[1] <= edgeTransition) ? 1 : 0 ;

    c1 = 1 ;
    c2 = 1 ;
    do {
	c2i1 = eArray[ ptr->channels[c2] ].index1 ; 
	if( rectArray[c2i1].ur == 0 ) {
	    c2++ ;
	    continue ;
	}
	guideline = (eArray[ ptr->channels[c2] ].rtside +
		     eArray[ ptr->channels[c1] ].lbside) / 2 ;
	min = 10000000 ;
	if( vertical ) {
	    ymin = 10000000 ;
	    ymax = 0 ;
	    for( c = c1 ; c <= c2 ; c++ ) {
		i1 = eArray[ ptr->channels[c] ].index1 ;
		i2 = eArray[ ptr->channels[c] ].index2 ;
		if( ABS( rectArray[i1].xc - guideline) < min ) {
		    min = ABS( rectArray[i1].xc - guideline) ;
		    minc = c ;
		}
		if( ABS( rectArray[i2].xc - guideline) < min ) {
		    min = ABS( rectArray[i2].xc - guideline) ;
		    minc = c ;
		}
	    }
	} else {
	    xmin = 10000000 ;
	    xmax = 0 ;
	    for( c = c1 ; c <= c2 ; c++ ) {
		i1 = eArray[ ptr->channels[c] ].index1 ;
		i2 = eArray[ ptr->channels[c] ].index2 ;
		if( ABS( rectArray[i1].yc - guideline) < min ) {
		    min = ABS( rectArray[i1].yc - guideline) ;
		    minc = c ;
		}
		if( ABS( rectArray[i2].yc - guideline) < min ) {
		    min = ABS( rectArray[i2].yc - guideline) ;
		    minc = c ;
		}
	    }
	}
	for( c = c1 ; c <= c2 ; c++ ) {
	    i1 = eArray[ ptr->channels[c] ].index1 ;
	    i2 = eArray[ ptr->channels[c] ].index2 ;
	    if( c != minc ) {
		eArray[ ptr->channels[c] ].notActive = 1 ;
	    } else {
		eArray[ ptr->channels[c] ].rtside = 
			    eArray[ ptr->channels[c2] ].rtside ;
		eArray[ ptr->channels[c] ].lbside = 
			    eArray[ ptr->channels[c1] ].lbside ;
	    }
	    if( c == c1 ) {
		if( vertical == 1 ) {
		    rectArray[i1].ur = 1 ;
		    rectArray[i2].lr = 1 ;
		} else {
		    rectArray[i1].ur = 1 ;
		    rectArray[i2].ul = 1 ;
		}
	    }
	    if( c == c2 ) {
		if( vertical == 1 ) {
		    rectArray[i1].ul = 1 ;
		    rectArray[i2].ll = 1 ;
		} else {
		    rectArray[i1].lr = 1 ;
		    rectArray[i2].ll = 1 ;
		}
	    }
	    if( c > c1 && c < c2 ) {
		if( vertical == 1 ) {
		    rectArray[i1].ul = 1 ;
		    rectArray[i1].ur = 1 ;
		    rectArray[i2].ll = 1 ;
		    rectArray[i2].lr = 1 ;
		} else {
		    rectArray[i1].ur = 1 ;
		    rectArray[i1].lr = 1 ;
		    rectArray[i2].ul = 1 ;
		    rectArray[i2].ll = 1 ;
		}
	    }
	}
	c1 = ++c2 ;
    } while( c2 <= count ) ;
}

for( i = 1 ; i <= numRects ; i++ ) {
    rectArray[i].nedges = 0 ;
}

for( i = 1 ; i <= numXnodes ; i++ ) {
    for( ptr = xNodeArray[i] ; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	if( ptr->fixedWidth == 1 ) {
	    continue ;
	}
	count = ptr->channels[0] ;
	for( c = 1 ; c <= count ; c++ ) {
	    if( eArray[ ptr->channels[c] ].notActive == 1 ) {
		continue ;
	    }
	    index1 = eArray[ ptr->channels[c] ].index1 ;
	    index2 = eArray[ ptr->channels[c] ].index2 ;
	    rectArray[index2].nedges++ ;
	    rectArray[index1].nedges++ ;
	}
    }
}

for( i = 1 ; i <= numYnodes ; i++ ) {
    for( ptr = yNodeArray[i] ; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	if( ptr->fixedWidth == 1 ) {
	    continue ;
	}
	count = ptr->channels[0] ;
	for( c = 1 ; c <= count ; c++ ) {
	    if( eArray[ ptr->channels[c] ].notActive == 1 ) {
		continue ;
	    }
	    index1 = eArray[ ptr->channels[c] ].index1 ;
	    index2 = eArray[ ptr->channels[c] ].index2 ;
	    rectArray[index2].nedges++ ;
	    rectArray[index1].nedges++ ;
	}
    }
}

hit = 1 ;
while( hit ) {
    hit = 0 ;
    for( i = 1 ; i <= numXnodes ; i++ ) {
	for( ptr = xNodeArray[i]; ptr != (WCPTR) NULL; ptr = ptr->next ){
	    if( ptr->fixedWidth == 1 ) {
		continue ;
	    }
	    count = ptr->channels[0] ;
	    for( c = 1 ; c <= count ; c++ ) {
		if( eArray[ ptr->channels[c] ].notActive == 1 ) {
		    continue ;
		}
		index1 = eArray[ ptr->channels[c] ].index1 ;
		index2 = eArray[ ptr->channels[c] ].index2 ;

		if( rectArray[index2].nedges == 1 ||
				    rectArray[index1].nedges == 1 ) {
		    eArray[ ptr->channels[c] ].notActive = 1 ;
		    rectArray[index2].nedges-- ;
		    rectArray[index1].nedges-- ;
		    hit = 1 ;
		    flag = 0 ;
		    for( k = 1 ; ; k++ ) {
			if( c + k <= count ) {
			    if( eArray[ ptr->channels[c + k] ].
						    notActive == 0) {
				    flag = 1 ;
				    eArray[ptr->channels[c + k]].lbside -=
					eArray[ ptr->channels[c] ].rtside -
					eArray[ ptr->channels[c] ].lbside ;
			    }
			}
			if( flag ) {
			    break ;
			}
			if( c - k >= 1 ) {
			    if( eArray[ ptr->channels[c - k] ].
						    notActive == 0){
				    flag = 1 ;
				    eArray[ptr->channels[c - k]].rtside +=
					eArray[ ptr->channels[c] ].rtside -
					eArray[ ptr->channels[c] ].lbside ;
			    }
			}
			if( flag ) {
			    break ;
			}
		    }
		}
	    }
	}
    }

    for( i = 1 ; i <= numYnodes ; i++ ) {
	for( ptr = yNodeArray[i]; ptr != (WCPTR) NULL; ptr = ptr->next ){
	    if( ptr->fixedWidth == 1 ) {
		continue ;
	    }
	    count = ptr->channels[0] ;
	    for( c = 1 ; c <= count ; c++ ) {
		if( eArray[ ptr->channels[c] ].notActive == 1 ) {
		    continue ;
		}
		index1 = eArray[ ptr->channels[c] ].index1 ;
		index2 = eArray[ ptr->channels[c] ].index2 ;
		if( rectArray[index2].nedges == 1 ||
				    rectArray[index1].nedges == 1 ) {
		    eArray[ ptr->channels[c] ].notActive = 1 ;
		    rectArray[index2].nedges-- ;
		    rectArray[index1].nedges-- ;
		    hit = 1 ;
		    flag = 0 ;
		    for( k = 1 ; ; k++ ) {
			if( c + k <= count ) {
			    if( eArray[ ptr->channels[c + k] ].
						    notActive == 0) {
				    flag = 1 ;
				    eArray[ptr->channels[c + k]].lbside -=
					eArray[ ptr->channels[c] ].rtside -
					eArray[ ptr->channels[c] ].lbside ;
			    }
			}
			if( flag ) {
			    break ;
			}
			if( c - k >= 1 ) {
			    if( eArray[ ptr->channels[c - k] ].
						    notActive == 0){
				    flag = 1 ;
				    eArray[ptr->channels[c - k]].rtside +=
					eArray[ ptr->channels[c] ].rtside -
					eArray[ ptr->channels[c] ].lbside ;
			    }
			}
			if( flag ) {
			    break ;
			}
		    }
		}
	    }
	}
    }
}


for( twoTimes = 1 ; twoTimes <= 2 ; twoTimes++ ) {
    for( i = 1 ; i <= numXnodes ; i++ ) {
	for( ptr = xNodeArray[i] ; ptr != (WCPTR) NULL ; 
						ptr = ptr->next ){
	    if( ptr->fixedWidth == 1 ) {
		continue ;
	    }
	    count = ptr->channels[0] ;
	    if( count <= 1 ) {
		continue ;
	    }
	    
	    if( eArray[ ptr->channels[1] ].notActive == 1 ) {
		i2 = eArray[ ptr->channels[1] ].index2 ;
		i1 = eArray[ ptr->channels[1] ].index1 ;
		edge = eArray[ ptr->channels[1] ].edge1 ;
		start = edgeList[edge].start ;
		end   = edgeList[edge].end ;
		ymax = rectArray[i2].yc ;
		ymin = rectArray[i1].yc ;
		for( c = 2 ; c <= count ; c++ ) {
		    if( eArray[ ptr->channels[c] ].notActive == 0 ) {
			i1 = eArray[ ptr->channels[c] ].index1 ;
			i2 = eArray[ ptr->channels[c] ].index2 ;
			if( rectArray[i2].yc < ymax ) {
			    j = eArray[ ptr->channels[1] ].index2 ;
			    if( rectArray[j].t < end &&
					rectArray[j].yreset == 0 ) {
				rectArray[j].yc = rectArray[i2].yc ;
				rectArray[j].yreset = 1 ;
			    } else if( rectArray[i2].yc < end ) { 
				rectArray[i2].yc = end ;
			    }
			}
			if( rectArray[i1].yc > ymin ) {
			    j = eArray[ ptr->channels[1] ].index1 ;
			    if( start < rectArray[j].b &&
					rectArray[j].yreset == 0 ) {
				rectArray[j].yc = rectArray[i1].yc ;
				rectArray[j].yreset = 1 ;
			    } else if( start < rectArray[i1].yc ) {
				rectArray[i1].yc = start ;
			    }
			}
			break ;
		    }
		}
	    }
	    if( eArray[ ptr->channels[count] ].notActive == 1 ) {
		i2 = eArray[ ptr->channels[count] ].index2 ;
		i1 = eArray[ ptr->channels[count] ].index1 ;
		edge = eArray[ ptr->channels[count] ].edge2 ;
		start = edgeList[edge].start ;
		end   = edgeList[edge].end ;
		ymax = rectArray[i2].yc ;
		ymin = rectArray[i1].yc ;
		for( c = count - 1 ; c >= 1 ; c-- ) {
		    if( eArray[ ptr->channels[c] ].notActive == 0 ) {
			i1 = eArray[ ptr->channels[c] ].index1 ;
			i2 = eArray[ ptr->channels[c] ].index2 ;
			if( rectArray[i2].yc < ymax ) {
			    j = eArray[ ptr->channels[count] ].index2 ;
			    if( rectArray[j].t < end &&
					rectArray[j].yreset == 0 ) {
				rectArray[j].yc = rectArray[i2].yc ;
				rectArray[j].yreset = 1 ;
			    } else if( rectArray[i2].yc < end ) { 
				rectArray[i2].yc = end ;
			    }
			}
			if( rectArray[i1].yc > ymin ) {
			    j = eArray[ ptr->channels[count] ].index1 ;
			    if( start < rectArray[j].b &&
					rectArray[j].yreset == 0 ) {
				rectArray[j].yc = rectArray[i1].yc ;
				rectArray[j].yreset = 1 ;
			    } else if( start < rectArray[i1].yc ) {
				rectArray[i1].yc = start ;
			    }
			}
			break ;
		    }
		}
	    }
	}
    }

    for( i = 1 ; i <= numYnodes ; i++ ) {
	for( ptr = yNodeArray[i] ; ptr != (WCPTR) NULL ; 
					    ptr = ptr->next ){
	    if( ptr->fixedWidth == 1 ) {
		continue ;
	    }
	    count = ptr->channels[0] ;
	    if( count <= 1 ) {
		continue ;
	    }

	    if( eArray[ ptr->channels[1] ].notActive == 1 ) {
		i2 = eArray[ ptr->channels[1] ].index2 ;
		i1 = eArray[ ptr->channels[1] ].index1 ;
		edge = eArray[ ptr->channels[1] ].edge1 ;
		start = edgeList[edge].start ;
		end   = edgeList[edge].end ;
		xmax = rectArray[i2].xc ;
		xmin = rectArray[i1].xc ;
		for( c = 2 ; c <= count ; c++ ) {
		    if( eArray[ ptr->channels[c] ].notActive == 0 ) {
			i1 = eArray[ ptr->channels[c] ].index1 ;
			i2 = eArray[ ptr->channels[c] ].index2 ;
			if( rectArray[i2].xc < xmax ) {
			    j = eArray[ ptr->channels[1] ].index2 ;
			    if( rectArray[j].r < end &&
					rectArray[j].xreset == 0 ) {
				rectArray[j].xc = rectArray[i2].xc ;
				rectArray[j].xreset = 1 ;
			    } else if( rectArray[i2].xc < end ) { 
				rectArray[i2].xc = end ;
			    }
			}
			if( rectArray[i1].xc > xmin ) {
			    j = eArray[ ptr->channels[1] ].index1 ;
			    if( start < rectArray[j].l &&
					rectArray[j].xreset == 0 ) {
				rectArray[j].xc = rectArray[i1].xc ;
				rectArray[j].xreset = 1 ;
			    } else if( start < rectArray[i1].xc ) {
				rectArray[i1].xc = start ;
			    }
			}
			break ;
		    }
		}
	    }
	    if( eArray[ ptr->channels[count] ].notActive == 1 ) {
		i2 = eArray[ ptr->channels[count] ].index2 ;
		i1 = eArray[ ptr->channels[count] ].index1 ;
		edge = eArray[ ptr->channels[count] ].edge2 ;
		start = edgeList[edge].start ;
		end   = edgeList[edge].end ;
		xmax = rectArray[i2].xc ;
		xmin = rectArray[i1].xc ;
		for( c = count - 1 ; c >= 1 ; c-- ) {
		    if( eArray[ ptr->channels[c] ].notActive == 0 ) {
			i1 = eArray[ ptr->channels[c] ].index1 ;
			i2 = eArray[ ptr->channels[c] ].index2 ;
			if( rectArray[i2].xc < xmax ) {
			    j = eArray[ ptr->channels[count] ].index2 ;
			    if( rectArray[j].r < end &&
					rectArray[j].xreset == 0 ) {
				rectArray[j].xc = rectArray[i2].xc ;
				rectArray[j].xreset = 1 ;
			    } else if( rectArray[i2].xc < end ) { 
				rectArray[i2].xc = end ;
			    }
			}
			if( rectArray[i1].xc > xmin ) {
			    j = eArray[ ptr->channels[count] ].index1 ;
			    if( start < rectArray[j].l &&
					rectArray[j].xreset == 0 ) {
				rectArray[j].xc = rectArray[i1].xc ;
				rectArray[j].xreset = 1 ;
			    } else if( start < rectArray[i1].xc ) {
				rectArray[i1].xc = start ;
			    }
			}
			break ;
		    }
		}
	    }
	}
    }
}

return ;
}
