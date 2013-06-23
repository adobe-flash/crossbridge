#include "custom.h"
extern int ecount ;

extern void point( int *x , int *y );
extern void move( int moveType );
extern void delHtab(void);

void ggenorien(void)
{

int cell , orient , termnum , aorient ;
int height , length , net , terminal , xpos , ypos , pin ;
CELLBOXPTR ptr ;
TERMBOXPTR tmptr0 , tmptr , cptr , koptr , term ;
NETBOXPTR netptr ;
TERMNETSPTR termptr ;


for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    ptr = cellarray[ cell ] ;
    if( ptr->numterms == 0 || ptr->softflag == 1 ) {
	continue ;
    }
    height = ptr->config[0]->top - ptr->config[0]->bottom ;
    length = ptr->config[0]->right - ptr->config[0]->left ;

    for( orient = 1 ; orient <= 7 ; orient++ ) {
	if( ptr->config[orient] == TILENULL ) {
	    continue ;
	}
	cptr = ptr->config[orient]->termptr ;
	ptr->config[orient]->termptr = TERMNULL ;
	if( cptr != TERMNULL ) {
	    koptr = cptr ;
	    while( cptr->nextterm != TERMNULL ) {
		cptr = cptr->nextterm ;
		free( koptr ) ;
		koptr = cptr ;
	    }
	    free( cptr ) ;
	}
    }
    aorient = ptr->orientList[8] ;
    if( aorient != 0 ) {
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
	tmptr0 = ptr->config[0]->termptr ;
	tmptr = ptr->config[aorient]->termptr = (TERMBOXPTR) 
					  malloc( sizeof( TERMBOX ) ) ;
	tmptr->nextterm = TERMNULL ;
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
    height = ptr->config[0]->top - ptr->config[0]->bottom ;
    length = ptr->config[0]->right - ptr->config[0]->left ;

    for( orient = 1 ; orient <= 7 ; orient++ ) {
	if( orient == ptr->orientList[8] ) {
	    continue ;
	}
	if( ptr->orientList[orient] == 0 ) {
	    continue ;
	}
	tmptr0 = ptr->config[0]->termptr ;
	tmptr = ptr->config[orient]->termptr = (TERMBOXPTR) 
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

delHtab() ;
for( net = 1 ; net <= numnets ; net++ ) {
    for( netptr = netarray[ net ]->netptr ; netptr != NETNULL ;
				    netptr = netptr->nextterm ) {
	if( netptr->terminal > maxterm ) {
	    termarray[ netptr->terminal ] = (TERMNETSPTR) malloc(
					    sizeof( TERMNETS ) ) ;
	    termarray[ netptr->terminal ]->net = net ;
	    termarray[ netptr->terminal ]->termptr = netptr ;
	}
    }
}

for( cell = 1 ; cell <= numcells + numpads ; cell++ ) {
    ptr = cellarray[ cell ] ;
    if( ptr->softflag == 0 ) {
	term = ptr->config[ptr->orient]->termptr ;
	for( ; term != TERMNULL ; term = term->nextterm ) {
	    termptr = termarray[term->terminal ];
	    termptr->termptr->xpos = term->xpos + ptr->xcenter ;
	    termptr->termptr->ypos = term->ypos + ptr->ycenter ;
	}
    } else {
	for( pin = 1 ; pin <= ptr->numUnComTerms ; pin++ ) {
	   terminal = ptr->unComTerms[pin].terminal ;
	   xpos = ptr->unComTerms[pin].finalx ;
	   ypos = ptr->unComTerms[pin].finaly ;
	   termptr = termarray[terminal] ;
	   termptr->termptr->xpos = xpos + ptr->xcenter ;
	   termptr->termptr->ypos = ypos + ptr->ycenter ;
	}
    }
}
maxterm += ecount ;
return ;
}
