#include "custom.h"
#define L 1
#define T 2
#define R 3
#define B 4
extern double *padspace ;
extern int *fixLRBT ;

void placepads(void)
{

int coreHeight , coreWidth , zxshift , zyshift , pad , count ;
int height , width , maxHeight , maxWidth , space , separation ;
int extraSpace , last , xshift ;

coreHeight = blockt ;
coreWidth  = blockr ;
blockb  = 0 ;
blockl  = 0 ;
zxshift = 0 ;
zyshift = 0 ;
xshift  = 0 ;
for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    if( cellarray[pad]->padside == B ) {
	count = 1 ;
	height = cellarray[pad]->config[0]->top -
				 cellarray[pad]->config[0]->bottom ;
	width = cellarray[pad]->config[0]->right -
				 cellarray[pad]->config[0]->left ;
	for( pad++ ; pad <= numcells + numpads ; pad++ ) {
	    if( cellarray[pad]->padside != B ) {
		break ;
	    }
	    count++ ;
	    if( cellarray[pad]->config[0]->top -
			cellarray[pad]->config[0]->bottom > height ) {
		height = cellarray[pad]->config[0]->top
				   - cellarray[pad]->config[0]->bottom ;
	    }
	    width += cellarray[pad]->config[0]->right -
				      cellarray[pad]->config[0]->left ;
	}
	maxHeight = height ;
	if( fixLRBT[2] == 0 ) {
	    space = coreWidth - width ;
	    separation = space / (count + 1) ;
	    if( separation < 0 ) {
		separation = 0 ;
	    }
	} else {
	    space = ( coreWidth >= width ) ? coreWidth : width ;
	}
	if( width > coreWidth ) {
	    zxshift = (width - coreWidth) / 2 ;
	    coreWidth = width ;
	}
	extraSpace = 0.1 * (blockt - blockb) ;
	blockb += maxHeight + extraSpace ;
	blockt += maxHeight + extraSpace ;

	for( pad = numcells + 1; pad <= numcells + numpads; pad++){
	    if( cellarray[pad]->padside == B ) {
		height = cellarray[pad]->config[0]->top -
				     cellarray[pad]->config[0]->bottom ;
		width =  cellarray[pad]->config[0]->right -
				     cellarray[pad]->config[0]->left ;
		if( fixLRBT[2] == 0 ) {
		    cellarray[pad]->xcenter = separation + width / 2 ;
		} else {
		    cellarray[pad]->xcenter = padspace[pad - numcells]
							    * space ;
		}
		cellarray[pad]->ycenter = maxHeight - ( height - 
							height / 2 ) ;
		last = separation + width ;

		for( pad++ ; pad <= numcells + numpads ; pad++ ) {
		    if( cellarray[pad]->padside != B ) {
			break ;
		    }
		    height = cellarray[pad]->config[0]->top -
			     cellarray[pad]->config[0]->bottom ;
		    width =  cellarray[pad]->config[0]->right -
			     cellarray[pad]->config[0]->left ;
		    if( fixLRBT[2] == 0 ) {
			cellarray[pad]->xcenter = last + separation + 
						    width / 2 ;
		    } else {
			cellarray[pad]->xcenter = 
				    padspace[ pad - numcells ] * space ;
		    }
		    cellarray[pad]->ycenter = maxHeight - ( height - 
							height / 2 ) ;
		    last += separation + width ;
		}
	    }
	}
    }
}

