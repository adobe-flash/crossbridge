#include "custom.h"
extern int finalShot ;

extern int wireestx( int xc , int yy1 , int yy2 , double factor3 );
extern int wireesty( int yc , int xx1 , int xx2 , double factor3 );

void loadbins(int new)
{

CELLBOXPTR ptr ;
TILEBOXPTR tileptr , tptr ;
int i , j , k ;
int startx , endx , starty , endy , *occuptr ;
int lowBinX , highBinX , lowBinY , highBinY ;
int cell , left , right , bottom , top , rl , tb ;
int maxhalfdim ;


for( i = 0 ; i <= numBinsX ; i++ ) {
    for( j = 0 ; j <= numBinsY ; j++ ) {
	for( k = 0 ; k <= 4 + numcells ; k++ ) {
	    blockarray[i][j][k] = 0 ;
	}
    }
}


left   = blockl ;
right  = blockr ;
bottom = blockb ;
top    = blockt ;

rl = (right - left) / 2 ;
tb = (top - bottom) / 2 ;
maxhalfdim = ( (right - left) >= (top - bottom) ) ?
				    right - left : top - bottom ;

if( new ) {
    i = 0 ; 

    /* ************************************************************** */

    ptr = cellarray[ numcells + numpads + ++i ] = ALLOC( CELLBOX ) ;

    ptr->numtiles = 1  ;
    ptr->numterms = 0  ;
    ptr->xcenter  = left - maxhalfdim / 2 ;
    ptr->ycenter  = (top + bottom) / 2    ; 

    tptr = ptr->config[0] = ALLOC( TILEBOX )  ;
    tptr->termptr = TERMNULL ;
    tptr->nexttile = ALLOC( TILEBOX )   ;

    tptr->nexttile->nexttile = TILENULL ;
    tptr->nexttile->termptr = TERMNULL  ;

    for( j = 1 ; j < 8 ; j++ ) {
	ptr->config[ j ] = TILENULL ;
    }

    tptr->left            = -maxhalfdim / 2 ;
    tptr->nexttile->left  = -maxhalfdim / 2 ;
    tptr->right           =  maxhalfdim / 2 ;
    tptr->nexttile->right =  maxhalfdim / 2 ;

    tptr->bottom           = -tb    ;
    tptr->nexttile->bottom = -tb    ;
    tptr->top              = (top - bottom) - tb ;
    tptr->nexttile->top    = (top - bottom) - tb ;
	    
    /* ************************************************************* */

    ptr = cellarray[ numcells + numpads + ++i ] = ALLOC( CELLBOX ) ;

    ptr->numtiles = 1  ;
    ptr->numterms = 0  ;
    ptr->xcenter  = right + maxhalfdim / 2 ;
    ptr->ycenter  = (top + bottom) / 2     ; 

    tptr = ptr->config[0] = ALLOC( TILEBOX )  ;
    tptr->termptr = TERMNULL ;
    tptr->nexttile = ALLOC( TILEBOX )   ;

    tptr->nexttile->nexttile = TILENULL ;
    tptr->nexttile->termptr  = TERMNULL ;

    for( j = 1 ; j < 8 ; j++ ) {
	ptr->config[ j ] = TILENULL ;
    }

    tptr->left            = -maxhalfdim / 2 ;
    tptr->nexttile->left  = -maxhalfdim / 2 ;
    tptr->right           =  maxhalfdim / 2 ;
    tptr->nexttile->right =  maxhalfdim / 2 ;

    tptr->bottom           = -tb ;
    tptr->nexttile->bottom = -tb ;
    tptr->top              = (top - bottom) - tb ;
    tptr->nexttile->top    = (top - bottom) - tb ;
	    

    /* ************************************************************* */

    ptr = cellarray[ numcells + numpads + ++i ] = ALLOC( CELLBOX ) ;

    ptr->numtiles = 1  ;
    ptr->numterms = 0  ;
    ptr->xcenter  = (left + right) / 2      ; 
    ptr->ycenter  = bottom - maxhalfdim / 2 ;

    tptr = ptr->config[0] = ALLOC( TILEBOX ) ;
    tptr->termptr  = TERMNULL ;
    tptr->nexttile = ALLOC( TILEBOX ) ;

    tptr->nexttile->nexttile = TILENULL ;
    tptr->nexttile->termptr  = TERMNULL ;

    for( j = 1 ; j < 8 ; j++ ) {
	ptr->config[j] = TILENULL ;
    }

    tptr->bottom           = -maxhalfdim / 2 ;
    tptr->nexttile->bottom = -maxhalfdim / 2 ;
    tptr->top              =  maxhalfdim / 2 ;
    tptr->nexttile->top    =  maxhalfdim / 2 ;

    tptr->left            = -rl    ;
    tptr->nexttile->left  = -rl    ;
    tptr->right           = (right - left) - rl ;
    tptr->nexttile->right = (right - left) - rl ;
	    

    /* ************************************************************** */

    ptr = cellarray[ numcells + numpads + ++i ] = ALLOC( CELLBOX ) ;

    ptr->numtiles = 1  ;
    ptr->numterms = 0  ;
    ptr->xcenter  = (left + right) / 2   ; 
    ptr->ycenter  = top + maxhalfdim / 2 ;

    tptr = ptr->config[0] = ALLOC( TILEBOX ) ;
    tptr->termptr  = TERMNULL ;
    tptr->nexttile = ALLOC( TILEBOX ) ;

    tptr->nexttile->nexttile = TILENULL ;
    tptr->nexttile->termptr  = TERMNULL ;

    for( j = 1 ; j < 8 ; j++ ) {
	ptr->config[j] = TILENULL ;
    }

    tptr->bottom           = -maxhalfdim / 2 ;
    tptr->nexttile->bottom = -maxhalfdim / 2 ;
    tptr->top              =  maxhalfdim / 2 ;
    tptr->nexttile->top    =  maxhalfdim / 2 ;

    tptr->left            = -rl ;
    tptr->nexttile->left  = -rl ;
    tptr->right           = (right - left) - rl ;
    tptr->nexttile->right = (right - left) - rl ;

} else {

    i = 0 ; 

    /* ************************************************************** */

    ptr = cellarray[ numcells + numpads + ++i ] ;

    ptr->xcenter  = left - maxhalfdim / 2 ;
    ptr->ycenter  = (top + bottom) / 2    ; 

    tptr = ptr->config[0] ;

    tptr->left            = -maxhalfdim / 2 ;
    tptr->nexttile->left  = -maxhalfdim / 2 ;
    tptr->right           =  maxhalfdim / 2 ;
    tptr->nexttile->right =  maxhalfdim / 2 ;

    tptr->bottom           = -tb    ;
    tptr->nexttile->bottom = -tb    ;
    tptr->top              = (top - bottom) - tb ;
    tptr->nexttile->top    = (top - bottom) - tb ;
	    
    /* ************************************************************* */

    ptr = cellarray[ numcells + numpads + ++i ] ;

    ptr->xcenter  = right + maxhalfdim / 2 ;
    ptr->ycenter  = (top + bottom) / 2     ; 

    tptr = ptr->config[0] ;

    tptr->left            = -maxhalfdim / 2 ;
    tptr->nexttile->left  = -maxhalfdim / 2 ;
    tptr->right           =  maxhalfdim / 2 ;
    tptr->nexttile->right =  maxhalfdim / 2 ;

    tptr->bottom           = -tb ;
    tptr->nexttile->bottom = -tb ;
    tptr->top              = (top - bottom) - tb ;
    tptr->nexttile->top    = (top - bottom) - tb ;
	    

    /* ************************************************************* */

    ptr = cellarray[ numcells + numpads + ++i ] ;

    ptr->xcenter  = (left + right) / 2      ; 
    ptr->ycenter  = bottom - maxhalfdim / 2 ;

    tptr = ptr->config[0] ;

    tptr->bottom           = -maxhalfdim / 2 ;
    tptr->nexttile->bottom = -maxhalfdim / 2 ;
    tptr->top              =  maxhalfdim / 2 ;
    tptr->nexttile->top    =  maxhalfdim / 2 ;

    tptr->left            = -rl    ;
    tptr->nexttile->left  = -rl    ;
    tptr->right           = (right - left) - rl ;
    tptr->nexttile->right = (right - left) - rl ;
	    

    /* ************************************************************** */

    ptr = cellarray[ numcells + numpads + ++i ] ;

    ptr->xcenter  = (left + right) / 2   ; 
    ptr->ycenter  = top + maxhalfdim / 2 ;

    tptr = ptr->config[0] ;

    tptr->bottom           = -maxhalfdim / 2 ;
    tptr->nexttile->bottom = -maxhalfdim / 2 ;
    tptr->top              =  maxhalfdim / 2 ;
    tptr->nexttile->top    =  maxhalfdim / 2 ;

    tptr->left            = -rl ;
    tptr->nexttile->left  = -rl ;
    tptr->right           = (right - left) - rl ;
    tptr->nexttile->right = (right - left) - rl ;

}

for( cell = 1 ; cell <= numcells + numpads + 4 ; cell++ ) {
    if( cell > numcells && cell <= numcells + numpads ) {
	continue ;
    }
    ptr = cellarray[cell] ;
    tileptr = ptr->config[ptr->orient] ;

    startx = ptr->xcenter + tileptr->left ;
    endx   = ptr->xcenter + tileptr->right;
    starty = ptr->ycenter + tileptr->bottom ; 
    endy   = ptr->ycenter + tileptr->top;
    if( cell <= numcells ) {
	if( finalShot == 0 ) {
	    startx -= wireestx(startx, starty, endy, tileptr->lweight );
	    endx   += wireestx(endx, starty, endy, tileptr->rweight );
	    starty -= wireesty(starty, startx, endx, tileptr->bweight );
	    endy   += wireesty(endy, startx, endx, tileptr->tweight );
	} else {
	    startx -= tileptr->lborder ;
	    endx   += tileptr->rborder;
	    starty -= tileptr->bborder ; 
	    endy   += tileptr->tborder;
	}
    }

    lowBinX = (startx - binOffsetX) / binWidthX ;
    if( lowBinX < 1 ) {
	lowBinX = 1 ;
    } else if( lowBinX > numBinsX ) {
	lowBinX = numBinsX ;
    }
    highBinX = (endx - binOffsetX) / binWidthX ;
    if( highBinX > numBinsX ) {
	highBinX = numBinsX ;
    } else if( highBinX < 1 ) {
	highBinX = 1 ;
    }
    if( lowBinX == highBinX ) {
	lowBinY = (starty - binOffsetY) / binWidthY ;
	if( lowBinY < 1 ) {
	    lowBinY = 1 ;
	} else if( lowBinY > numBinsY ) {
	    lowBinY = numBinsY ;
	}
	highBinY = (endy - binOffsetY) / binWidthY ;
	if( highBinY > numBinsY ) {
	    highBinY = numBinsY ;
	} else if( highBinY < 1 ) {
	    highBinY = 1 ;
	}
	if( lowBinY == highBinY ) {
	    occuptr = blockarray[lowBinX][lowBinY] ;
	} else {
	    occuptr = bucket ;
	}
    } else {
	occuptr = bucket ;
    }
    occuptr[ ++occuptr[0]] = cell ;
}

return ;
}
