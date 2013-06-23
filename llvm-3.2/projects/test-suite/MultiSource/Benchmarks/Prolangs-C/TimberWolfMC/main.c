#include "custom.h"
double spot_control ;
int spotPenalty ;
int spotXhash ;
int spotYhash ;
char **spots ;
int numXspots ;
int numYspots ;
int spotSize  ;
extern int finalShot ;
int choose ;
int bigcell  ;
int toobig   ;
extern int totNetLen ;
extern int wireEstimateOnly ;
double chipaspect ;
int iwire, iwirex , iwirey , icost ;
int fwire, fwirex , fwirey , fcost ;
double totFunc ;
double totPen  ;
int cost_only ;
int attpercell ;
int doPlacement ;
int doChannelGraph ;
int doGlobalRoute ;
int doCompaction ;
double Tsave ;
double aveCellSide ;
double analyze() ;
int rangeLimit ;

extern int wireestx( int xc , int yy1 , int yy2 , double factor3 );
extern int wireesty( int yc , int xx1 , int xx2 , double factor3 );
extern void readpar(void);
extern void readcells( FILE *fp );
extern void scrapnet(void);
extern void config1(void);
extern void readnets( FILE *fp );
extern int findcost(void);
extern void initcheck(void);
extern int testloop( int trials );
extern void config2(void);
extern int test2loop( int trials );
extern void utemp(void);
extern void finalout(void);
extern void finalcheck(void);
extern void twstats(void);
extern void gmain(void);
extern void rmain(void);

void TW_oldinput( FILE *fp );
void prepSpots(void);

