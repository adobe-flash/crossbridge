#include "custom.h"
extern int flips ;
extern int flipp ;
extern int attp  ;

extern int upin( CELLBOXPTR acellptr , int UCpin , int seq , int firstNewSite,
		int lastNewSite );
extern int upinswap(CELLBOXPTR acellptr,int pin1,int pin2,int site1,int site2);

void selectpin( CELLBOXPTR acellptr )
{

CONTENTBOX *SCptr ;
UNCOMBOX *UCptr ;
int i, first, last, firstNewSite, lastNewSite, sum, k, seq ;
int ok, site, ns , side , pin1 , pin2 ;


UCptr = acellptr->unComTerms ;
/*
 *  randomly select an uncommitted pin group or
 *  sequence for transfer to a new site
 */

do {
    i = (int) ( (double) acellptr->numUnComTerms * ( 
			    (double)RAND / (double)0x7fffffff)) + 1;
} while( i == acellptr->numUnComTerms + 1 );

while( UCptr[i].sequence == 0 ) {
    i-- ;
}

if( UCptr[i].numranges > 2 ) {
    do {
	side = (int)( (double) UCptr[i].numranges *
	    ( (double) RAND / (double)0x7fffffff) ) + 1 ;
    } while( side == UCptr[i].numranges + 1 ) ;
} else if( UCptr[i].numranges == 2 ) {
    do {
	side = (int)( (double) UCptr[i].numranges *
	    ( (double) RAND / (double)0x7fffffff) ) + 1 ;
    } while( side == UCptr[i].numranges + 1 ) ;
    if( side == 1 ) {
	side = UCptr[i].range1 ;
    } else {
	side = UCptr[i].range2 ;
    }
} else if( UCptr[i].numranges == 1 ) {
    side = UCptr[i].range1 ;
} else {
    return ;
}
first = acellptr->sideArray[side].firstSite ;
last  = acellptr->sideArray[side].lastSite  ;
ns    = last - first + 1       ;

seq = UCptr[i].sequence ;
ok = 0 ;
SCptr = acellptr->siteContent ;
do {
    sum = 0 ;
    site = (int)((double)ns * ((double) RAND / (double)0x7fffffff));

    firstNewSite = first + site ;
    for( k = firstNewSite; k <= last; k++){
	sum += SCptr[k].capacity;
	if( sum >= seq ) {
	    ok = 1 ;
	    lastNewSite = k ;
	    break ;
	}
    }
} while( !ok ) ;

if( firstNewSite != UCptr[i].site ) {
    if( upin( acellptr, i, seq, firstNewSite, lastNewSite ) ) {
	flips++ ;
	flipp++ ;
    } 
    attp++  ;
}
/* 
 *  Determine if the pins are "group" specified, and if so,
 *  swap two of them
 */
if( UCptr[i].groupflag == 1 && seq >= 2 ) {
    do {
	pin1 = (int)((double)seq * ((double)RAND / 
					    (double)0x7fffffff));
    } while( pin1 == seq ) ;
    do {
	pin2 = (int)((double)seq * ((double)RAND / 
					    (double)0x7fffffff));
    } while( pin2 == seq || pin2 == pin1 ) ;

    if( UCptr[i + pin1].site != UCptr[i + pin2].site ) {
	if( upinswap( acellptr, i + pin1 , i + pin2 ,
		    UCptr[i + pin1].site , UCptr[i + pin2].site )) {
	    flips++ ;
	    flipp++ ;
	} 
	attp++  ;
    }
}
return ;
}