for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    if( cellarray[pad]->padside == T ) {
	count = 1 ;
	width = cellarray[pad]->config[0]->right -
				      cellarray[pad]->config[0]->left ;
	for( pad++ ; pad <= numcells + numpads ; pad++ ) {
	    if( cellarray[pad]->padside != T ) {
		break ;
	    }
	    count++ ;
	    width += cellarray[pad]->config[0]->right -
				      cellarray[pad]->config[0]->left ;
	}
	if( fixLRBT[3] == 0 ) {
	    space = coreWidth - width ;
	    separation = space / (count + 1) ;
	    if( separation < 0 ) {
		separation = 0 ;
	    }
	} else {
	    space = (coreWidth >= width ) ? coreWidth : width ;
	}
	if( width > coreWidth ) {
	    zxshift += (width - coreWidth) / 2 ;
	}

	for( pad = numcells + 1; pad <= numcells + numpads; pad++){
	    if( cellarray[pad]->padside == T ) {
		height = cellarray[pad]->config[0]->top -
			 cellarray[pad]->config[0]->bottom ;
		width =  cellarray[pad]->config[0]->right -
			 cellarray[pad]->config[0]->left ;
		if( fixLRBT[3] == 0 ) {
		    cellarray[pad]->xcenter = separation + width / 2 ;
		} else {
		    cellarray[pad]->xcenter = padspace[pad - numcells]
							      * space ;
		}
		cellarray[pad]->ycenter = blockt + 0.1 * 
					(blockt - blockb) + height / 2 ;
		last = separation + width ;

		for( pad++ ; pad <= numcells + numpads ; pad++ ) {
		    if( cellarray[pad]->padside != T ) {
			break ;
		    }
		    height = cellarray[pad]->config[0]->top -
			     cellarray[pad]->config[0]->bottom ;
		    width =  cellarray[pad]->config[0]->right -
			     cellarray[pad]->config[0]->left ;
		    if( fixLRBT[3] == 0 ) {
			cellarray[pad]->xcenter = last + separation + 
							    width / 2 ;
		    } else {
			cellarray[pad]->xcenter = 
				    padspace[ pad - numcells ] * space ;
		    }
		    cellarray[pad]->ycenter = blockt + 0.1 * 
					(blockt - blockb) + height / 2 ;
		    last += separation + width ;
		}
	    }
	}
    }
}

for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    if( cellarray[pad]->padside == L ) {
	count = 1 ;
	height = cellarray[pad]->config[0]->top -
				     cellarray[pad]->config[0]->bottom ;
	width = cellarray[pad]->config[0]->right -
				      cellarray[pad]->config[0]->left ;
	for( pad++ ; pad <= numcells + numpads ; pad++ ) {
	    if( cellarray[pad]->padside != L ) {
		break ;
	    }
	    count++ ;
	    if( cellarray[pad]->config[0]->right -
			    cellarray[pad]->config[0]->left > width ) {
		width = cellarray[pad]->config[0]->right -
				     cellarray[pad]->config[0]->left ;
	    }
	    height += cellarray[pad]->config[0]->top -
				  cellarray[pad]->config[0]->bottom ;
	}
	maxWidth = width ;
	if( fixLRBT[0] == 0 ) {
	    space = coreHeight - height ;
	    separation = space / (count + 1) ;
	    if( separation < 0 ) {
		separation = 0 ;
	    }
	} else {
	    space = (coreHeight >= height ) ? coreHeight : height ;
	}
	if( height > coreHeight ) {
	    zyshift = (height - coreHeight) / 2 ;
	    coreHeight = height ;
	}
	extraSpace = 0.1 * (blockr - blockl) ;
	blockr += maxWidth + extraSpace ;
	blockl += maxWidth + extraSpace ;
	xshift =  maxWidth + extraSpace ;

	for( pad = numcells + 1; pad <= numcells + numpads; pad++){
	    if( cellarray[pad]->padside == L ) {
		height = cellarray[pad]->config[0]->top -
				 cellarray[pad]->config[0]->bottom ;
		width =  cellarray[pad]->config[0]->right -
				 cellarray[pad]->config[0]->left ;
		if( fixLRBT[0] == 0 ) {
		    cellarray[pad]->ycenter = blockb + separation + 
							height / 2 ;
		} else {
		    cellarray[pad]->ycenter = blockb +
				padspace[ pad - numcells ] * space ;
		}
		cellarray[pad]->xcenter = maxWidth - 
						(width - width / 2) ;
		last = blockb + separation + height ;

		for( pad++ ; pad <= numcells + numpads ; pad++ ) {
		    if( cellarray[pad]->padside != L ) {
			break ;
		    }
		    height = cellarray[pad]->config[0]->top -
				     cellarray[pad]->config[0]->bottom ;
		    width =  cellarray[pad]->config[0]->right -
				     cellarray[pad]->config[0]->left ;
		    if( fixLRBT[0] == 0 ) {
			cellarray[pad]->ycenter = last + separation + 
							height / 2 ;
		    } else {
			cellarray[pad]->ycenter = blockb +
				    padspace[ pad - numcells ] * space ;
		    }
		    cellarray[pad]->xcenter = maxWidth - (width -
							    width / 2) ;
		    last += separation + height ;
		}
	    }
	}
    }
}