int main( int argc , char *argv[])
{

FILE *fp , *fopen() ;
char filename[1024] ;
int attempts , padflippers , cell , i , w , h ;
double temp1 , temp2 , fraction , ratio ;
double aveCside , variance , standardDev ;

offset     = 0  ;
lapFactor  = 1.0;

finalShot = 0 ;
maxWeight  = 2  ;
baseWeight = 1  ;
layersFactor = 1 ;
totNetLen  = 0  ;
randVar = -1 ;

if( argc != 2 ) {
    printf("TimberWolf usage:  TimberWolf circuitName <CR> \n");
    exit(0);
}
cktName = (char *)malloc( (strlen( argv[1] ) + 1 ) * sizeof( char ) ) ;
sprintf( cktName , "%s" , argv[1] ) ;

sprintf( filename, "%s.out" , cktName ) ;
if( (fpo = fopen( filename , "w")) == (FILE *) NULL ) {
    printf("can't open %s\n", filename ) ;
    exit (0);
}

fprintf(fpo,"TimberWolfMC dated: June 29 1986\n");

readpar() ;

if( doPlacement ) {
    overlap  = woverlap  ;
    overlapf = woverlapf ;
    overlapx = woverlapx ;

    if( randVar == -1 ) {
	randVar = time(0) ;
    }
    fprintf(fpo,"\nThe rand generator seed was: %d\n\n\n", randVar ) ;
    fflush(fpo);


    sprintf(filename, "%s.cel" , cktName ) ;
    if( (fp = fopen( filename , "r")) == (FILE *) NULL ) {
	fprintf(fpo,"can't open %s\n", filename ) ;
	exit (0);
    } 
    readcells( fp ) ;
    scrapnet() ;
    config1() ;
    bdxlength = blockr - blockl ;
    bdylength = blockt - blockb ;
    fprintf(fpo,"bdxlength:%d    bdylength:%d\n",bdxlength,bdylength);
    fflush(fpo);
    fclose(fp);

    sprintf(filename, "%s.net", cktName ) ;
    if ( (fp = fopen ( filename , "r")) == (FILE *) NULL ) {
	fprintf(fpo,"can't open %s\n", filename ) ;
	exit(0);
    } 
    readnets( fp ) ;

    sprintf(filename, "%s.sav", cktName ) ;
    if( (fp = fopen( filename , "w") ) == NULL ) {
	fprintf(fpo,"can't open %s\n", filename ) ;
	exit(0) ;
    } else {
	fclose(fp) ;
    }


    funccost = findcost() ;
    initcheck() ;

    fprintf(fpo,"\n\n\nTHE ROUTE COST OF THE CURRENT PLACEMENT: %d\n", 
						    funccost ) ;
    fprintf(fpo,"\nTHE PENALTY OF THE CURRENT PLACEMENT: %d\n" , 
							penalty ) ;
    fprintf(fpo,"\nTHE OVERFILL OF THE CURRENT PIN PLACEMENT: %d\n",
							 overfill );
    fflush(fpo);


    ratio = analyze() ;

    if( wireEstimateOnly ) {
	exit(0);
    }
    /*
     *   This portion readjusts after taking into account expected
     *   routing area
     */
    T = 1000000000.0 ;
    bigcell = numcells * 11 ;
    toobig  = bigcell + 1 ;
    attempts = testloop( 100 * numcells ) ;
    totNetLen = (int)( (totFunc / (double) attempts) / ratio ) ;
    i = 0 ;
    for( cell = 1 ; cell <= numcells ; cell++ ) {
	if( cellarray[cell]->softflag == 1 ) {
	    i++ ;
	}
    }
    fraction = (double) i / (double) numcells ;
    temp1 = (double) (totNetLen) / 
		(1.0 + (fraction / (sqrt( (double) numcells )))) ;
    totNetLen = (int) temp1 ;

    config2() ;
    bdxlength = blockr - blockl ;
    bdylength = blockt - blockb ;

    temp2 = ((double) bdxlength * (double) bdylength) / (double) numcells;
    aveCellSide = sqrt( temp2 ) ;
    /*
    temp2 = (double) aveChanWid * 1.8 / aveCellSide ;
    if( temp2 > 0.16 ) {
	temp2 -= 0.14 ;
	totNetLen = (int)( (double) totNetLen * ( 1.0 + 8.0 * temp2 ) );
	config2() ;
	bdxlength = blockr - blockl ;
	bdylength = blockt - blockb ;
	temp2 = ((double) bdxlength * 
				(double) bdylength) / (double) numcells;
	aveCellSide = sqrt( temp2 ) ;
	temp2 = (double) aveChanWid * 1.8 / aveCellSide ;
    }
    */


    fprintf(fpo,"bdxlength:%d    bdylength:%d\n",bdxlength,bdylength);
    funccost = findcost() ;
    fprintf(fpo,"\n\n\n");
    fprintf(fpo,"parameter adjust: route:%d  penalty:%d  overfill:%d\n",
			    funccost, penalty, overfill ) ;
    fflush(fpo);

    bigcell = numcells * 11 ;
    toobig  = bigcell + 1 ;
    attempts = test2loop( 100 * numcells ) ;
    totFunc /= (double) attempts ;
    totPen  /= (double) attempts ;
    lapFactor = 0.40 * totFunc / totPen ;

    temp1 = (double) numnets / (double) numcells ;
    temp2 = ((double) bdxlength * (double) bdylength) / (double) numcells;
    temp2 = sqrt( temp2 ) ;
    T = 100000.0 * ( temp2 / 127.0 ) ;

    fprintf(fpo,"nets.per.cell:%g  ave.cell.side:%g\n",temp1,temp2);

    temp1 = 1.8 * (double) aveChanWid / aveCellSide ;
    if( temp1 <= 0.08 ) {
	temp1 = - log10(temp1) - 0.10 ;
	temp1 = ( 0.001 * pow( 10.0 , temp1 ) ) - 0.02 ;
	if( lapFactor < temp1 ) {
	    lapFactor = temp1 ;
	}
	if( lapFactor < 0.02 ) {
	    lapFactor = 0.02 ;
	}
    } else {
	if( lapFactor < 0.02 ) {
	    lapFactor = 0.02 ;
	}
    }

    fprintf(fpo,"\n\nOVERLAP FACTOR (COMPUTED) : %f\n\n", lapFactor ) ;
    offset    = (int)( 4.0 / lapFactor ) ;

    aveCside = 0.0 ;
    for( cell = 1 ; cell <= numcells ; cell++ ) {
	w = cellarray[cell]->config[ cellarray[cell]->orient ]->right -
	    cellarray[cell]->config[ cellarray[cell]->orient ]->left ;
	h = cellarray[cell]->config[ cellarray[cell]->orient ]->top -
	    cellarray[cell]->config[ cellarray[cell]->orient ]->bottom ;
	w += maxWeight * maxWeight * aveChanWid ;
        h += maxWeight * maxWeight * aveChanWid ;
	aveCside += sqrt( (double)(w * h) ) ;
    }
    aveCside /= (double) numcells ;

    variance = 0.0 ;
    for( cell = 1 ; cell <= numcells ; cell++ ) {
	w = cellarray[cell]->config[ cellarray[cell]->orient ]->right -
	    cellarray[cell]->config[ cellarray[cell]->orient ]->left ;
	h = cellarray[cell]->config[ cellarray[cell]->orient ]->top -
	    cellarray[cell]->config[ cellarray[cell]->orient ]->bottom ;
	w += maxWeight * maxWeight * aveChanWid ;
        h += maxWeight * maxWeight * aveChanWid ;
	variance += ( sqrt( (double)(w * h) ) - aveCside ) *
		    ( sqrt( (double)(w * h) ) - aveCside ) ;
    }
    variance /= (double) numcells ;
    standardDev = sqrt( variance ) ;
    fprintf(fpo,"MEAN AND STANDARD DEVIATION OF SQRT OF CELL AREA:\n");
    fprintf(fpo,"MEAN: %g   STANDARD DEVIATION: %g\n", aveCside, 
							standardDev );
    rangeLimit = (int)( aveCside + 2.0 * standardDev ) ;

    if( T < 100000.0 ) {
	T = 100000.0 ;
    }
    Tsave = T ;
    funccost  = findcost() ;
    if( T > ((double) funccost / 10.0) ) {
	T = (double) funccost / 10.0 ;
	Tsave = T ;
	funccost  = findcost() ;
    }
    fprintf(fpo,"\n\n\nThe New Cost Values after readjustment:\n\n");
    fprintf(fpo,"route:%d  penalty:%d  overfill:%d\n\n\n",
			    funccost, penalty, overfill ) ;
    fflush(fpo);

    sprintf(filename, "%s.res", cktName ) ;
    if( (fp = fopen( filename , "r") ) == NULL ) {
	fprintf(fpo,"restart file: %s  wasn't present\n", filename ) ;
    } else {
	fprintf(fpo,"reading data from %s\n", filename ) ;
	TW_oldinput( fp ) ;
	funccost = findcost() ;
	fprintf(fpo,"\n\n\nTHE ROUTE COST OF THE CURRENT PLACEMENT: %d\n"
						  , funccost ) ;
	fprintf(fpo,"\n\nTHE PENALTY OF THE CURRENT PLACEMENT: %d\n" ,
						     penalty ) ;
	fprintf(fpo,"\n\nTHE OVERFILL OF THE CURRENT PIN PLACEMENT: %d\n",
						     overfill ) ;
    }
    fflush(fpo);


    if( !cost_only ) {
	fprintf(fpo,"\nTimberWolfMC Cell Placement Ready for Action\n\n");

	padflippers = 0 ;
	for( cell = numcells + 1; cell <= numcells + numpads; cell++ ){
	    if( cellarray[cell]->class > 0 ) {
		padflippers++ ;
	    }
	}

	attmax  =  attpercell * (numcells + padflippers) ;
	if( padflippers >= 2 ) {
	    bigcell = (numcells + numpads) * 11 ;
	    choose  = numcells + numpads ;
	} else {
	    bigcell = numcells * 11 ;
	    choose  = numcells ;
	}
	toobig  = bigcell + 1 ;
	prepSpots() ;
	utemp() ;
    }

    finalout() ;

    finalcheck() ;
    twstats() ;
} else if( doChannelGraph ) {
    gmain() ;
    if( doGlobalRoute ) {
	rmain() ;
    }
} else if( doGlobalRoute ) {
    rmain() ;
}
fprintf(fpo,"\n\n************************************ \n\n");
fprintf(fpo,"TimberWolfMC has completed its mission\n");
fprintf(fpo,"\n\n************************************ \n\n");
fclose(fpo);
exit(0);
return 0;
}






