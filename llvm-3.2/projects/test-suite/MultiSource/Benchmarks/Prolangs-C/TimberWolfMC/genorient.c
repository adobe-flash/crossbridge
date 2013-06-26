#include "custom.h"

extern void move( int moveType );
extern void rect( int *l , int *b , int *r , int *t );
extern void point( int *x , int *y );
extern void delHtab(void);

void genorient(void)
{

int cell , orient , tilenum , termnum , aorient ;
int height , length , site , net ;
CELLBOXPTR ptr ;
TILEBOXPTR tptr0 , tptr ;
TERMBOXPTR tmptr0 , tmptr ;
LOCBOX *siteptr0 , *siteptr ;
NETBOXPTR netptr ;


for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    ptr = cellarray[cell] ;
    aorient = ptr->orient ;
    if( aorient != 0 ) {
	tptr0 = ptr->config[0] ;
	height = tptr0->top - tptr0->bottom ;
	length = tptr0->right - tptr0->left ;
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
	tptr = ptr->config[aorient] = (TILEBOXPTR) malloc(sizeof(TILEBOX));
	tptr->nexttile = TILENULL ;
	tptr->termptr  = TERMNULL      ;
	tptr->left     = tptr0->left   ;
	tptr->right    = tptr0->right  ;
	tptr->bottom   = tptr0->bottom ;
	tptr->top      = tptr0->top    ;
	tptr->oleft    = tptr0->oleft   ;
	tptr->oright   = tptr0->oright  ;
	tptr->obottom  = tptr0->obottom ;
	tptr->otop     = tptr0->otop    ;

	tptr->lweight = tptr0->lweight ;
	tptr->rweight = tptr0->rweight ;
	tptr->bweight = tptr0->bweight ;
	tptr->tweight = tptr0->tweight ;
	switch( orient ) {
	    case 1 :  
		tptr0->lweight = tptr->lweight ;
		tptr0->rweight = tptr->rweight ;
		tptr0->bweight = tptr->tweight ;
		tptr0->tweight = tptr->bweight ;
		break ;
	    case 2 :
		tptr0->lweight = tptr->rweight ;
		tptr0->rweight = tptr->lweight ;
		tptr0->bweight = tptr->bweight ;
		tptr0->tweight = tptr->tweight ;
		break ;
	    case 3 :
		tptr0->lweight = tptr->rweight ;
		tptr0->rweight = tptr->lweight ;
		tptr0->bweight = tptr->tweight ;
		tptr0->tweight = tptr->bweight ;
		break ;
	    case 4 :
		tptr0->lweight = tptr->tweight ;
		tptr0->rweight = tptr->bweight ;
		tptr0->bweight = tptr->rweight ;
		tptr0->tweight = tptr->lweight ;
		break ;
	    case 5 :
		tptr0->lweight = tptr->bweight ;
		tptr0->rweight = tptr->tweight ;
		tptr0->bweight = tptr->lweight ;
		tptr0->tweight = tptr->rweight ;
		break ;
	    case 6 :
		tptr0->lweight = tptr->tweight ;
		tptr0->rweight = tptr->bweight ;
		tptr0->bweight = tptr->lweight ;
		tptr0->tweight = tptr->rweight ;
		break ;
	    case 7 :
		tptr0->lweight = tptr->bweight ;
		tptr0->rweight = tptr->tweight ;
		tptr0->bweight = tptr->rweight ;
		tptr0->tweight = tptr->lweight ;
		break ;
	}

	move( orient ) ;

	rect( &tptr0->left, &tptr0->bottom, &tptr0->right, &tptr0->top );

	if( (height % 2 != 0 && (orient == 4 || orient == 6))||
	    (length % 2 != 0 && (orient == 2 || orient == 3))){

	    tptr0->left++  ;
	    tptr0->right++ ;
	}
	tptr0->oleft    = tptr0->left   ;
	tptr0->oright   = tptr0->right  ;
	if( (height % 2 != 0 && (orient == 1 || orient == 3))||
	    (length % 2 != 0 && (orient == 4 || orient == 7))){

	    tptr0->bottom++ ;
	    tptr0->top++    ;
	}
	tptr0->obottom  = tptr0->bottom ;
	tptr0->otop     = tptr0->top    ;
	for( tilenum = 1 ; tilenum <= ptr->numtiles ; tilenum++ ) {
	    tptr0 = tptr0->nexttile ;
	    tptr  = tptr->nexttile = (TILEBOXPTR) malloc(
					    sizeof( TILEBOX ) ) ;
	    tptr->nexttile = TILENULL ;
	    tptr->termptr  = TERMNULL      ;
	    tptr->left    = tptr0->left   ;
	    tptr->right   = tptr0->right   ;
	    tptr->bottom  = tptr0->bottom  ;
	    tptr->top     = tptr0->top     ;
	    tptr->oleft   = tptr0->oleft   ;
	    tptr->oright  = tptr0->oright  ;
	    tptr->obottom = tptr0->obottom ;
	    tptr->otop    = tptr0->otop    ;

	    tptr->lweight = tptr0->lweight ;
	    tptr->rweight = tptr0->rweight ;
	    tptr->bweight = tptr0->bweight ;
	    tptr->tweight = tptr0->tweight ;
	    switch( orient ) {
		case 1 :  
		    tptr0->lweight = tptr->lweight ;
		    tptr0->rweight = tptr->rweight ;
		    tptr0->bweight = tptr->tweight ;
		    tptr0->tweight = tptr->bweight ;
		    break ;
		case 2 :
		    tptr0->lweight = tptr->rweight ;
		    tptr0->rweight = tptr->lweight ;
		    tptr0->bweight = tptr->bweight ;
		    tptr0->tweight = tptr->tweight ;
		    break ;
		case 3 :
		    tptr0->lweight = tptr->rweight ;
		    tptr0->rweight = tptr->lweight ;
		    tptr0->bweight = tptr->tweight ;
		    tptr0->tweight = tptr->bweight ;
		    break ;
		case 4 :
		    tptr0->lweight = tptr->tweight ;
		    tptr0->rweight = tptr->bweight ;
		    tptr0->bweight = tptr->rweight ;
		    tptr0->tweight = tptr->lweight ;
		    break ;
		case 5 :
		    tptr0->lweight = tptr->bweight ;
		    tptr0->rweight = tptr->tweight ;
		    tptr0->bweight = tptr->lweight ;
		    tptr0->tweight = tptr->rweight ;
		    break ;
		case 6 :
		    tptr0->lweight = tptr->tweight ;
		    tptr0->rweight = tptr->bweight ;
		    tptr0->bweight = tptr->lweight ;
		    tptr0->tweight = tptr->rweight ;
		    break ;
		case 7 :
		    tptr0->lweight = tptr->bweight ;
		    tptr0->rweight = tptr->tweight ;
		    tptr0->bweight = tptr->rweight ;
		    tptr0->tweight = tptr->lweight ;
		    break ;
	    }

	    move( orient ) ; 

	    rect( &tptr0->left, &tptr0->bottom, &tptr0->right , 
						&tptr0->top ) ;

	    if( (height % 2 != 0 && (orient == 4 || orient == 6))||
		(length % 2 != 0 && (orient == 2 || orient == 3))){
		tptr0->left++  ;
		tptr0->right++ ;
	    }
	    tptr0->oleft   = tptr0->left   ;
	    tptr0->oright  = tptr0->right  ;
	    if( (height % 2 != 0 && (orient == 1 || orient == 3))||
		(length % 2 != 0 && (orient == 4 || orient == 7))){
		tptr0->bottom++ ;
		tptr0->top++    ;
	    }
	    tptr0->obottom = tptr0->bottom ;
	    tptr0->otop    = tptr0->top    ;
	}

	/*  And NOW for the fixed terminals */

	if( ptr->numterms != 0 ) {
	    tmptr0 = ptr->config[0]->termptr ;
	    tmptr = ptr->config[aorient]->termptr = (TERMBOXPTR) 
					  malloc( sizeof( TERMBOX ) ) ;
	    tmptr->nextterm = TERMNULL ;
	    tmptr->terminal = tmptr0->terminal ;
	    tmptr->xpos = tmptr0->xpos ;
	    tmptr->ypos = tmptr0->ypos ;
	    tmptr->oxpos = tmptr0->oxpos ;
	    tmptr->oypos = tmptr0->oypos ;

	    move( orient ) ; 

	    point( &tmptr0->xpos , &tmptr0->ypos ) ;

	    if( (height % 2 != 0 && (orient == 4 || orient == 6))||
		(length % 2 != 0 && (orient == 2 || orient == 3))){
		tmptr0->xpos++  ;
	    }
	    tmptr0->oxpos = tmptr0->xpos ;
	    if( (height % 2 != 0 && (orient == 1 || orient == 3))||
		(length % 2 != 0 && (orient == 4 || orient == 7))){
		tmptr0->ypos++ ;
	    }
	    tmptr0->oypos = tmptr0->ypos ;
	    for( termnum = 2 ; termnum <= ptr->numterms ; termnum++ ) {
		tmptr0 = tmptr0->nextterm ;
		tmptr  = tmptr->nextterm = (TERMBOXPTR) malloc(
					    sizeof( TERMBOX ) ) ;
		tmptr->nextterm = TERMNULL ;
		tmptr->terminal = tmptr0->terminal ;
		tmptr->xpos = tmptr0->xpos ;
		tmptr->ypos = tmptr0->ypos ;
		tmptr->oxpos = tmptr0->oxpos ;
		tmptr->oypos = tmptr0->oypos ;

		move( orient ) ; 

		point( &tmptr0->xpos , &tmptr0->ypos ) ;

		if( (height % 2 != 0 && (orient == 4 || orient == 6))||
		    (length % 2 != 0 && (orient == 2 || orient == 3))){
		    tmptr0->xpos++  ;
		}
		tmptr0->oxpos = tmptr0->xpos ;
		if( (height % 2 != 0 && (orient == 1 || orient == 3))||
		    (length % 2 != 0 && (orient == 4 || orient == 7))){
		    tmptr0->ypos++ ;
		}
		tmptr0->oypos = tmptr0->ypos ;
	    }
	}

	/*  Now the sites for the soft cells */

	if( ptr->numsites != 0 ) {
	    siteptr0 = ptr->config[0]->siteLocArray ;
	    siteptr = ptr->config[aorient]->siteLocArray = (LOCBOX *) 
		      malloc( (ptr->numsites + 1) * sizeof( LOCBOX ) ) ;
	    for( site = 1 ; site <= ptr->numsites ; site++ ) {
		siteptr[site].xpos = siteptr0[site].xpos ;
		siteptr[site].ypos = siteptr0[site].ypos ;
		siteptr[site].oxpos = siteptr0[site].oxpos ;
		siteptr[site].oypos = siteptr0[site].oypos ;

		move( orient ) ; 

		point( &(siteptr0[ site ].xpos) , 
		       &(siteptr0[ site ].ypos) );

		if( (height % 2 != 0 && (orient == 4 || orient == 6))||
		    (length % 2 != 0 && (orient == 2 || orient == 3))){
		    siteptr0[ site ].xpos++ ;
		}
		siteptr0[site].oxpos = siteptr0[site].xpos ;
		if( (height % 2 != 0 && (orient == 1 || orient == 3))||
		    (length % 2 != 0 && (orient == 4 || orient == 7))){
		    siteptr0[ site ].ypos++ ;
		}
		siteptr0[site].oypos = siteptr0[site].ypos ;
	    }
	}
    }
}

