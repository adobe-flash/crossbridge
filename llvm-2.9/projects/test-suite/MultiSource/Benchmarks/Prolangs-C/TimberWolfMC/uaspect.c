#include "custom.h"

extern void move( int moveType );
extern void rect( int *l , int *b , int *r , int *t );
extern void point( int *x , int *y );
extern int ufixnet( TERMBOXPTR termsptr );
extern void ufixpin( TERMBOXPTR termsptr , int flag , int targetx ,
		    int targety );
extern int usoftnet( CELLBOXPTR cellptr );
extern void usoftpin( CELLBOXPTR cellptr , int flag , int targetx ,
		     int targety , int orient );

int uaspect( int a , double newAspect )
{

CELLBOXPTR acellptr , ptr ;
TERMNETSPTR tmptr ;
TILEBOXPTR atileptr , tptr , tptr0 ;
TILEBOXPTR tileptr1 ;
TERMBOXPTR termptr , atermptr , tmptr1 , tmptr0 ;
UNCOMBOX *UCptr ;
LOCBOX *SLptr , *siteptr , *siteptr0 ;

int tm , site , length , height , orient , aorient ;
int cost , newpenalty , tilenum , termnum ;
int x , y , i ;
int axcenter , aycenter , Hdiv2 , Wdiv2 ;
int oleft , oright , obottom , otop ;
double aspFactor , val ;


acellptr = cellarray[ a ]    ;
axcenter = acellptr->xcenter ;
aycenter = acellptr->ycenter ;
aorient  = acellptr->orient  ;
atileptr = acellptr->config[ aorient ] ;
atermptr = atileptr->termptr ;
oleft   = atileptr->oleft    ;
oright  = atileptr->oright   ;
obottom = atileptr->obottom  ;
otop    = atileptr->otop     ;

UCptr = acellptr->unComTerms ;
SLptr = atileptr->siteLocArray ;

if( aorient <= 3 ) {
    aspFactor = sqrt( newAspect / acellptr->aspectO ) ;
} else {
    aspFactor = sqrt( acellptr->aspectO / newAspect ) ;
}
val = (double)(otop - obottom) * aspFactor ;
Hdiv2 = ROUND( val ) / 2 ;
val = (double)(oright - oleft) / aspFactor ;
Wdiv2 = ROUND( val ) / 2 ;

newpenalty = penalty ;

newpenalty -= (*overlap)( a, axcenter, aycenter, aorient, 0, 1, 0 ) ;
occa1ptr = blockarray[binX][binY] ;

newpenalty += (*overlapx)( a, oleft, obottom, Wdiv2, Hdiv2, aspFactor);
occa2ptr = blockarray[binX][binY] ;


for( termptr = atermptr ; termptr != TERMNULL ; termptr =
						termptr->nextterm ) {
    val = (double)(termptr->oxpos - oleft) / aspFactor ;
    x = axcenter + ROUND( val ) - Wdiv2 ;
    val = (double)(termptr->oypos - obottom) * aspFactor ;
    y = aycenter + ROUND( val ) - Hdiv2 ;

    tmptr = termarray[ termptr->terminal ] ;
    netarray[ tmptr->net ]->flag = 1 ;
    tmptr->termptr->flag = 1 ;
    tmptr->termptr->newx = x ;
    tmptr->termptr->newy = y ;
}
for( tm = 1 ; tm <= acellptr->numUnComTerms ; tm++ ) {
    val = (double)(SLptr[ UCptr[tm].site ].oxpos - oleft) / aspFactor ;
    x = axcenter + ROUND( val ) - Wdiv2 ;
    val = (double)(SLptr[ UCptr[tm].site ].oypos - obottom) * aspFactor;
    y = aycenter + ROUND( val ) - Hdiv2 ;

    tmptr = termarray[ UCptr[tm].terminal ] ;
    netarray[ tmptr->net ]->flag = 1 ;
    tmptr->termptr->flag = 1 ;
    tmptr->termptr->newx = x ;
    tmptr->termptr->newy = y ;
}

cost = funccost ;

cost += ufixnet( atermptr ) ;
cost += usoftnet( acellptr ) ;


if( ( cost + newpenalty <= funccost + penalty ) || 
	(exp( (double)( funccost + penalty - cost - newpenalty ) /
	T) > ( (double) RAND / (double)0x7fffffff ) ) ) {

    ufixpin( atermptr, 1, 0 , 0 ) ;
    usoftpin( acellptr, 1, 0, 0, 0 ) ;

    if( occa1ptr != occa2ptr ) {
	for( i = 1 ; occa1ptr[ i ] != a ; i++ ) ;
	occa1ptr[ i ] = occa1ptr[  occa1ptr[0]--  ] ;
	occa2ptr[ ++occa2ptr[0]  ] = a ;
    }

    /*
     *   Time to compute the new coordinates for the cell
     *   since its aspect ratio has changed.  Do it for
     *   the orient 0 version and then use the routines
     *   from readcells() to transfer for other orients.
     */

    for( tileptr1 = atileptr ; tileptr1 != TILENULL ;
				   tileptr1 = tileptr1->nexttile ){
	val = (double)(tileptr1->oleft - oleft) / aspFactor ;
	tileptr1->left   = ROUND( val ) - Wdiv2 ;
	val = (double)(tileptr1->oright - oleft) / aspFactor ;
	tileptr1->right  = ROUND( val ) - Wdiv2 ;
	val = (double)(tileptr1->obottom - obottom) * aspFactor ;
	tileptr1->bottom = ROUND( val ) - Hdiv2 ;
	val = (double)(tileptr1->otop - obottom) * aspFactor ;
	tileptr1->top    = ROUND( val ) - Hdiv2 ;
    }

    for( termptr = atermptr ; termptr != TERMNULL ; termptr =
					    termptr->nextterm ) {
	val = (double)(termptr->oxpos - oleft) / aspFactor ;
	termptr->xpos = ROUND( val ) - Wdiv2 ;
	val = (double)(termptr->oypos - obottom) * aspFactor ;
	termptr->ypos = ROUND( val ) - Hdiv2 ;
    }
    for( site = 1 ; site <= acellptr->numsites ; site++ ) {
	val = (double)(SLptr[ site ].oxpos - oleft) / aspFactor ;
	SLptr[site].xpos = ROUND( val ) - Wdiv2 ;
	val = (double)(SLptr[ site ].oypos - obottom) * aspFactor ;
	SLptr[site].ypos = ROUND( val ) - Hdiv2 ;
   } 

    /*
	NOW WE HAVE TO LOAD IN THE OTHER CONFIGURATIONS
    */

    ptr = acellptr ;

    if( aorient != 0 ) {
	height = atileptr->top - atileptr->bottom ;
	length = atileptr->right - atileptr->left ;
	switch( aorient ) {
	    case 1 : orient = 1 ;
		     break ;
	    case 2 : orient = 2 ;
		     break ;
	    case 3 : orient = 3 ;
		     break ;
	    case 4 : orient = 4 ;
		     break ;
	    case 5 : orient = 5 ;
		     break ;
	    case 6 : orient = 7 ;
		     break ;
	    case 7 : orient = 6 ;
		     break ;
	}
	tptr0 = ptr->config[ 0 ] ;
	tptr  = atileptr ;
	tptr0->left     = tptr->left   ;
	tptr0->right    = tptr->right  ;
	tptr0->bottom   = tptr->bottom ;
	tptr0->top      = tptr->top    ;

	move( orient ) ;

	rect( &tptr0->left , &tptr0->bottom , &tptr0->right , 
						&tptr0->top ) ;

	if( (height % 2 != 0 && (orient == 4 || 
					orient == 6))||
	    (length % 2 != 0 && (orient == 2 || 
					orient == 3))){

	    tptr0->left++  ;
	    tptr0->right++ ;
	}
	if( (height % 2 != 0 && (orient == 1 || 
					orient == 3))||
	    (length % 2 != 0 && (orient == 4 || 
					orient == 7))){

	    tptr0->bottom++ ;
	    tptr0->top++    ;
	}
	for( tilenum = 1 ; tilenum <= ptr->numtiles ; 
						tilenum++ ) {
	    tptr0 = tptr0->nexttile ;
	    tptr  = tptr->nexttile ; 
	    tptr0->left   = tptr->left   ;
	    tptr0->right  = tptr->right  ;
	    tptr0->bottom = tptr->bottom ;
	    tptr0->top    = tptr->top    ;

	    move( orient ) ; 

	    rect( &tptr0->left, &tptr0->bottom, &tptr0->right , 
						&tptr0->top ) ;

	    if( (height % 2 != 0 && (orient == 4 || 
						orient == 6))||
		(length % 2 != 0 && (orient == 2 || 
						orient == 3))){
		tptr0->left++  ;
		tptr0->right++ ;
	    }
	    if( (height % 2 != 0 && (orient == 1 || 
						orient == 3))||
		(length % 2 != 0 && (orient == 4 || 
						orient == 7))){
		tptr0->bottom++ ;
		tptr0->top++    ;
	    }
	}

	/*  And NOW for the fixed terminals */

	if( ptr->numterms != 0 ) {
	    tmptr0 = ptr->config[ 0 ]->termptr ;
	    tmptr1 = atermptr ;
	    tmptr0->xpos = tmptr1->xpos ;
	    tmptr0->ypos = tmptr1->ypos ;

	    move( orient ) ; 

	    point( &tmptr0->xpos , &tmptr0->ypos ) ;

	    if( (height % 2 != 0 && (orient == 4 || 
						orient == 6))||
		(length % 2 != 0 && (orient == 2 || 
						orient == 3))){
		tmptr0->xpos++  ;
	    }
	    if( (height % 2 != 0 && (orient == 1 || 
						orient == 3))||
		(length % 2 != 0 && (orient == 4 || 
						orient == 7))){
		tmptr0->ypos++ ;
	    }
	    for( termnum = 2 ; termnum <= ptr->numterms ; 
						termnum++ ) {
		tmptr0 = tmptr0->nextterm ;
		tmptr1  = tmptr1->nextterm ; 
		tmptr0->xpos = tmptr1->xpos ;
		tmptr0->ypos = tmptr1->ypos ;

		move( orient ) ; 

		point( &tmptr0->xpos , &tmptr0->ypos ) ;

		if( (height % 2 != 0 && (orient == 4 || 
						orient == 6))||
		    (length % 2 != 0 && (orient == 2 || 
						orient == 3))){
		    tmptr0->xpos++  ;
		}
		if( (height % 2 != 0 && (orient == 1 || 
						orient == 3))||
		    (length % 2 != 0 && (orient == 4 || 
						orient == 7))){
		    tmptr0->ypos++ ;
		}
	    }
	}

	/*  Now the sites for the soft cells */

	if( ptr->numsites != 0 ) {
	    siteptr0 = ptr->config[ 0 ]->siteLocArray ;
	    siteptr = SLptr ;
	    for( site = 1 ; site <= ptr->numsites ; site++ ) {
		siteptr0[ site ].xpos = siteptr[ site ].xpos ;
		siteptr0[ site ].ypos = siteptr[ site ].ypos ;

		move( orient ) ; 

		point( &(siteptr0[ site ].xpos) , 
		       &(siteptr0[ site ].ypos) );

		if( (height % 2 != 0 && (orient == 4 || 
						orient == 6))||
		    (length % 2 != 0 && (orient == 2 ||
						orient == 3))){
		    siteptr0[ site ].xpos++ ;
		}
		if( (height % 2 != 0 && (orient == 1 || 
						orient == 3))||
		    (length % 2 != 0 && (orient == 4 ||
						orient == 7))){
		    siteptr0[ site ].ypos++ ;
		}
	    }
	}
    }
    /* ************************************************** */
    height = ptr->config[0]->top - ptr->config[0]->bottom ;
    length = ptr->config[0]->right - ptr->config[0]->left ;

    for( orient = 1 ; orient <= 7 ; orient++ ) {
	if( orient == aorient ) {
	    continue ;
	}
	tptr0 = ptr->config[ 0 ] ;
	tptr  = ptr->config[ orient ] ; 
	tptr->left     = tptr0->left   ;
	tptr->right    = tptr0->right  ;
	tptr->bottom   = tptr0->bottom ;
	tptr->top      = tptr0->top    ;

	move( orient ) ;

	rect( &tptr->left , &tptr->bottom , &tptr->right , 
						&tptr->top ) ;

	if( (height % 2 != 0 && (orient == 4 || orient == 6))||
	    (length % 2 != 0 && (orient == 2 || orient == 3))){

	    tptr->left++  ;
	    tptr->right++ ;
	}
	if( (height % 2 != 0 && (orient == 1 || orient == 3))||
	    (length % 2 != 0 && (orient == 4 || orient == 7))){

	    tptr->bottom++ ;
	    tptr->top++    ;
	}
	for( tilenum = 1 ; tilenum <= ptr->numtiles ; 
						tilenum++ ) {
	    tptr0 = tptr0->nexttile ;
	    tptr  = tptr->nexttile ; 
	    tptr->left     = tptr0->left   ;
	    tptr->right    = tptr0->right  ;
	    tptr->bottom   = tptr0->bottom ;
	    tptr->top      = tptr0->top    ;

	    move( orient ) ; 

	    rect( &tptr->left, &tptr->bottom, &tptr->right , 
						&tptr->top ) ;

	    if( (height % 2 != 0 && (orient == 4 || 
						orient == 6))||
		(length % 2 != 0 && (orient == 2 || 
						orient == 3))){
		tptr->left++  ;
		tptr->right++ ;
	    }
	    if( (height % 2 != 0 && (orient == 1 || 
						orient == 3))||
		(length % 2 != 0 && (orient == 4 || 
						orient == 7))){
		tptr->bottom++ ;
		tptr->top++    ;
	    }
	}
    }

    /*  And NOW for the fixed terminals */

    if( ptr->numterms != 0 ) {

	for( orient = 1 ; orient <= 7 ; orient++ ) {
	    if( orient == aorient ) {
		continue ;
	    }
	    tmptr0 = ptr->config[ 0 ]->termptr ;
	    tmptr1 = ptr->config[ orient ]->termptr ; 
	    tmptr1->xpos     = tmptr0->xpos     ;
	    tmptr1->ypos     = tmptr0->ypos     ;

	    move( orient ) ;

	    point( &tmptr1->xpos , &tmptr1->ypos ) ;

	    if( (height % 2 != 0 && (orient == 4 || 
						orient == 6))||
		(length % 2 != 0 && (orient == 2 || 
						orient == 3))){
		tmptr1->xpos++  ;
	    }
	    if( (height % 2 != 0 && (orient == 1 || 
						orient == 3))||
		(length % 2 != 0 && (orient == 4 || 
						orient == 7))){
		tmptr1->ypos++ ;
	    }
	    for( termnum = 2 ; termnum <= ptr->numterms ; 
						termnum++ ) {
		tmptr0 = tmptr0->nextterm ;
		tmptr1  = tmptr1->nextterm ; 
		tmptr1->xpos     = tmptr0->xpos ;
		tmptr1->ypos     = tmptr0->ypos ;

		move( orient ) ; 

		point( &tmptr1->xpos , &tmptr1->ypos ) ;

		if( (height % 2 != 0 && (orient == 4 || 
						orient == 6))||
		    (length % 2 != 0 && (orient == 2 || 
						orient == 3))){
		    tmptr1->xpos++  ;
		}
		if( (height % 2 != 0 && (orient == 1 || 
						orient == 3))||
		    (length % 2 != 0 && (orient == 4 || 
						orient == 7))){
		    tmptr1->ypos++ ;
		}
	    }
	}
    }

    /*  Now the sites for the soft cells */

    if( ptr->numsites != 0 ) {

	siteptr0 = ptr->config[ 0 ]->siteLocArray ;
	for( orient = 1 ; orient <= 7 ; orient++ ) {
	    if( orient == aorient ) {
		continue ;
	    }
	    siteptr = ptr->config[ orient ]->siteLocArray ; 
	    for( site = 1 ; site <= ptr->numsites ; site++ ) {
		siteptr[ site ].xpos = siteptr0[ site ].xpos ;
		siteptr[ site ].ypos = siteptr0[ site ].ypos ;

		move( orient ) ; 

		point( &(siteptr[ site ].xpos) , 
		       &(siteptr[ site ].ypos) );

		if( (height % 2 != 0 && (orient == 4 || 
						orient == 6))||
		    (length % 2 != 0 && (orient == 2 ||
						orient == 3))){
		    siteptr[ site ].xpos++ ;
		}
		if( (height % 2 != 0 && (orient == 1 || 
						orient == 3))||
		    (length % 2 != 0 && (orient == 4 ||
						orient == 7))){
		    siteptr[ site ].ypos++ ;
		}
	    }
	}
    }
    /* **************************************************** */
    acellptr->aspect = newAspect ;

    funccost = cost ;
    penalty = newpenalty ;
    return (1) ;
} else {
    return (0) ;
}
}
