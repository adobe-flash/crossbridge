#include "custom.h"
extern int attpercell ;
extern int bigcell ;
extern int toobig ;
extern int attmax ;
extern int doChannelGraph ;
extern int doGlobalRoute ;
extern int doCompaction ;
extern int bbleft , bbright , bbbottom , bbtop ;
extern double Tsave ;
int finalShot ;
int redoFlag ;
int TWsignal ;

extern int checkpen(void);
extern void addpins(void);
extern void doborder(void);
extern void subpins(void);
extern void gpass2( FILE *fp );
extern void grdcell( FILE *fp );
extern void rebin(int flag);
extern int findcost(void);
extern void outgeo(void);
extern void outpin(void);
extern void fixpenal(void);
extern void gmain(void);
extern void rmain(void);
extern void scrappin(void);
extern void wirecosts( int *withPads , int *withOutPads );
extern void initbb(void);
extern void utemp(void);
extern void finalpin(void);
extern void prboard(void);
extern void outbig(void);
extern void outsmall(void);
extern void output(void);
extern void savewolf(void);

void prepOutput(void);

void finalout(void)
{
FILE *fp ;
int c , withPads , withOutPads ;
char filename[1024] ;
char command[128] ;
double Tscale , bound ;

TWsignal = 0 ;

Tscale = Tsave / 100000.0 ;
bound = log10( Tsave ) ;
if( bound > 6.0 ) {
    bound = bound - 1.0 ;
} else {
    bound = 5.0 ;
}
bound = pow( 4.0 , bound ) ;

redoFlag = 0 ;

prepOutput() ;

sprintf(filename, "%s.cel" , cktName ) ;
fp = fopen( filename , "r") ;

gpass2( fp ) ;
grdcell( fp ) ;
fclose( fp ) ;

overlap  = goverlap  ;
overlapf = goverlapf ;
overlapx = goverlapx ;
rebin(0) ;

lapFactor *= 2.0 ;
offset = (int)( 4.0 / lapFactor ) ;
fprintf(fpo,"\n\nNew Value for lapFactor: %f\n", lapFactor );
fprintf(fpo,"New Value for offset: %d\n\n", offset );

funccost = findcost() ;
outgeo() ;
outpin() ;

if( checkpen() > 0 ) {

    fixpenal() ;
    redoFlag = 1 ;
    prepOutput() ;
    rebin(0) ;

    funccost = findcost() ;
    outgeo() ;
    outpin() ;
    if( checkpen() > 0 ) {
	fprintf(fpo,"TimberWolf failed to resolve overlap problem.\n\n");
	fflush(fpo);
	return;
    } else {
	fprintf(fpo,"\n\nPotential Overlap Problem Averted\n\n");
	fflush(fpo);
    }
}

if( doCompaction > 0 ) {
    finalShot = 1 ;
    gmain() ;
    rmain() ;
    redoFlag = 0 ;
    addpins() ;

    sprintf( command , "cp %s.cfs cfss1", cktName ) ;
    system( command ) ;
    sprintf( command , "cp %s.cfb cfbb1", cktName ) ;
    system( command ) ;

    for( c = 1 ; c <= doCompaction ; c++ ) {

	finalShot = c ;
	if( c == doCompaction ) {
	    lapFactor *= 2.0 ;
	    offset = (int)( 4.0 / lapFactor ) ;
	    fprintf(fpo,"\n\nNew Value for lapFactor: %f\n", lapFactor );
	    fprintf(fpo,"New Value for offset: %d\n\n", offset );
	}

	doborder() ;
	rebin(1) ;

	if( c == 1 ) {
	    scrappin() ;
	    attmax  = 50 * numcells ;
	    bigcell = numcells * 11 ;
	    toobig = bigcell + 1 ;
	}

	/*
	if( c == 1 ) {
	    reconfigSides( 0.02 ) ;
	}
	if( c == doCompaction ) {
	    reconfigSides( 0.05 ) ;
	}
	*/
	
	funccost = findcost() ;
	fprintf(fpo,"\n\nCompactor Pass Number: %d\n", c ) ;
	fprintf(fpo,"    begins with:  route cost: %d  overlap: %d  ",
						funccost , penalty ) ;
	fprintf(fpo,"  overfill: %d\n", overfill ) ;

	wirecosts( &withPads , &withOutPads ) ;
	fprintf(fpo,"\n\nCOMPACT #%d: TOTAL Wire Length:\n", c );
	fprintf(fpo,"(before) WITH Pads: %d    WITHOUT Pads: %d\n\n",
					withPads , withOutPads ) ;

	initbb() ;
	if( c == 1 ) {
	    fprintf(fpo,"\n\nCORE Bounding Box: l:%d r:%d b:%d t:%d\n\n",
			    bbleft , bbright , bbbottom , bbtop ) ;
	}
	/* T = 600.0 * Tscale ; */
	T = log10( 0.015 * bound ) / log10( 4.0 ) ;
	T = pow( 10.0 , T ) ;
	utemp() ;
	fprintf(fpo,"    ends with:    route cost: %d  overlap: %d  ",
						funccost , penalty ) ;
	fprintf(fpo,"  overfill: %d\n", overfill ) ;
	fflush( fpo ) ;

	/*
	if( c == doCompaction ) {
	    config3() ;
	    funccost = findcost() ;
	}
	*/

	prepOutput() ;
	outgeo() ;
	outpin() ;

	rebin(0) ;
	if( checkpen() > 0 ) {
	    fixpenal() ;
	    redoFlag = 1 ;
	    prepOutput() ;
	    rebin(0) ;

	    funccost = findcost() ;
	    outgeo() ;
	    outpin() ;

	    if( checkpen() > 0 ) {
		fprintf(fpo,"TimberWolf failed to resolve overlap ");
		fprintf(fpo,"problem.  \n Will have to die\n\n");
		fflush(fpo);
		return;
	    }
	}
	wirecosts( &withPads , &withOutPads ) ;
	fprintf(fpo,"\n\nCOMPACT #%d: TOTAL Wire Length:\n", c );
	fprintf(fpo,"(after) WITH Pads: %d    WITHOUT Pads: %d\n\n",
					withPads , withOutPads ) ;

	gmain() ;
	rmain() ;
	redoFlag = 0 ;

	sprintf( command , "cp %s.cfs cfss%d", cktName , c + 1) ;
	system( command ) ;
	sprintf( command , "cp %s.cfb cfbb%d", cktName , c + 1) ;
	system( command ) ;
    }
    subpins() ;
    fprintf(fpo,"\n\nCORE Bounding Box: l:%d r:%d b:%d t:%d\n\n",
			    bbleft , bbright , bbbottom , bbtop ) ;
} else {
    if( doChannelGraph ) {
	gmain() ;
    }
    if( doGlobalRoute ) {
	rmain() ;
    }
}

return ;
}




void prepOutput(void)
{

finalpin() ;

prboard () ;
outbig()   ;
outsmall() ;
output()   ;
if( finalShot == 0 ) {
    savewolf() ;
}

return ;
}