/* ************************************************** */

for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    ptr = cellarray[ cell ]  ;
    height = ptr->config[0]->top - ptr->config[0]->bottom ;
    length = ptr->config[0]->right - ptr->config[0]->left ;

    for( orient = 1 ; orient <= 7 ; orient++ ) {
	if( orient == ptr->orient ) {
	    continue ;
	}
	if( ptr->orientList[orient] == 0 ) {
	    continue ;
	}
	tptr0 = ptr->config[ 0 ] ;
	tptr  = ptr->config[ orient ] = (TILEBOXPTR) malloc(
					    sizeof( TILEBOX ) ) ;
	tptr->nexttile = TILENULL      ;
	tptr->left     = tptr0->left   ;
	tptr->right    = tptr0->right  ;
	tptr->bottom   = tptr0->bottom ;
	tptr->top      = tptr0->top    ;
	tptr->termptr  = TERMNULL      ;

	switch( orient ) {
	    case 1 :  
		tptr->lweight = tptr0->lweight ;
		tptr->rweight = tptr0->rweight ;
		tptr->bweight = tptr0->tweight ;
		tptr->tweight = tptr0->bweight ;
		break ;
	    case 2 :
		tptr->lweight = tptr0->rweight ;
		tptr->rweight = tptr0->lweight ;
		tptr->bweight = tptr0->bweight ;
		tptr->tweight = tptr0->tweight ;
		break ;
	    case 3 :
		tptr->lweight = tptr0->rweight ;
		tptr->rweight = tptr0->lweight ;
		tptr->bweight = tptr0->tweight ;
		tptr->tweight = tptr0->bweight ;
		break ;
	    case 4 :
		tptr->lweight = tptr0->tweight ;
		tptr->rweight = tptr0->bweight ;
		tptr->bweight = tptr0->rweight ;
		tptr->tweight = tptr0->lweight ;
		break ;
	    case 5 :
		tptr->lweight = tptr0->bweight ;
		tptr->rweight = tptr0->tweight ;
		tptr->bweight = tptr0->lweight ;
		tptr->tweight = tptr0->rweight ;
		break ;
	    case 6 :
		tptr->lweight = tptr0->tweight ;
		tptr->rweight = tptr0->bweight ;
		tptr->bweight = tptr0->lweight ;
		tptr->tweight = tptr0->rweight ;
		break ;
	    case 7 :
		tptr->lweight = tptr0->bweight ;
		tptr->rweight = tptr0->tweight ;
		tptr->bweight = tptr0->rweight ;
		tptr->tweight = tptr0->lweight ;
		break ;
	}

	move( orient ) ;
	rect( &tptr->left , &tptr->bottom , &tptr->right , 
						    &tptr->top ) ;
	tptr->oleft   = tptr->left   ;
	tptr->oright  = tptr->right  ;
	tptr->obottom = tptr->bottom ;
	tptr->otop    = tptr->top    ;

	if( (height % 2 != 0 && (orient == 4 || orient == 6))||
	    (length % 2 != 0 && (orient == 2 || orient == 3))){

	    tptr->left++  ;
	    tptr->right++ ;
	    tptr->oleft++  ;
	    tptr->oright++ ;
	}
	if( (height % 2 != 0 && (orient == 1 || orient == 3))||
	    (length % 2 != 0 && (orient == 4 || orient == 7))){

	    tptr->bottom++ ;
	    tptr->top++    ;
	    tptr->obottom++ ;
	    tptr->otop++    ;
	}
	for( tilenum = 1 ; tilenum <= ptr->numtiles ; tilenum++ ) {
	    tptr0 = tptr0->nexttile ;
	    tptr  = tptr->nexttile = (TILEBOXPTR) malloc(
					    sizeof( TILEBOX ) ) ;
	    tptr->nexttile = TILENULL      ;
	    tptr->left     = tptr0->left   ;
	    tptr->right    = tptr0->right  ;
	    tptr->bottom   = tptr0->bottom ;
	    tptr->top      = tptr0->top    ;
	    tptr->termptr  = TERMNULL      ;

	    switch( orient ) {
		case 1 :  
		    tptr->lweight = tptr0->lweight ;
		    tptr->rweight = tptr0->rweight ;
		    tptr->bweight = tptr0->tweight ;
		    tptr->tweight = tptr0->bweight ;
		    break ;
		case 2 :
		    tptr->lweight = tptr0->rweight ;
		    tptr->rweight = tptr0->lweight ;
		    tptr->bweight = tptr0->bweight ;
		    tptr->tweight = tptr0->tweight ;
		    break ;
		case 3 :
		    tptr->lweight = tptr0->rweight ;
		    tptr->rweight = tptr0->lweight ;
		    tptr->bweight = tptr0->tweight ;
		    tptr->tweight = tptr0->bweight ;
		    break ;
		case 4 :
		    tptr->lweight = tptr0->tweight ;
		    tptr->rweight = tptr0->bweight ;
		    tptr->bweight = tptr0->rweight ;
		    tptr->tweight = tptr0->lweight ;
		    break ;
		case 5 :
		    tptr->lweight = tptr0->bweight ;
		    tptr->rweight = tptr0->tweight ;
		    tptr->bweight = tptr0->lweight ;
		    tptr->tweight = tptr0->rweight ;
		    break ;
		case 6 :
		    tptr->lweight = tptr0->tweight ;
		    tptr->rweight = tptr0->bweight ;
		    tptr->bweight = tptr0->lweight ;
		    tptr->tweight = tptr0->rweight ;
		    break ;
		case 7 :
		    tptr->lweight = tptr0->bweight ;
		    tptr->rweight = tptr0->tweight ;
		    tptr->bweight = tptr0->rweight ;
		    tptr->tweight = tptr0->lweight ;
		    break ;
	    }

	    move( orient ) ;
	    rect( &tptr->left, &tptr->bottom, &tptr->right , 
						    &tptr->top ) ;
	    tptr->oleft   = tptr->left   ;
	    tptr->oright  = tptr->right  ;
	    tptr->obottom = tptr->bottom ;
	    tptr->otop    = tptr->top    ;

	    if( (height % 2 != 0 && (orient == 4 || orient == 6))||
		(length % 2 != 0 && (orient == 2 || orient == 3))){

		tptr->left++  ;
		tptr->right++ ;
		tptr->oleft++  ;
		tptr->oright++ ;
	    }

	    if( (height % 2 != 0 && (orient == 1 || orient == 3))||
		(length % 2 != 0 && (orient == 4 || orient == 7))){

		tptr->bottom++ ;
		tptr->top++    ;
		tptr->obottom++ ;
		tptr->otop++    ;
	    }
	}
    }
}

