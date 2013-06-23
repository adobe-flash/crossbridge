#include "custom.h"
int perim ;
extern int totPins ;
extern double *padspace ;
extern int *fixLRBT ;
typedef struct kbox {
    int cap ;
    int HV ;
    int sp ;
    int x ;
    int y ;
}
KBOX ,
*KBOXPTR ;

KBOXPTR kArray ;

BUSTBOXPTR A , B , R ;

extern void parser( FILE *fp );
extern void pass2( FILE *fp );
extern int perimeter(void);
extern void buster(void);
extern int Hside( int cell , int xx1 , int xx2 , int y , int flag );
extern int Vside( int cell , int x , int yy1 , int yy2 , int flag );
extern int hashfind( char hname[] );
extern int findside( CELLBOXPTR cellptr , int x , int y );
extern void loadside( int side , double factor );
extern void placepin(void);
extern void genorient(void);
extern void watesides( CELLBOXPTR ptr );
extern void setpwates(void );

void readcells( FILE *fp )
{

int i , j , cell , temp ;
int xcenter , ycenter ;
int xpos , ypos , hit , pad , groupflag ;
int sequence , k , lastSite , numsites , unComTerms;
int first , l , m , prevsites , orient , norients , class ;
int firstside , lastside , side , distant , ncorners , corner ;
int x , y , minx , miny , maxx , maxy , xx1 , xx2 , yy1 , yy2 ;
int cellnum , pinctr , netx , tot , totx , toty ;
double aspub , asplb , space ;
char input[1024] ;
CELLBOXPTR ptr ;
TILEBOXPTR tile ;
TERMBOXPTR term ;
NETBOXPTR netptr , saveptr ;

parser( fp ) ;
pass2( fp )  ;
pinctr = 0   ;
perim  = 0   ;

/*  prepare MaxSites sites  */
kArray = (KBOXPTR) malloc( (MaxSites + 1) * sizeof( KBOX ));
for( k = 1 ; k <= MaxSites ; k++ ) {
    kArray[ k ].cap = 0 ;
    kArray[ k ].HV = 0 ;
    kArray[ k ].sp = 0 ;
    kArray[ k ].x = 0 ;
    kArray[ k ].y = 0 ;
}

A = ( BUSTBOXPTR ) malloc( 31 * sizeof( BUSTBOX ) ) ;
B = ( BUSTBOXPTR ) malloc( 31 * sizeof( BUSTBOX ) ) ;
R = ( BUSTBOXPTR ) malloc( 5 * sizeof( BUSTBOX ) ) ;
pSideArray = (PSIDEBOX *) malloc( 31 * sizeof( PSIDEBOX ) ) ;


cell = 0 ;
pad = 0 ;
while( fscanf( fp , " %s " , input ) == 1 ) {

    if( strcmp( input , "cell") == 0 ) {
	if( cell > 0 ) {
	    watesides( ptr ) ;
	}
	ptr = cellarray[ ++cell ] ;
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* cell name */
	ptr->cname = (char *)malloc((strlen(input) + 1) * sizeof(char));
	sprintf( ptr->cname , "%s" , input ) ;

	for( k = 0 ; k <= 30 ; k++ ) {
	    A[k].xc = 0 ;
	    A[k].yc = 0 ;
	}
	for( k = 0 ; k <= 5 ; k++ ) {
	    R[k].xc = 0 ;
	    R[k].yc = 0 ;
	}

	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	    A[ ++A[0].xc ].xc = x ;
	    A[ A[0].xc ].yc = y ;
	}
	ptr->numsides = ncorners ;
	perim += perimeter() ;

	for( k = 1 ; k <= A[0].xc ; k++ ) {
	    if( k < A[0].xc ) {
		if( k % 2 == 1 ) {
		    pSideArray[k].length = ABS(A[k + 1].yc - A[k].yc) ;
		    pSideArray[k].vertical = 1 ;
		    pSideArray[k].pincount = 0 ;
		    pSideArray[k].position = A[k].xc ;
		} else {
		    pSideArray[k].length = ABS(A[k + 1].xc - A[k].xc) ;
		    pSideArray[k].vertical = 0 ;
		    pSideArray[k].pincount = 0 ;
		    pSideArray[k].position = A[k].yc ;
		}
	    } else {
		pSideArray[k].length = ABS(A[1].xc - A[k].xc) ;
		pSideArray[k].vertical = 0 ;
		pSideArray[k].pincount = 0 ;
		pSideArray[k].position = A[k].yc ;
	    }
	}

	for( k = 1 ; k <= A[0].xc ; k++ ) {
	    x = A[k].xc ;
	    y = A[k].yc ;
	    if( k == 1 ) {
		minx = x ;
		miny = y ;
		maxx = x ;
		maxy = y ;
	    } else {
		if( x < minx ) {
		    minx = x ;
		}
		if( x > maxx ) {
		    maxx = x ;
		}
		if( y < miny ) {
		    miny = y ;
		}
		if( y > maxy ) {
		    maxy = y ;
		}
	    }
	}
	tile = ptr->config[ 0 ] = (TILEBOXPTR) malloc(
					    sizeof( TILEBOX ) ) ;
	tile->nexttile = TILENULL ;
	tile->termptr = TERMNULL ;
	tile->siteLocArray = LNULL ;
	xcenter = (maxx + minx) / 2 ;
	ycenter = (maxy + miny) / 2 ;
	tile->left   = tile->oleft   = minx - xcenter ;
	tile->right  = tile->oright  = maxx - xcenter ;
	tile->bottom = tile->obottom = miny - ycenter ;
	tile->top    = tile->otop    = maxy - ycenter ;
	tile->lborder = 0 ;
	tile->rborder = 0 ;
	tile->bborder = 0 ;
	tile->tborder = 0 ;
	ptr->xcenter = xcenter ;
	ptr->ycenter = ycenter ;
	ptr->numtiles = 0 ;
	do {
	    ptr->numtiles++ ;
	    tile = tile->nexttile = (TILEBOXPTR) malloc( 
					sizeof( TILEBOX ) ) ;
	    tile->nexttile = TILENULL ;
	    tile->termptr  = TERMNULL ;
	    tile->siteLocArray = LNULL;
	    buster() ;
	    tile->left   = tile->oleft   = R[1].xc - xcenter ;
	    tile->right  = tile->oright  = R[4].xc - xcenter ;
	    tile->bottom = tile->obottom = R[1].yc - ycenter ;
	    tile->top    = tile->otop    = R[2].yc - ycenter ;
	    tile->lborder = 0 ;
	    tile->rborder = 0 ;
	    tile->bborder = 0 ;
	    tile->tborder = 0 ;
	} while( A[0].xc > 0 ) ;

	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &class ) ;
	ptr->class = class ;
	fscanf( fp , " %d " , &norients ) ;
	fscanf( fp , " %s " , input ) ;
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    fscanf( fp , " %d " , &temp ) ;
	    ptr->orientList[temp] = 1 ;
	    if( orient == 1 ) {
		ptr->orientList[8] = ptr->orient = temp ;
	    }
	}

    } else if( strcmp( input , "pad") == 0 ) {

	if( pad == 0 ) {
	    watesides( ptr ) ;
	}
	ptr = cellarray[ ++cell ] ;
	pad++ ;
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* pad name */
	ptr->cname = (char *)malloc((strlen(input) + 1) * sizeof(char));
	sprintf( ptr->cname , "%s" , input ) ;
	for( k = 0 ; k <= 30 ; k++ ) {
	    A[k].xc = 0 ;
	    A[k].yc = 0 ;
	}
	for( k = 0 ; k <= 5 ; k++ ) {
	    R[k].xc = 0 ;
	    R[k].yc = 0 ;
	}

	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	    A[ ++A[0].xc ].xc = x ;
	    A[ A[0].xc ].yc = y ;
	}

	for( k = 1 ; k <= A[0].xc ; k++ ) {
	    x = A[k].xc ;
	    y = A[k].yc ;
	    if( k == 1 ) {
		minx = x ;
		miny = y ;
		maxx = x ;
		maxy = y ;
	    } else {
		if( x < minx ) {
		    minx = x ;
		}
		if( x > maxx ) {
		    maxx = x ;
		}
		if( y < miny ) {
		    miny = y ;
		}
		if( y > maxy ) {
		    maxy = y ;
		}
	    }
	}
	tile = ptr->config[ 0 ] = (TILEBOXPTR) malloc(
					    sizeof( TILEBOX ) ) ;
	tile->nexttile = TILENULL ;
	tile->termptr = TERMNULL ;
	tile->siteLocArray = LNULL ;
	xcenter = (maxx + minx) / 2 ;
	ycenter = (maxy + miny) / 2 ;
	tile->left   = tile->oleft   = minx - xcenter ;
	tile->right  = tile->oright  = maxx - xcenter ;
	tile->bottom = tile->obottom = miny - ycenter ;
	tile->top    = tile->otop    = maxy - ycenter ;
	tile->lborder = 0 ;
	tile->rborder = 0 ;
	tile->bborder = 0 ;
	tile->tborder = 0 ;
	ptr->xcenter = xcenter ;
	ptr->ycenter = ycenter ;
	ptr->numtiles = 0 ;
	do {
	    ptr->numtiles++ ;
	    tile = tile->nexttile = (TILEBOXPTR) malloc( 
					sizeof( TILEBOX ) ) ;
	    tile->nexttile = TILENULL ;
	    tile->termptr  = TERMNULL ;
	    tile->siteLocArray = LNULL;
	    buster() ;
	    tile->left   = tile->oleft   = R[1].xc - xcenter ;
	    tile->right  = tile->oright  = R[4].xc - xcenter ;
	    tile->bottom = tile->obottom = R[1].yc - ycenter ;
	    tile->top    = tile->otop    = R[2].yc - ycenter ;
	    tile->lborder = 0 ;
	    tile->rborder = 0 ;
	    tile->bborder = 0 ;
	    tile->tborder = 0 ;
	} while( A[0].xc > 0 ) ;

    } else if( strcmp( input , "padside") == 0 ) {

	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "L" ) == 0 ) {
	    ptr->padside = 1 ;
	} else if( strcmp( input , "T" ) == 0 ) {
	    ptr->padside = 2 ;
	} else if( strcmp( input , "R" ) == 0 ) {
	    ptr->padside = 3 ;
	} else if( strcmp( input , "B" ) == 0 ) {
	    ptr->padside = 4 ;
	} else {
	    fprintf(fpo,"padside not specified properly for ");
	    fprintf(fpo,"pad: %s\n", ptr->cname ) ;
	    exit(0);
	}

	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &class ) ;
	ptr->class = class ;
	fscanf( fp , " %d " , &norients ) ;
	fscanf( fp , " %s " , input ) ;
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    fscanf( fp , " %d " , &temp ) ;
	    ptr->orientList[temp] = 1 ;
	    if( orient == 1 ) {
		ptr->orientList[8] = ptr->orient = temp ;
	    }
	}

    } else if( strcmp( input , "sidespace") == 0 ) {
	fscanf( fp , " %lf " , &space ) ;
	if( ptr->padside == 1 ) {
	    fixLRBT[0] = 1 ;	
	    padspace[ pad ] = space ;
	} else if( ptr->padside == 3 ) {
	    fixLRBT[1] = 1 ;	
	    padspace[ pad ] = space ;
	} else if( ptr->padside == 4 ) {
	    fixLRBT[2] = 1 ;	
	    padspace[ pad ] = space ;
	} else if( ptr->padside == 2 ) {
	    fixLRBT[3] = 1 ;	
	    padspace[ pad ] = space ;
	} 

    } else if( strcmp( input , "softcell") == 0 ) {

	if( cell > 0 ) {
	    watesides( ptr ) ;
	}
	ptr = cellarray[ ++cell ] ;
	fscanf( fp , "%d" , &cellnum ) ;
	fscanf( fp , "%s" , input ) ; /* "name" */
	fscanf( fp , "%s" , input ) ; /* cell name */
	ptr->cname = (char *)malloc((strlen(input) + 1) * sizeof(char));
	sprintf( ptr->cname , "%s" , input ) ;

	ptr->softflag = 1 ;

	for( k = 0 ; k <= 30 ; k++ ) {
	    A[k].xc = 0 ;
	    A[k].yc = 0 ;
	    B[k].xc = 0 ;
	    B[k].yc = 0 ;
	}
	for( k = 0 ; k <= 5 ; k++ ) {
	    R[k].xc = 0 ;
	    R[k].yc = 0 ;
	}

	fscanf( fp , " %d " , &ncorners ) ;
	fscanf( fp , " %s " , input ) ;
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    fscanf( fp , " %d %d " , &x , &y ) ;
	    A[ ++A[0].xc ].xc = x ;
	    A[ A[0].xc ].yc = y ;
	    B[ ++B[0].xc ].xc = x ;
	    B[ B[0].xc ].yc = y ;
	}
	ptr->numsides = ncorners ;
	ptr->sideArray = (SIDEBOX *) malloc( (ncorners + 1) *
						sizeof( SIDEBOX ) ) ;
	perim += perimeter() ;

	for( k = 1 ; k <= A[0].xc ; k++ ) {
	    if( k < A[0].xc ) {
		if( k % 2 == 1 ) {
		    pSideArray[k].length = ABS(A[k + 1].yc - A[k].yc) ;
		    pSideArray[k].vertical = 1 ;
		    pSideArray[k].pincount = 0 ;
		    pSideArray[k].position = A[k].xc ;
		} else {
		    pSideArray[k].length = ABS(A[k + 1].xc - A[k].xc) ;
		    pSideArray[k].vertical = 0 ;
		    pSideArray[k].pincount = 0 ;
		    pSideArray[k].position = A[k].yc ;
		}
	    } else {
		pSideArray[k].length = ABS(A[1].xc - A[k].xc) ;
		pSideArray[k].vertical = 0 ;
		pSideArray[k].pincount = 0 ;
		pSideArray[k].position = A[k].yc ;
	    }
	}

	for( k = 1 ; k <= A[0].xc ; k++ ) {
	    x = A[k].xc ;
	    y = A[k].yc ;
	    if( k == 1 ) {
		minx = x ;
		miny = y ;
		maxx = x ;
		maxy = y ;
	    } else {
		if( x < minx ) {
		    minx = x ;
		}
		if( x > maxx ) {
		    maxx = x ;
		}
		if( y < miny ) {
		    miny = y ;
		}
		if( y > maxy ) {
		    maxy = y ;
		}
	    }
	}
	ptr->aspect = (double)(maxy - miny) / (double)(maxx - minx);
	ptr->aspectO = ptr->aspect ;

	tile = ptr->config[ 0 ] = (TILEBOXPTR) malloc(
					    sizeof( TILEBOX ) ) ;
	tile->nexttile = TILENULL ;
	tile->termptr = TERMNULL ;
	tile->siteLocArray = LNULL ;
	xcenter = (maxx + minx) / 2 ;
	ycenter = (maxy + miny) / 2 ;
	tile->left   = tile->oleft   = minx - xcenter ;
	tile->right  = tile->oright  = maxx - xcenter ;
	tile->bottom = tile->obottom = miny - ycenter ;
	tile->top    = tile->otop    = maxy - ycenter ;
	tile->lborder = 0 ;
	tile->rborder = 0 ;
	tile->bborder = 0 ;
	tile->tborder = 0 ;
	ptr->xcenter = xcenter ;
	ptr->ycenter = ycenter ;
	ptr->numtiles = 0 ;
	do {
	    ptr->numtiles++ ;
	    tile = tile->nexttile = (TILEBOXPTR) malloc( 
					sizeof( TILEBOX ) ) ;
	    tile->nexttile = TILENULL ;
	    tile->termptr  = TERMNULL ;
	    tile->siteLocArray = LNULL;
	    buster() ;
	    tile->left   = tile->oleft   = R[1].xc - xcenter ;
	    tile->right  = tile->oright  = R[4].xc - xcenter ;
	    tile->bottom = tile->obottom = R[1].yc - ycenter ;
	    tile->top    = tile->otop    = R[2].yc - ycenter ;
	    tile->lborder = 0 ;
	    tile->rborder = 0 ;
	    tile->bborder = 0 ;
	    tile->tborder = 0 ;
	} while( A[0].xc > 0 ) ;

	numsites = 0 ;
	if( ptr->numUnComTerms != 0 ) {
	    lastSite = 0 ;
	    ptr->unComTerms = (UNCOMBOX *) malloc( 
		(ptr->numUnComTerms + 1) * sizeof( UNCOMBOX ) );
	    for( k = 1 ; k <= B[0].xc ; k++ ) {
		xx1 = B[k].xc ;
		yy1 = B[k].yc ;
		if( k == B[0].xc ) {
		    xx2 = B[1].xc ;
		    yy2 = B[1].yc ;
		} else {
		    xx2 = B[ k + 1 ].xc ;
		    yy2 = B[ k + 1 ].yc ;
		}
		if( yy1 == yy2 ) {  /* side is horizontal */
		    numsites += Hside( cell, xx1, xx2, yy1, 0 );
		} else {
		    numsites += Vside( cell, xx1, yy1, yy2, 0 );
		} 
		ptr->sideArray[k].firstSite = lastSite + 1 ;
		ptr->sideArray[k].lastSite  = numsites ;
		lastSite = numsites ;
	    }
	}
	ptr->numsites = numsites ;
	ptr->siteContent = (CONTENTBOX *) 
	    malloc( (numsites + 1) * sizeof( CONTENTBOX ) ) ;
	ptr->config[0]->siteLocArray = (LOCBOX *) 
		    malloc( (numsites + 1) * sizeof(LOCBOX) ) ;
	if( ptr->numUnComTerms != 0 ) {
	    numsites = 0 ;
	    for( k = 1 ; k <= B[0].xc ; k++ ) {
		xx1 = B[k].xc ;
		yy1 = B[k].yc ;
		if( k == B[0].xc ) {
		    xx2 = B[1].xc ;
		    yy2 = B[1].yc ;
		} else {
		    xx2 = B[ k + 1 ].xc ;
		    yy2 = B[ k + 1 ].yc ;
		}
		prevsites = numsites ;
		if( yy1 == yy2 ) {  /* side is horizontal */
		    numsites += Hside( cell, xx1, xx2, yy1, 1 );
		} else {
		    numsites += Vside( cell, xx1, yy1, yy2, 1 );
		}
		for( j = 1 , l = prevsites + 1 ; l <= numsites ;
						    l++ , j++ ){
		    ptr->siteContent[ l ].HorV = kArray[j].HV ;
		    ptr->siteContent[ l ].span = kArray[j].sp ;
		    ptr->config[0]->siteLocArray[ l ].xpos = 
					  kArray[j].x - xcenter;
		    ptr->config[0]->siteLocArray[ l ].ypos = 
					  kArray[j].y - ycenter;
		    ptr->config[0]->siteLocArray[ l ].oxpos = 
					  kArray[j].x - xcenter;
		    ptr->config[0]->siteLocArray[ l ].oypos = 
					  kArray[j].y - ycenter;
		    ptr->siteContent[ l ].contents = 0 ;
		    ptr->siteContent[ l ].newContents = 0 ;
		    ptr->siteContent[ l ].capacity = 
						kArray[j].cap ;
		}
	    }
	}
	unComTerms = 0 ;

    } else if( strcmp( input , "pin") == 0 ) {

	fscanf( fp , " %s " , input ) ; /* "name"   */
	fscanf( fp , " %s " , input ) ; /* pin name */
	pinnames[++pinctr] = (char *)malloc((strlen(input) + 1) * 
						    sizeof(char) );
	sprintf( pinnames[pinctr] , "%s" , input ) ;
	fscanf( fp , " %s " , input ) ; /* "signal" */
	fscanf( fp , " %s " , input ) ; /* signal name */
	netx = hashfind( input ) ;
	if( netarray[netx]->nname == NULL ) {
	    netarray[netx]->nname = (char *)malloc((strlen(input) + 1) *
						    sizeof(char) );
	    sprintf( netarray[netx]->nname , "%s" , input ) ;
	}
	saveptr = netarray[ netx ]->netptr ;
	netptr = netarray[ netx ]->netptr = (NETBOXPTR) 
					 malloc( sizeof(NETBOX) );
	netptr->nextterm = saveptr ;
	netptr->terminal = pinctr ;
	netptr->xpos     = 0    ;
	netptr->ypos     = 0    ;
	netptr->newx     = 0    ;
	netptr->newy     = 0    ;
	netptr->flag     = 0    ;
	netptr->cell     = cell ;
	netptr->skip     = 0    ;

	fscanf( fp , " %d %d " , &xpos , &ypos ) ;
	if( pad == 0 ) {
	    side = findside( ptr , xpos , ypos ) ;
	    loadside( side , 1.0 ) ;
	}

	tot = 1 ;
	totx = xpos ;
	toty = ypos ;
	xpos -= xcenter ;
	ypos -= ycenter ;
	hit = 0 ;
	if( ptr->softflag == 1 ) {
	    for( m = 1 ; m <= ptr->numsites ; m++ ) {
		if( ptr->config[0]->siteLocArray[m].xpos == xpos ) {
		    /*
		     *  Find the site with the nearest ypos 
		     */
		    hit = 1 ;
		    distant = ABS( ptr->config[0]->
					    siteLocArray[m].ypos -
					    ypos ) ;
		    for( m++ ; m <= ptr->numsites ; m++ ) {
			if(ptr->config[0]->siteLocArray[m].xpos ==
						    xpos ) {
			    if( ABS( ptr->config[0]->
					    siteLocArray[m].ypos -
					ypos ) < distant ) {
				distant = ABS( ptr->config[0]->
					    siteLocArray[m].ypos -
					    ypos ) ;
			    } else {
				break ;
			    }
			} else {
			    break ;
			}
		    }
		    if( ++unComTerms > ptr->numUnComTerms ) {
			fprintf(fpo,"screwup in readcells: ");
			fprintf(fpo,"failed to properly count ");
			fprintf(fpo,"the no. of uncommitted pins\n");
			exit(0);
		    }
		    ptr->unComTerms[ unComTerms ].sequence = 1 ;
		    ptr->unComTerms[ unComTerms ].terminal = pinctr ;
		    ptr->unComTerms[ unComTerms ].numranges = 0 ;
		    ptr->unComTerms[ unComTerms ].range1    = m - 1 ;
		    break ;
		} else if( ptr->config[0]->siteLocArray[m].ypos ==
					    ypos ) {
		    /*
		     *  Find the site with the nearest xpos 
		     */
		    hit = 1 ;
		    distant = ABS( ptr->config[0]->
					    siteLocArray[m].xpos -
					    xpos ) ;
		    for( m++ ; m <= ptr->numsites ; m++ ) {
			if(ptr->config[0]->siteLocArray[m].ypos ==
						    ypos ) {
			    if( ABS( ptr->config[0]->
					    siteLocArray[m].xpos -
					xpos ) < distant ) {
				distant = ABS( ptr->config[0]->
					    siteLocArray[m].xpos -
					    xpos ) ;
			    } else {
				break ;
			    }
			} else {
			    break ;
			}
		    }
		    if( ++unComTerms > ptr->numUnComTerms ) {
			fprintf(fpo,"screwup in readcells: ");
			fprintf(fpo,"failed to properly count ");
			fprintf(fpo,"the no. of uncommitted pins\n");
			exit(0);
		    }
		    ptr->unComTerms[ unComTerms ].sequence = 1 ;
		    ptr->unComTerms[ unComTerms ].terminal = pinctr ;
		    ptr->unComTerms[ unComTerms ].numranges = 0 ;
		    ptr->unComTerms[ unComTerms ].range1    = m - 1 ;
		    break ;
		}
	    }
	}
	if( hit == 0 ) {
	    ptr->numterms++ ;
	    if( ptr->numterms == 1 ) {
		term = ptr->config[ 0 ]->termptr = 
			(TERMBOXPTR) malloc( sizeof( TERMBOX ) ) ;
	    } else {
		term = term->nextterm = (TERMBOXPTR) malloc( 
					   sizeof( TERMBOX ) ) ;
	    }
	    term->terminal = pinctr ;
	    term->nextterm = TERMNULL ;
	    term->xpos  = xpos ;
	    term->ypos  = ypos ;
	    term->oxpos = xpos ;
	    term->oypos = ypos ;
	}

    } else if( strcmp( input , "equiv") == 0 ) {

	fscanf( fp , " %s " , input ) ; /* "name"   */
	fscanf( fp , " %s " , input ) ; /* pin name */
	fscanf( fp , " %d %d " , &xpos , &ypos ) ;
	if( pad == 0 ) {
	    side = findside( ptr , xpos , ypos ) ;
	    loadside( side , 1.0 ) ;
	}
	tot++ ;
	totx += xpos ;
	toty += ypos ;
	term->xpos = (totx / tot ) - xcenter ;
	term->ypos = (toty / tot ) - ycenter ;
	term->oxpos = (totx / tot ) - xcenter ;
	term->oypos = (toty / tot ) - ycenter ;

    } else if( strcmp( input , "asplb") == 0 ) {

	fscanf( fp , "%lf" , &asplb ) ;
	ptr->aspLB = asplb ;

    } else if( strcmp( input , "aspub") == 0 ) {

	fscanf( fp , "%lf" , &aspub ) ;
	ptr->aspUB = aspub ;

	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d " , &class ) ;
	ptr->class = class ;
	fscanf( fp , " %d " , &norients ) ;
	fscanf( fp , " %s " , input ) ;
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    fscanf( fp , " %d " , &temp ) ;
	    ptr->orientList[temp] = 1 ;
	    if( orient == 1 ) {
		ptr->orientList[8] = ptr->orient = temp ;
	    }
	}

    } else if( strcmp( input , "sequence") == 0 ||
				    strcmp( input , "group") == 0 ) {
	ptr->numgroups++ ;
	if( strcmp( input , "group") == 0 ) {
	    groupflag = 1 ;
	} else {
	    groupflag = 0 ;
	}
	fscanf( fp , " %d " , &sequence ) ;
	for( i = 1 ; i <= sequence ; i++ ) {
	    fscanf( fp , " %s " , input ) ; /* "pin"    */
	    fscanf( fp , " %s " , input ) ; /* "name"   */
	    fscanf( fp , " %s " , input ) ; /* pin name */
	    pinnames[++pinctr] = (char *) malloc((strlen(input) + 1) *
						    sizeof(char));
	    sprintf( pinnames[pinctr] , "%s" , input ) ;
	    fscanf( fp , " %s " , input ) ; /* "signal" */
	    fscanf( fp , " %s " , input ) ; /* signal name */
	    netx = hashfind( input ) ;
	    if( netarray[netx]->nname == NULL ) {
		netarray[netx]->nname = (char *) malloc( 
				    (strlen(input) + 1) * sizeof(char));
		sprintf( netarray[netx]->nname, "%s", input ) ;
	    }
	    saveptr = netarray[netx]->netptr ;
	    netptr  = netarray[netx]->netptr = (NETBOXPTR) 
					     malloc( sizeof(NETBOX) );
	    netptr->nextterm = saveptr ;
	    netptr->terminal = pinctr  ;
	    netptr->xpos     = 0       ;
	    netptr->ypos     = 0       ;
	    netptr->newx     = 0       ;
	    netptr->newy     = 0       ;
	    netptr->flag     = 0       ;
	    netptr->cell     = cell    ;
	    netptr->skip     = 0       ;

	    unComTerms++ ;
	    if( i == 1 ) {
		first = unComTerms ;
		ptr->unComTerms[first].sequence = sequence ;
	    } else {
		ptr->unComTerms[unComTerms].sequence = 0 ;
	    }
	    ptr->unComTerms[unComTerms].terminal = pinctr ;
	    ptr->unComTerms[unComTerms].groupflag = groupflag ;
	}
	fscanf( fp , " %s " , input ) ;
	fscanf( fp , " %d %d " , &firstside , &lastside ) ;
	if( firstside < 0 || firstside > B[0].xc ) {
	    fprintf(fpo,"value of firstside out of range\n");
	    fprintf(fpo,"firstside:%d  range:1 to %d\n", firstside,
						    B[0].xc ) ;
	    fprintf(fpo,"current cell is:%d\n", cell ) ;
	    exit(0);
	}
	if( lastside < 0 || lastside > B[0].xc ) {
	    fprintf(fpo,"value of lastside out of range\n");
	    fprintf(fpo,"lastside:%d  range:1 to %d\n", lastside, 
							    B[0].xc ) ;
	    fprintf(fpo,"current cell is:%d\n", cell ) ;
	    exit(0);
	}
	if( firstside == 0 ) {
	    for( i = first ; i <= unComTerms ; i++ ) {
		ptr->unComTerms[i].numranges = ptr->numsides ;
		ptr->unComTerms[i].range1 = 0 ;
	    }
	    for( i = 1 ; i <= ptr->numsides ; i++ ) {
		/* loadside( i , 2 * (double) sequence / 
					    (double) ptr->numsides ) ;
		*/
		loadside( i, (double) sequence / (double) ptr->numsides);
	    }
	} else if( lastside == 0 ) {
	    for( i = first ; i <= unComTerms ; i++ ) {
		ptr->unComTerms[i].numranges = 1 ;
		ptr->unComTerms[i].range1 = firstside ;
	    }
	    loadside( firstside , (double) sequence ) ;
	} else {
	    for( i = first ; i <= unComTerms ; i++ ) {
		ptr->unComTerms[i].numranges = 2 ;
		ptr->unComTerms[i].range1 = firstside ;
		ptr->unComTerms[i].range2 = lastside  ;
	    }
	    loadside( firstside , (double) sequence / 2.0 ) ;
	    loadside( lastside  , (double) sequence / 2.0 ) ;
	}
    }
}

pinsPerLen = (double) totPins / (double) perim ;

setpwates() ;
placepin()  ;
genorient() ;

return ;
}
