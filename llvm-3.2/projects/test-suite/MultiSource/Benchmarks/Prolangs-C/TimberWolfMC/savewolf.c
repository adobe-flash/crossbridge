#include "custom.h"

extern void move( int moveType );
extern void rect( int *l , int *b , int *r , int *t );
extern void point( int *x , int *y );
extern void loadbins(int new);

void savewolf(void)
{

FILE *fp ;
char filename[1024] ;
int cell , terminal ;
CELLBOXPTR cellptr ;

sprintf( filename , "%s.sav", cktName ) ;
fp = fopen( filename , "w" ) ;

for( cell = 1 ; cell <= numcells ; cell++ ) {
    cellptr = cellarray[ cell ] ;
    fprintf( fp , "%d %d %d %d\n", cell , cellptr->orient , 
			cellptr->xcenter , cellptr->ycenter ) ;
    for( terminal = 1 ; terminal <= cellptr->numUnComTerms ;
						terminal++ ) {
	fprintf( fp , " %d ", cellptr->unComTerms[ terminal ].
							site );
	if( terminal == cellptr->numUnComTerms ) {
	    fprintf( fp , " %lf\n" , cellptr->aspect );
	}
    }
}
fclose( fp ) ;
return ;
}


void TW_oldinput( FILE *fp )
{

int i , cell , orient , tm , site , terminal , height , length ;
int xcenter , ycenter , xpos , ypos , x , y ;
int oleft , oright , obottom , otop , Hdiv2 , Wdiv2 , aorient ;
double aspect , aspFactor , val ;
int tilenum , termnum ;
CELLBOXPTR ptr ;
LOCBOX *SLptr , *siteptr , *siteptr0 ;
TILEBOXPTR atileptr , tileptr1 , tptr , tptr0 ;
TERMBOXPTR term , atermptr , termptr , tmptr0 , tmptr1 ;
TERMNETSPTR teptr ;



while( fscanf( fp , " %d %d %d %d ", &cell , &orient , 
				    &xcenter , &ycenter ) == 4 ) {
    ptr = cellarray[ cell ] ;

    ptr->orient  = orient  ;
    ptr->xcenter = xcenter ;
    ptr->ycenter = ycenter ;
    aorient = orient ;

    for( terminal = 1; terminal <= ptr->numUnComTerms; terminal++ ){
	if( terminal == 1 ) {
	    for( i = 1 ; i <= ptr->numsites ; i++ ) {
		ptr->siteContent[ i ].contents = 0 ;
	    }
	}
	fscanf( fp , " %d ", &site ) ;
	ptr->unComTerms[ terminal ].site = site ;
	++ptr->siteContent[ site ].contents ;
	if( terminal == ptr->numUnComTerms ) {
	    fscanf( fp , "%lf" , &aspect ) ;
	    ptr->aspect = aspect ;
	}
    }
    if( ptr->softflag == 1 && ptr->aspUB > 0.01 + ptr->aspLB ) {
	atileptr = ptr->config[ aorient ] ;
	atermptr = atileptr->termptr ;
	oleft   = atileptr->oleft    ;
	oright  = atileptr->oright   ;
	obottom = atileptr->obottom  ;
	otop    = atileptr->otop     ;

	SLptr = atileptr->siteLocArray ;

	if( aorient <= 3 ) {
	    aspFactor = sqrt( aspect / ptr->aspectO ) ;
	} else {
	    aspFactor = sqrt( ptr->aspectO / aspect ) ;
	}
	val = (double)(otop - obottom) * aspFactor ;
	Hdiv2 = ROUND( val ) / 2 ;
	val = (double)(oright - oleft) / aspFactor ;
	Wdiv2 = ROUND( val ) / 2 ;

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
	    val = (double)(tileptr1->obottom - obottom) * aspFactor;
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
	for( site = 1 ; site <= ptr->numsites ; site++ ) {
	    val = (double)(SLptr[ site ].oxpos - oleft) / aspFactor ;
	    SLptr[site].xpos = ROUND( val ) - Wdiv2 ;
	    val = (double)(SLptr[site].oypos - obottom) * aspFactor ;
	    SLptr[site].ypos = ROUND( val ) - Hdiv2 ;
       }

	/*
	    NOW WE HAVE TO LOAD IN THE OTHER CONFIGURATIONS
	*/

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
	    for( tilenum = 1 ; tilenum <= ptr->numtiles ; 
					    tilenum++ ) {
		tptr0 = tptr0->nexttile ;
		tptr  = tptr->nexttile ; 
		tptr0->left   = tptr->left   ;
		tptr0->right  = tptr->right  ;
		tptr0->bottom = tptr->bottom ;
		tptr0->top    = tptr->top    ;

		move( orient ) ; 

		rect( &tptr0->left,&tptr0->bottom,&tptr0->right,
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
		for( site = 1 ; site <= ptr->numsites; site++ ){
		    siteptr0[ site ].xpos = siteptr[site].xpos ;
		    siteptr0[ site ].ypos = siteptr[site].ypos ;

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
		for( site = 1; site <= ptr->numsites; site++ ) {
		    siteptr[ site ].xpos = siteptr0[site].xpos ;
		    siteptr[ site ].ypos = siteptr0[site].ypos ;

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
    }
    for( term = ptr->config[ aorient ]->termptr ; 
		    term != TERMNULL ; term = term->nextterm ) {
	xpos = term->xpos ;
	ypos = term->ypos ;
	teptr = termarray[ term->terminal ] ;
	teptr->termptr->xpos = xpos + xcenter ;
	teptr->termptr->ypos = ypos + ycenter ;
    }
    for( tm = 1 ; tm <= ptr->numUnComTerms ; tm++ ) {
	terminal = ptr->unComTerms[ tm ].terminal ;
	site = ptr->unComTerms[ tm ].site ;
	x = ptr->config[ aorient ]->siteLocArray[ site ].xpos ;
	y = ptr->config[ aorient ]->siteLocArray[ site ].ypos ;

	teptr = termarray[ terminal ] ;
	teptr->termptr->xpos = x + xcenter ;
	teptr->termptr->ypos = y + ycenter ;
    }
    loadbins(0) ;
}
return ;
}
