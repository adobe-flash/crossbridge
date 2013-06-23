#include "custom.h"

void placepin(void)
{
int i , j , cell ;
int k , firstsite , lastsite , numsites ;
int sum , truth , amount , l , m , n ;
int side , site ;
CELLBOXPTR ptr ;
CONTENTBOX *SCptr ;
UNCOMBOX *UCptr ;

 /*  
     We have yet to find an initial placement for the soft cell
     terminals.  Scan through all the cells and check for
     softflag == 1.  Then those with firstSite == lastSite
     must be placed in that site.  Find a placement for all
     other terminals.
 */

for( cell = 1 ; cell <= numcells ; cell++ ) {
    ptr = cellarray[ cell ] ;
    if( ptr->softflag == 0 || ptr->numsites == 0 ) {
	continue ;
    }
    UCptr = ptr->unComTerms ;
    SCptr = ptr->siteContent ;

    for( i = 1 ; i <= ptr->numUnComTerms ; i++ ) {
	if( UCptr[i].sequence != 0 ) {
	    if( UCptr[i].numranges == 1 ) {
		sum = 0 ;
		n = ptr->sideArray[ UCptr[i].range1 ].firstSite ; 
		for(; n <= ptr->sideArray[ UCptr[i].range1 ].lastSite ; 
				 				n++ ) {
		    sum += SCptr[n].capacity ;
		}
		if( sum < UCptr[i].sequence ) {
		    fprintf(fpo,"DEATH: Sequence was specified which");
		    fprintf(fpo," required more capacity than given\n");
		    fprintf(fpo,"sides could hanlde. Problem cell:%d\n",
								cell ) ;
		    exit(0) ;
		}
	    } else if( UCptr[i].numranges == 2 ) {
		sum = 0 ;
		n = ptr->sideArray[ UCptr[i].range1 ].firstSite ; 
		for(; n <= ptr->sideArray[ UCptr[i].range1 ].lastSite ; 
				 				n++ ) {
		    sum += SCptr[n].capacity ;
		}
		if( sum < UCptr[i].sequence ) {
		    fprintf(fpo,"DEATH: Sequence was specified which");
		    fprintf(fpo," required more capacity than given\n");
		    fprintf(fpo,"sides could hanlde. Problem cell:%d\n",
								cell ) ;
		    exit(0) ;
		}
		sum = 0 ;
		n = ptr->sideArray[ UCptr[i].range2 ].firstSite ; 
		for(; n <= ptr->sideArray[ UCptr[i].range2 ].lastSite ; 
				 				n++ ) {
		    sum += SCptr[n].capacity ;
		}
		if( sum < UCptr[i].sequence ) {
		    fprintf(fpo,"DEATH: Sequence was specified which");
		    fprintf(fpo," required more capacity than given\n");
		    fprintf(fpo,"sides could hanlde. Problem cell:%d\n",
								cell ) ;
		    exit(0) ;
		}
	    } else if( UCptr[i].numranges > 2 ) {
		sum = 0 ;
		n = 1 ;
		for(; n <= ptr->sideArray[ptr->numsides].lastSite; n++){
		    sum += SCptr[n].capacity ;
		}
		if( sum < UCptr[i].sequence ) {
		    fprintf(fpo,"DEATH: Sequence was specified which");
		    fprintf(fpo," required more capacity than given\n");
		    fprintf(fpo,"sides could hanlde. Problem cell:%d\n",
								cell ) ;
		    exit(0) ;
		}
	    }
	    /*
	     *  Strategy:  Randomly select a legal side for the
	     *  sequence.  Then randomly pick a starting site on
	     *  this side such that the sequence fits on the side
	     */
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
	    }
	    if( UCptr[i].numranges == 0 ) {
		firstsite = UCptr[i].range1 ;
		lastsite  = firstsite       ;
		numsites  = 1               ;
	    } else {
		firstsite = ptr->sideArray[side].firstSite ;
		lastsite  = ptr->sideArray[side].lastSite  ;
		numsites  = lastsite - firstsite + 1       ;
	    }

	    do {
		j = (int)( (double)numsites *
			    ( (double)RAND / (double)0x7fffffff) ) ;
		sum = 0 ;
		truth = 1 ;
		for( k = firstsite + j ; ; k++ ) {
		    if( k > lastsite ) {
			truth = 0 ;
			break ;
		    } else {
			sum += SCptr[k].capacity ;
			if( sum >= UCptr[i].sequence ) {
			    break ;
			    /* 
			       We found it. 
			       The sequence can be housed in
			       sites (firstsite + j) thru (k)
			    */
			}
		    }
		}
	    } while( !truth ) ; 

	    /* The sites below are going to be filled to capacity by
	       the sequence beginning with i  */
	    sum = 0 ;
	    l   = i ;
	    for( site = firstsite + j ; site < k ; site++ ) {
		amount = SCptr[site].capacity ;
		sum += amount ;
		SCptr[site].contents += amount ;
		for( m = l ; m < l + amount ; m++ ) {
		    UCptr[m].site = site ;
		}
		l += amount ;
	    }
	    /*  Now we have to add to the contents of site k
		which was partially filled by the sequence
		beginning with i  */
	    amount = UCptr[i].sequence - sum ;
	    SCptr[k].contents += amount ;
	    for( m = l ; m < l + amount ; m++ ) {
		UCptr[m].site = k ;
	    }
	}
    }
}
return ;

}
