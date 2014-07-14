#include "geo.h"

extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);
extern void fulllink(void);
extern void findnodes(void);
extern void changraph(void);
extern void gentwf(void);
extern void readgeo( FILE *fp );
extern void makelink(void);
extern void hprobes(void);
extern void vprobes(void);
extern void xgraph(void);
extern void ygraph(void);
extern void reduceg(void);
extern void printgph(void);

void gmain(void)
{

FILE *fp , *fopen() ;
TNODEPTR tnode ;
char filename[1024] ;
int dummy1 , dummy2 , i ;
FLAREPTR fptr , f2ptr ;
DLINK2PTR hrptr , hr2ptr ;
DLINK1PTR hptr , h2ptr ;


sprintf( filename , "%s.debug" , cktName ) ;
if( (fpdebug = fopen ( filename , "w")) == (FILE *) NULL ) {
    printf("can't open %s\n", filename ) ;
    exit(0);
}

sprintf( filename , "%s.geo" , cktName ) ;
if( (fp = fopen ( filename , "r")) == (FILE *) NULL ) {
    fprintf(fpdebug,"Error: file: %s not present\n", filename );
    exit(0);
}

readgeo( fp ) ;
makelink()    ;
hprobes()     ;
vprobes()     ;

free( Vptrs ) ;
free( Hptrs ) ;
if( Vroot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &Vroot , &tnode , &dummy1 , &dummy2 ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
if( Hroot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &Hroot , &tnode , &dummy1 , &dummy2 ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}


fulllink()  ;
findnodes() ;
changraph() ;
xgraph()    ;
ygraph()    ;
reduceg()   ;
printgph()  ;
gentwf()    ;

/*   ***********************************************    */
/*         THIS SECTION IS CLEAN-UP                     */
/*   ***********************************************    */


free( rectArray ) ;

hrptr = HRlist ;
while( hrptr != (DLINK2PTR) NULL ) {
    hr2ptr = hrptr->next ;
    free( hrptr ) ;
    hrptr = hr2ptr ;
}
hrptr = VRlist ;
while( hrptr != (DLINK2PTR) NULL ) {
    hr2ptr = hrptr->next ;
    free( hrptr ) ;
    hrptr = hr2ptr ;
}

free( LEptrs ) ;
free( BEptrs ) ;
if( LEroot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &LEroot , &tnode , &dummy1 , &dummy2 ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
if( BEroot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &BEroot , &tnode , &dummy1 , &dummy2 ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}

hptr = Hlist ;
while( hptr != (DLINK1PTR) NULL ) {
    h2ptr = hptr->next ;
    free( hptr ) ;
    hptr = h2ptr ;
}

hptr = Vlist ;
while( hptr != (DLINK1PTR) NULL ) {
    h2ptr = hptr->next ;
    free( hptr ) ;
    hptr = h2ptr ;
}

free( Vptrs ) ;
free( Hptrs ) ;
if( Vroot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &Vroot , &tnode , &dummy1 , &dummy2 ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
if( Hroot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &Hroot , &tnode , &dummy1 , &dummy2 ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}

free( VDptrs ) ;
free( HRptrs ) ;
if( VDroot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &VDroot , &tnode , &dummy1 , &dummy2 ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}
if( HRroot != (TNODEPTR) NULL ) {
    for( ; ; ) {
	tpop( &HRroot , &tnode , &dummy1 , &dummy2 ) ;
	if( tnode == (TNODEPTR) NULL ) {
	    break ;
	}
    }
}


hptr = hFixedList ;
while( hptr != (DLINK1PTR) NULL ) {
    h2ptr = hptr->next ;
    free( hptr ) ;
    hptr = h2ptr ;
}
hptr = vFixedList ;
while( hptr != (DLINK1PTR) NULL ) {
    h2ptr = hptr->next ;
    free( hptr ) ;
    hptr = h2ptr ;
}

for( i = 0 ; i <= numberCells + 1 ; i++ ) {
    fptr = xNodules[i].outList ;
    while( fptr != (FLAREPTR) NULL ) {
	free( fptr->eindex ) ;
	f2ptr = fptr->next ;
	free( fptr ) ;
	fptr = f2ptr ;
    }
    fptr = xNodules[i].inList ;
    while( fptr != (FLAREPTR) NULL ) {
	free( fptr->eindex ) ;
	f2ptr = fptr->next ;
	free( fptr ) ;
	fptr = f2ptr ;
    }
    fptr = yNodules[i].outList ;
    while( fptr != (FLAREPTR) NULL ) {
	free( fptr->eindex ) ;
	f2ptr = fptr->next ;
	free( fptr ) ;
	fptr = f2ptr ;
    }
    fptr = yNodules[i].inList ;
    while( fptr != (FLAREPTR) NULL ) {
	free( fptr->eindex ) ;
	f2ptr = fptr->next ;
	free( fptr ) ;
	fptr = f2ptr ;
    }
}
free( xNodules ) ;
free( yNodules ) ;

fclose( fpdebug );
fclose( fp );

return ;
}