/*  And NOW for the fixed terminals */

for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    ptr = cellarray[ cell ] ;
    if( ptr->numterms == 0 ) {
	continue ;
    }
    height = ptr->config[0]->top - ptr->config[0]->bottom ;
    length = ptr->config[0]->right - ptr->config[0]->left ;

    for( orient = 1 ; orient <= 7 ; orient++ ) {
	if( orient == ptr->orient ) {
	    continue ;
	}
	if( ptr->orientList[orient] == 0 ) {
	    continue ;
	}
	tmptr0 = ptr->config[ 0 ]->termptr ;
	tmptr = ptr->config[ orient ]->termptr = (TERMBOXPTR) 
				      malloc( sizeof( TERMBOX ) ) ;
	tmptr->nextterm = TERMNULL         ;
	tmptr->terminal = tmptr0->terminal ;
	tmptr->xpos     = tmptr0->xpos     ;
	tmptr->ypos     = tmptr0->ypos     ;

	move( orient ) ;
	point( &tmptr->xpos , &tmptr->ypos ) ;

	tmptr->oxpos = tmptr->xpos ;
	tmptr->oypos = tmptr->ypos ;

	if( (height % 2 != 0 && (orient == 4 || orient == 6))||
	    (length % 2 != 0 && (orient == 2 || orient == 3))){

	    tmptr->xpos++  ;
	    tmptr->oxpos++  ;
	}
	if( (height % 2 != 0 && (orient == 1 || orient == 3))||
	    (length % 2 != 0 && (orient == 4 || orient == 7))){

	    tmptr->ypos++ ;
	    tmptr->oypos++ ;
	}
	for( termnum = 2 ; termnum <= ptr->numterms ; termnum++ ) {
	    tmptr0 = tmptr0->nextterm ;
	    tmptr  = tmptr->nextterm = (TERMBOXPTR) malloc(
					    sizeof( TERMBOX ) ) ;
	    tmptr->nextterm = TERMNULL ;
	    tmptr->terminal = tmptr0->terminal ;
	    tmptr->xpos     = tmptr0->xpos ;
	    tmptr->ypos     = tmptr0->ypos ;

	    move( orient ) ;
	    point( &tmptr->xpos , &tmptr->ypos ) ;

	    tmptr->oxpos = tmptr->xpos ;
	    tmptr->oypos = tmptr->ypos ;

	    if( (height % 2 != 0 && (orient == 4 || orient == 6))||
		(length % 2 != 0 && (orient == 2 || orient == 3))){

		tmptr->xpos++  ;
		tmptr->oxpos++  ;
	    }
	    if( (height % 2 != 0 && (orient == 1 || orient == 3))||
		(length % 2 != 0 && (orient == 4 || orient == 7))){

		tmptr->ypos++ ;
		tmptr->oypos++ ;
	    }
	}
    }
}

