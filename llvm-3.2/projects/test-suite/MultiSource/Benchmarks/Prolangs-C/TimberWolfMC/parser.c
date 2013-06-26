#include "custom.h"
int *fixLRBT ;
double *padspace ;

extern void makebins( int numc );

void parser( FILE *fp )
{

int i , space , cell , test , orient , norients ;
int xpos , ypos , pad ;
int code , class ;
int sequence , firstside , lastside , cellnum ;
int corner , ncorners , x , y , minx , miny , maxx , maxy ;
double aspub , asplb ;
char input[1024] ;
CELLBOXPTR ptr ;


maxterm = 0    ;
cell = 0       ;
pad = 0        ;

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "cell") == 0 ) {
	test = fscanf( fp , "%d" , &cellnum ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input cell # in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input keyword name \n");
	    fprintf(fpo," in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	if( strcmp( input , "name" ) != 0 ) {
	    fprintf(fpo,"Failed to input keyword name \n");
	    fprintf(fpo,"in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input cell name in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	cell++ ;
	code = 3 ;
	test = fscanf( fp , " %d " , &ncorners ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input corners in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( ncorners < 4 ) {
	    fprintf(fpo,"Failed to input at least 4 corners");
	    fprintf(fpo," in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "corners") != 0 ) {
	    fprintf(fpo,"parser failed to find: corners ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    test = fscanf( fp , " %d %d " , &x , &y ) ;
	    if( test != 2 ) {
		fprintf(fpo,"Failed to input corner coordinates\n");
		fprintf(fpo,"in the .cel file; cell:%d\n", cell );
		exit(0);
	    } else {
		if( corner == 1 ) {
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
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "class") != 0 ) {
	    fprintf(fpo,"parser failed to find: class ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d " , &class ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input class in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d " , &norients ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input orientations in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( norients < 1 || norients > 8 ) {
	    fprintf(fpo,"Failed to input orientations between 1 and ");
	    fprintf(fpo,"8 in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "orientations") != 0 ) {
	    fprintf(fpo,"parser failed to find: orientations ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    test = fscanf( fp , " %d " , &x ) ;
	    if( test != 1 ) {
		fprintf(fpo,"Failed to input expected orientations\n");
		fprintf(fpo,"in the .cel file; cell:%d\n", cell );
		exit(0);
	    }
	}
	code = 4 ;
    } else if( strcmp( input , "pin") == 0 ) {
	if( code != 4 && code != 2 ) {
	    fprintf(fpo,"parser unexpectedly found: pin ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	code = 4 ;
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"parser failed to find keyword: name ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( strcmp( input , "name" ) != 0 ) {
	    fprintf(fpo,"parser failed to find keyword: name ");
	    fprintf(fpo,"in the .cel file; instead found:%s\n", input);
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"parser failed to find pin name ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"parser failed to find keyword: signal ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( strcmp( input , "signal" ) != 0 ) {
	    fprintf(fpo,"parser failed to find keyword: signal ");
	    fprintf(fpo,"in the .cel file; instead found:%s\n", input);
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"parser failed to find signal name ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d %d " , &xpos , &ypos );
	if( test != 2 ) {
	    fprintf(fpo,"Failed to input coordinates for a pin\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
    } else if( strcmp( input , "equiv") == 0 ) {
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"parser failed to find keyword: name ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( strcmp( input , "name" ) != 0 ) {
	    fprintf(fpo,"parser failed to find keyword: name ");
	    fprintf(fpo,"in the .cel file; instead found:%s\n", input);
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"parser failed to find pin name ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d %d " , &xpos , &ypos );
	if( test != 2 ) {
	    fprintf(fpo,"Failed to input coordinates for a pin\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
    } else if( strcmp( input , "softcell") == 0 ) {
	test = fscanf( fp , "%d" , &cellnum ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input cell number \n");
	    fprintf(fpo,"in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input keyword name \n");
	    fprintf(fpo,"in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	if( strcmp( input , "name" ) != 0 ) {
	    fprintf(fpo,"Failed to input keyword name \n");
	    fprintf(fpo,"in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input cell name in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	cell++ ;
	code = 1 ;
	test = fscanf( fp , " %d " , &ncorners ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input corners in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( ncorners < 4 ) {
	    fprintf(fpo,"Failed to input at least 4 corners");
	    fprintf(fpo," in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "corners") != 0 ) {
	    fprintf(fpo,"parser failed to find: corners ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    test = fscanf( fp , " %d %d " , &x , &y ) ;
	    if( test != 2 ) {
		fprintf(fpo,"Failed to input corner coordinates\n");
		fprintf(fpo,"in the .cel file; cell:%d\n", cell );
		exit(0);
	    } else {
		if( corner == 1 ) {
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
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "asplb") == 0 ) {
	    test = fscanf( fp , "%lf" , &asplb ) ;
	    if( test != 1 ) {
		fprintf(fpo,"Failed to input asplb in the .cel file\n");
		fprintf(fpo,"cell:%d\n", cell );
		exit(0);
	    }
	} else {
	    fprintf(fpo,"parser failed to find: asplb ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "aspub") == 0 ) {
	    test = fscanf( fp , "%lf" , &aspub ) ;
	    if( test != 1 ) {
		fprintf(fpo,"Failed to input aspub in the .cel file\n");
		fprintf(fpo,"cell:%d\n", cell );
		exit(0);
	    }
	} else {
	    fprintf(fpo,"parser failed to find: aspub ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "class") != 0 ) {
	    fprintf(fpo,"parser failed to find: class ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d " , &class ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input class in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d " , &norients ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input orientations in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( norients < 1 || norients > 8 ) {
	    fprintf(fpo,"Failed to input orientations between 1 and ");
	    fprintf(fpo,"8 in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "orientations") != 0 ) {
	    fprintf(fpo,"parser failed to find: orientations ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    test = fscanf( fp , " %d " , &x ) ;
	    if( test != 1 ) {
		fprintf(fpo,"Failed to input expected orientations\n");
		fprintf(fpo,"in the .cel file; cell:%d\n", cell );
		exit(0);
	    }
	}
	code = 2 ;
    } else if( strcmp( input , "sequence") == 0 ||
				    strcmp( input , "group") == 0 ) {
	if( code != 2 ) {
	    fprintf(fpo,"parser unexpectedly found: sequence/group ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d " , &sequence ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input a seq/group in .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	for( i = 1 ; i <= sequence ; i++ ) {
	    fscanf( fp , " %s " , input ) ;
	    if( strcmp( input , "pin") == 0 ) {
		test = fscanf( fp , "%s" , input ) ;
		if( test != 1 ) {
		    fprintf(fpo,"parser did not find keyword: ");
		    fprintf(fpo,"name in the .cel file\n");
		    fprintf(fpo,"cell:%d\n", cell );
		    exit(0);
		}
		if( strcmp( input , "name" ) != 0 ) {
		    fprintf(fpo,"parser failed to find keyword: ");
		    fprintf(fpo,"name in the .cel file:"); 
		    fprintf(fpo," instead found:%s\n", input);
		    fprintf(fpo,"cell:%d\n", cell );
		    exit(0);
		}
		test = fscanf( fp , "%s" , input ) ;
		if( test != 1 ) {
		    fprintf(fpo,"parser failed to find pin name ");
		    fprintf(fpo,"in the .cel file\n");
		    fprintf(fpo,"cell:%d\n", cell );
		    exit(0);
		}
		test = fscanf( fp , "%s" , input ) ;
		if( test != 1 ) {
		    fprintf(fpo,"parser failed to find keyword: ");
		    fprintf(fpo,"signal in the .cel file\n");
		    fprintf(fpo,"cell:%d\n", cell );
		    exit(0);
		}
		if( strcmp( input , "signal" ) != 0 ) {
		    fprintf(fpo,"parser failed to find keyword: ");
		    fprintf(fpo,"signal in the .cel file;\n");
		    fprintf(fpo," instead found:%s\n", input);
		    fprintf(fpo,"cell:%d\n", cell );
		    exit(0);
		}
		test = fscanf( fp , "%s" , input ) ;
		if( test != 1 ) {
		    fprintf(fpo,"parser failed to find signal ");
		    fprintf(fpo,"name in the .cel file\n");
		    fprintf(fpo,"cell:%d\n", cell );
		    exit(0);
		}
	    } else {
		fprintf(fpo,"parser failed to find: pin ");
		fprintf(fpo,"in the .cel file\n");
		fprintf(fpo,"cell:%d\n", cell );
		exit(0);
	    }
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "side.restriction") != 0 ) {
	    fprintf(fpo,"parser failed to find: side.restriction ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d %d " , &firstside , &lastside ) ;
	if( test != 2 ) {
	    fprintf(fpo,"Failed to input the 2 side.restriction \n");
	    fprintf(fpo,"sides in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
    } else if( strcmp( input , "pad") == 0 ) {
	test = fscanf( fp , "%d" , &cellnum ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input pad # in the .cel file\n");
	    fprintf(fpo,"pad:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input keyword name \n");
	    fprintf(fpo," in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	if( strcmp( input , "name" ) != 0 ) {
	    fprintf(fpo,"Failed to input keyword name \n");
	    fprintf(fpo,"in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , "%s" , input ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input cell name in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	pad++ ;
	code = 3 ;
	test = fscanf( fp , " %d " , &ncorners ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input corners in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( ncorners < 4 ) {
	    fprintf(fpo,"Failed to input at least 4 corners");
	    fprintf(fpo," in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "corners") != 0 ) {
	    fprintf(fpo,"parser failed to find: corners ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	for( corner = 1 ; corner <= ncorners ; corner++ ) {
	    test = fscanf( fp , " %d %d " , &x , &y ) ;
	    if( test != 2 ) {
		fprintf(fpo,"Failed to input corner coordinates\n");
		fprintf(fpo,"in the .cel file; cell:%d\n", cell );
		exit(0);
	    } else {
		if( corner == 1 ) {
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
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "padside") == 0 ) {
	    test = fscanf( fp , " %s " , input ) ;
	    if( test != 1 ) {
		fprintf( fpo, "Failed to input padside ");
		fprintf( fpo, "in the .cel file\n");
		fprintf( fpo, "Current pad: %d\n", pad ) ;
		exit(0);
	    }
	} else {
	    fprintf( fpo, "Failed to find keyword padside ");
	    fprintf( fpo, "for a pad\n") ;
	    fprintf( fpo, "Current pad: %d\n", pad ) ;
	    exit(0) ;
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "class") != 0 ) {
	    fprintf(fpo,"parser failed to find: class ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d " , &class ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input class in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	test = fscanf( fp , " %d " , &norients ) ;
	if( test != 1 ) {
	    fprintf(fpo,"Failed to input orientations in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	if( norients < 1 || norients > 8 ) {
	    fprintf(fpo,"Failed to input orientations between 1 and ");
	    fprintf(fpo,"8 in the .cel file; cell:%d\n", cell );
	    exit(0);
	}
	fscanf( fp , " %s " , input ) ;
	if( strcmp( input , "orientations") != 0 ) {
	    fprintf(fpo,"parser failed to find: orientations ");
	    fprintf(fpo,"in the .cel file\n");
	    fprintf(fpo,"cell:%d\n", cell );
	    exit(0);
	}
	for( orient = 1 ; orient <= norients ; orient++ ) {
	    test = fscanf( fp , " %d " , &x ) ;
	    if( test != 1 ) {
		fprintf(fpo,"Failed to input expected orientations\n");
		fprintf(fpo,"in the .cel file; cell:%d\n", cell );
		exit(0);
	    }
	}
	code = 4 ;
    } else if( strcmp( input , "sidespace") == 0 ) {
	test = fscanf( fp , " %d " , &space ) ;
    } else {
	fprintf(fpo,"found unparsable keyword ");
	fprintf(fpo," %s " , input ); 
	fprintf(fpo,"in the .cel file\n");
	fprintf(fpo,"cell:%d\n", cell );
	exit(0);
    }
}

numcells = cell ;
numpads = pad ;

rewind( fp ) ;

makebins( numcells ) ;

cellarray = (CELLBOXPTR *) malloc( (5 + numcells + numpads) *
					 sizeof( CELLBOXPTR ) ) ;
for( cell = 1 ; cell <= numcells + numpads + 4 ; cell++ ) {

    ptr = cellarray[ cell ] = ALLOC( CELLBOX ) ;

    ptr->xcenter    = 0 ;
    ptr->ycenter    = 0 ;
    ptr->orient     = 0 ;
    ptr->numtiles   = 0 ;
    ptr->numterms   = 0 ;
    ptr->numsites   = 0 ;
    ptr->numgroups  = 0 ;
    ptr->softflag   = 0 ;
    ptr->padside    = 0 ;
    ptr->numUnComTerms = 0 ;
    ptr->aspect = 1.0 ;
    ptr->aspUB = 1.0 ;
    ptr->aspLB = 1.0 ;

    for( i = 0 ; i < 8 ; i++ ) {
	ptr->config[ i ] = TILENULL ;
	ptr->orientList[i] = 0 ;
    }
}
padspace = (double *) malloc( (numpads + 1) * sizeof(double) ) ;
for( i = 1 ; i <= numpads ; i++ ) {
    padspace[i] = 0.0 ;
}
fixLRBT  = (int *) malloc( 4 * sizeof( int ) ) ;
fixLRBT[0] = 0 ;
fixLRBT[1] = 0 ;
fixLRBT[2] = 0 ;
fixLRBT[3] = 0 ;
return ;
}