void prepSpots(void)
{

TILEBOXPTR tileptr ;
int w , h , s , min , max , x , y , cell ;
int l , r , b , t ;
int lspot , rspot , bspot , tspot ;

min = 10000000 ;
max = -10000000 ;
for( cell = 1 ; cell <= numcells ; cell++ ) {
    w = cellarray[cell]->config[ cellarray[cell]->orient ]->right -
	cellarray[cell]->config[ cellarray[cell]->orient ]->left ;
    h = cellarray[cell]->config[ cellarray[cell]->orient ]->top -
	cellarray[cell]->config[ cellarray[cell]->orient ]->bottom ;
    s = (w < h) ? w : h ;
    min = (s < min) ? s : min ;
    min = (min > 1) ? min : 1 ;
    s = (w > h) ? w : h ;
    max = (s >= max) ? s : max ;
}
spotXhash = blockl - max ;
spotYhash = blockb - max ;
spotSize = ( (min / 4) > ((blockr - blockl) / 2000) ) ?
			    (min / 4) : ((blockr - blockl) / 2000) ;
numXspots = ( (blockr + max - spotXhash) / spotSize ) + 1 ;
numYspots = ( (blockt + max - spotYhash) / spotSize ) + 1 ;
fprintf(fpo,"numXspots:%d       numYspots:%d\n\n",numXspots,numYspots);

spots = (char **) malloc( numXspots * sizeof(char *) ) ;
for( x = 0 ; x < numXspots ; x++ ) {
    spots[x] = (char *) malloc( numYspots * sizeof(char) ) ;
    for( y = 0 ; y < numYspots ; y++ ) {
	spots[x][y] = 0 ;
    }
}

for( cell = 1 ; cell <= numcells ; cell++ ) {
    tileptr = cellarray[cell]->config[ cellarray[cell]->orient ] ;
    l = cellarray[cell]->xcenter + tileptr->left ;
    r = cellarray[cell]->xcenter + tileptr->right ;
    b = cellarray[cell]->ycenter + tileptr->bottom ;
    t = cellarray[cell]->ycenter + tileptr->top ;

    l -= wireestx(l,b,t,tileptr->lweight ) ;
    r += wireestx(r,b,t,tileptr->rweight ) ;
    b -= wireesty(b,l,r,tileptr->bweight ) ;
    t += wireesty(t,l,r,tileptr->tweight ) ;

    lspot = (l - spotXhash) / spotSize ;
    rspot = (r - spotXhash) / spotSize ;
    bspot = (b - spotYhash) / spotSize ;
    tspot = (t - spotYhash) / spotSize ;

    for( x = lspot + 1 ; x < rspot ; x++ ) {
	for( y = bspot + 1 ; y < tspot ; y++ ) {
	    spots[x][y]++ ;
	}
    }
}

lspot = (blockl - spotXhash) / spotSize ;
rspot = (blockr - spotXhash) / spotSize ;
bspot = (blockb - spotYhash) / spotSize ;
tspot = (blockt - spotYhash) / spotSize ;
for( x = 0 ; x < numXspots ; x++ ) {
    for( y = 0 ; y < numYspots ; y++ ) {
	if( x >= lspot && x <= rspot && y >= bspot && y <= tspot ) {
	    continue ;
	}
	spots[x][y]++ ;
    }
}

spotPenalty = 0 ;
for( x = 0 ; x < numXspots ; x++ ) {
    for( y = 0 ; y < numYspots ; y++ ) {
	spotPenalty += (int)(spot_control * 
		((double)(ABS(spots[x][y] - 1))) * ((double)(spotSize)));
    }
}
fprintf(fpo,"Value of Spot Control: %f\n", spot_control ) ;
fprintf(fpo,"Value of Spot Size: %d\n", spotSize ) ;
fprintf(fpo,"Initial Value of Spot Penalty: %d\n\n", spotPenalty ) ;

return ;
}
