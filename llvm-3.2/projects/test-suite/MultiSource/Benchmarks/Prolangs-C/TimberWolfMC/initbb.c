#include "custom.h"
int bbleft , bbright , bbbottom , bbtop ;
int bbbl , bbbr , bbbb , bbbt ;

void initbb(void)
{

CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
int xcenter , ycenter , cell , l , r , b , t ;

bbleft = 1000000 ;
bbbottom = 1000000 ;
bbtop = 0 ;
bbright = 0 ;

for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[cell] ;
    xcenter = cellptr->xcenter ;
    ycenter = cellptr->ycenter ;
    tileptr = cellptr->config[cellptr->orient] ;
    l = xcenter + tileptr->left ;
    r = xcenter + tileptr->right ;
    b = ycenter + tileptr->bottom ;
    t = ycenter + tileptr->top ;
    if( l < bbleft ) {
	bbleft = l ;
    }
    if( r > bbright ) {
	bbright = r ;
    }
    if( b < bbbottom ) {
	bbbottom = b ;
    }
    if( t > bbtop ) {
	bbtop = t ;
    }
}
return ;

}




int deltaBB( int cell , int xb , int yb )
{

CELLBOXPTR cellptr ;
TILEBOXPTR tileptr ;
int xcenter , ycenter , l , r , b , t , c ;
int cost ;

cost = 0 ;
cellptr = cellarray[cell] ;
xcenter = cellptr->xcenter ;
ycenter = cellptr->ycenter ;
tileptr = cellptr->config[cellptr->orient] ;
l = xcenter + tileptr->left ;
r = xcenter + tileptr->right ;
b = ycenter + tileptr->bottom ;
t = ycenter + tileptr->top ;
if( l == bbleft ) {
    bbbl = 1000000 ;
    for( c = 1 ; c <= numcells ; c++ ) {
	if( cell == c ) {
	    continue ;
	}
	cellptr = cellarray[c] ;
	xcenter = cellptr->xcenter ;
	tileptr = cellptr->config[cellptr->orient] ;
	l = xcenter + tileptr->left ;
	if( l < bbbl ) {
	    bbbl = l ;
	}
    }
    cost -= bbbl - bbleft ;
} else {
    bbbl = bbleft ;
}
if( r == bbright ) {
    bbbr = 0 ;
    for( c = 1 ; c <= numcells ; c++ ) {
	if( cell == c ) {
	    continue ;
	}
	cellptr = cellarray[c] ;
	xcenter = cellptr->xcenter ;
	tileptr = cellptr->config[cellptr->orient] ;
	r = xcenter + tileptr->right ;
	if( r > bbbr ) {
	    bbbr = r ;
	}
    }
    cost -= bbright - bbbr ;
} else {
    bbbr = bbright ;
}
if( b == bbbottom ) {
    bbbb = 1000000 ;
    for( c = 1 ; c <= numcells ; c++ ) {
	if( cell == c ) {
	    continue ;
	}
	cellptr = cellarray[c] ;
	ycenter = cellptr->ycenter ;
	tileptr = cellptr->config[cellptr->orient] ;
	b = ycenter + tileptr->bottom ;
	if( b < bbbb ) {
	    bbbb = b ;
	}
    }
    cost -= bbbb - bbbottom ;
} else {
    bbbb = bbbottom ;
}
if( t == bbtop ) {
    bbbt = 0 ;
    for( c = 1 ; c <= numcells ; c++ ) {
	if( cell == c ) {
	    continue ;
	}
	cellptr = cellarray[c] ;
	ycenter = cellptr->ycenter ;
	tileptr = cellptr->config[cellptr->orient] ;
	t = ycenter + tileptr->top ;
	if( t > bbbt ) {
	    bbbt = t ;
	}
    }
    cost -= bbtop - bbbt ;
} else {
    bbbt = bbtop ;
}

cellptr = cellarray[cell] ;
tileptr = cellptr->config[cellptr->orient] ;
l = xb + tileptr->left ;
r = xb + tileptr->right ;
b = yb + tileptr->bottom ;
t = yb + tileptr->top ;
if( l < bbbl ) {
    bbbl = l ;
    cost += bbbl - l ;
}
if( r > bbbr ) {
    bbbr = r ;
    cost += r - bbbr ;
}
if( b < bbbb ) {
    bbbb = b ;
    cost += bbbb - b ;
}
if( t > bbbt ) {
    bbbt = t ;
    cost += t - bbbt ;
}
return(cost) ;

}