for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    if( cellarray[pad]->padside == R ) {
	count = 1 ;
	height = cellarray[pad]->config[0]->top -
				 cellarray[pad]->config[0]->bottom ;

	for( pad++ ; pad <= numcells + numpads ; pad++ ) {
	    if( cellarray[pad]->padside != R ) {
		break ;
	    }
	    count++ ;
	    height += cellarray[pad]->config[0]->top -
				      cellarray[pad]->config[0]->bottom;
	}
	if( fixLRBT[1] == 0 ) {
	    space = coreHeight - height ;
	    separation = space / (count + 1) ;
	    if( separation < 0 ) {
		separation = 0 ;
	    }
	} else {
	    space = (coreHeight >= height) ? coreHeight : height ;
	}
	if( height > coreHeight ) {
	    zyshift += (height - coreHeight) / 2 ;
	}

	for( pad = numcells + 1; pad <= numcells + numpads; pad++){
	    if( cellarray[pad]->padside == R ) {
		height = cellarray[pad]->config[0]->top -
				     cellarray[pad]->config[0]->bottom ;
		width =  cellarray[pad]->config[0]->right -
				     cellarray[pad]->config[0]->left ;
		if( fixLRBT[1] == 0 ) {
		    cellarray[pad]->ycenter = blockb + separation + 
							    height / 2 ;
		} else {
		    cellarray[pad]->ycenter = blockb +
				    padspace[ pad - numcells ] * space ;
		}
		cellarray[pad]->xcenter = blockr + 
				0.1 * (blockr - blockl) + width / 2 ;
		last = blockb + separation + height ;

		for( pad++ ; pad <= numcells + numpads ; pad++ ) {
		    if( cellarray[pad]->padside != R ) {
			break ;
		    }
		    height = cellarray[pad]->config[0]->top -
				     cellarray[pad]->config[0]->bottom ;
		    width =  cellarray[pad]->config[0]->right -
				     cellarray[pad]->config[0]->left ;
		    if( fixLRBT[1] == 0 ) {
			cellarray[pad]->ycenter = last + separation + 
						    height / 2 ;
		    } else {
			cellarray[pad]->ycenter = blockb +
				    padspace[pad - numcells] * space ;
		    }
		    cellarray[pad]->xcenter = blockr + 
				0.1 * (blockr - blockl) + width / 2 ;
		    last += separation + height ;
		}
	    }
	}
    }
}

/*
 *    Center the Core
 */
for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    if( cellarray[pad]->padside == T || cellarray[pad]->padside == B ){
	cellarray[pad]->xcenter += xshift ;
    }
}

/*
 *    Center the Core  ( due to pad limitation ) 
 */
blockl += zxshift ;
blockr += zxshift ;
blockb += zyshift ;
blockt += zyshift ;
for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    if( cellarray[pad]->padside == R ) {
	cellarray[pad]->xcenter += 2 * zxshift ;
    }
}
for( pad = numcells + 1 ; pad <= numcells + numpads ; pad++ ) {
    if( cellarray[pad]->padside == T ) {
	cellarray[pad]->ycenter += 2 * zyshift ;
    }
}

return ;
}