/*  Now the sites for the soft cells */

for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    ptr = cellarray[ cell ] ;
    if( ptr->softflag == 0 ) {
	continue ;
    }
    if( ptr->numsites == 0 ) {
	continue ;
    }
    height = ptr->config[0]->top - ptr->config[0]->bottom ;
    length = ptr->config[0]->right - ptr->config[0]->left ;

    siteptr0 = ptr->config[ 0 ]->siteLocArray ;
    for( orient = 1 ; orient <= 7 ; orient++ ) {
	if( orient == ptr->orient ) {
	    continue ;
	}
	if( ptr->orientList[orient] == 0 ) {
	    continue ;
	}
	siteptr = ptr->config[ orient ]->siteLocArray = (LOCBOX *) 
	      malloc( (ptr->numsites + 1) * sizeof( LOCBOX ) ) ;

	for( site = 1 ; site <= ptr->numsites ; site++ ) {
	    siteptr[ site ].xpos = siteptr0[ site ].xpos ;
	    siteptr[ site ].ypos = siteptr0[ site ].ypos ;

	    move( orient ) ;

	    point( &(siteptr[ site ].xpos) , 
		   &(siteptr[ site ].ypos) );

	    siteptr[ site ].oxpos = siteptr[ site ].xpos ;
	    siteptr[ site ].oypos = siteptr[ site ].ypos ;

	    if( (height % 2 != 0 && (orient == 4 || orient == 6))||
		(length % 2 != 0 && (orient == 2 || orient == 3))){
		siteptr[ site ].xpos++ ;
		siteptr[ site ].oxpos++ ;
	    }
	    if( (height % 2 != 0 && (orient == 1 || orient == 3))||
		(length % 2 != 0 && (orient == 4 || orient == 7))){
		siteptr[ site ].ypos++ ;
		siteptr[ site ].oypos++ ;
	    }
	}
    }
}

delHtab() ;
for( net = 1 ; net <= numnets ; net++ ) {
    for( netptr = netarray[ net ]->netptr ; netptr != NETNULL ;
				    netptr = netptr->nextterm ) {
	termarray[ netptr->terminal ] = (TERMNETSPTR) malloc(
					sizeof( TERMNETS ) ) ;
	termarray[ netptr->terminal ]->net = net ;
	termarray[ netptr->terminal ]->termptr = netptr ;
    }
}

return ;
}
