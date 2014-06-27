#include "custom.h"
#include "23tree.h"
int *how_many;

extern void tdelete( TNODEPTR *root , int value , int property );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);
extern double wireratio( int numofcells, double cellspernet, double netsperd,
			double dnetspercell );

void qsortx(char *base, int n, int size);
void qst(char *base, char *max);

double analyze(void)
{

int **number , i , net , net1 , net2 , num , cell ;
int *count , different , cnum , c2num , *arraynet ;
int num_nets , tot_cels ;
double C , C1 , ratio ;
#ifdef notdef
double C2 , C3;
#endif
NETBOXPTR netptr ;



count  = (int *) malloc( (1 + numcells) * sizeof( int ) ) ;
number = (int **) malloc( (1 + numnets) * sizeof( int *) ) ;
how_many = (int *) malloc( (1 + numnets) * sizeof( int ) ) ;
arraynet = (int *) malloc( (1 + numnets) * sizeof( int ) ) ;
for( net = 0 ; net <= numnets ; net++ ) {
    number[net] = (int *) malloc( (1 + numcells) * sizeof(int) ) ;
}

for( net = 1 ; net <= numnets ; net++ ) {
    for( cell = 0 ; cell <= numcells ; cell++ ) {
	count[cell] = 0 ;
	number[net][cell] = 0 ;
    }
    netptr = netarray[net]->netptr ;
    for( ; netptr != (NETBOXPTR) NULL ; netptr = netptr->nextterm ){
	if( netptr->cell <= numcells ) {
	    count[netptr->cell] = 1 ;
	}
    }
    /*
     *  I would like to find the number of distinct nets
     */
    for( cell = 1 ; cell <= numcells ; cell++ ) { 
	if( count[cell] == 1 ) {
	    number[net][ ++number[net][0] ] = cell ;
	}
    }
}
/* ********************************************************** */
num_nets = 0 ;
tot_cels = 0 ;
for( net1 = 1 ; net1 <= numnets ; net1++ ) {
    if( number[net1][0] <= 1 ) {
	continue ;
    }
    num_nets++ ;
    tot_cels += number[net1][0] ;
}


fprintf(fpo,"\n\n*************************************\n");
fprintf(fpo,"AVERAGE NUMBER OF CELLS PER NET: %f\n",
		(double) tot_cels / (double) num_nets	) ;
fprintf(fpo,"*************************************\n\n\n");
/* ********************************************************** */
for( net1 = 1 ; net1 <= numnets ; net1++ ) {
    if( number[net1][0] == 0 ) {
	how_many[net1] = 0 ;
	continue ;
    }
    if( number[net1][0] == 1 ) {
	number[net1][0] = 0 ;
	how_many[net1] = 0 ;
	continue ;
    }
    how_many[net1] = 1 ;
    for( net2 = net1 + 1 ; net2 <= numnets ; net2++ ) {
	if( number[net2][0] != number[net1][0] ) {
	    continue ;
	}
	different = 0 ;
	for( i = 1 ; i <= numcells ; i++ ) {
	    if( number[net2][i] != number[net1][i] ) {
		different = 1 ;
		break ;
	    }
	}
	if( ! different ) {
	    number[net2][0] = 0 ;
	    how_many[net1]++ ;
	}
    }
}

arraynet[0] = 0 ;
for( net = 1 ; net <= numnets ; net++ ) {
    if( how_many[net] <= 0 ) {
	continue ;
    }
    arraynet[ ++arraynet[0] ] = net ;
}
num = arraynet[0] ;
arraynet[0] = arraynet[ arraynet[0] ] ;
qsortx( (char *) arraynet ,  num , sizeof( int ) ) ;
/*  sorted: most occurrences first  */

num = 0 ;
cnum = 0 ;
c2num = 0 ;
for( net = 1 ; net <= numnets ; net++ ) {
    if( number[net][0] > 0 ) {
	cnum += number[net][0] - 1 ;
	c2num += number[net][0] ;
	num++ ;
    }
}
C = (double) num / (double) numcells ;
C1 = (double) cnum / (double) num ;

#ifdef notdef
C2 = (double) c2num / (double) num ;
C3 = (double) cnum / (double)(numcells - 1) ;

fprintf(fpo,"\n\n\n**********************************************\n\n");
fprintf(fpo,"The average number of distinct nets per cell is\n");
fprintf(fpo,"given by: %6.2f\n\n", C );
fprintf(fpo,"The average number of cells per net is\n");
fprintf(fpo,"given by: %6.2f\n\n", C2 );
fprintf(fpo,"The average number of other cells per net is\n");
fprintf(fpo,"given by: %6.2f\n\n", C1 );
fprintf(fpo,"The ratio of total cells specified per net to\n");
fprintf(fpo,"numcells is given by: %6.2f\n\n", C3 );
fprintf(fpo,"The average number of cells connected to a cell is\n");
fprintf(fpo,"given by: %6.2f\n\n", C * C1 );
fprintf(fpo,"**********************************************\n\n\n");
#endif

ratio = wireratio( numcells, C1, 
		((double) numnets / (double) numcells) / C , C ) ;

fprintf(fpo,"Expected Wire Reduction Relative to Random:%6.2f\n\n",ratio);
fflush(fpo);

return( ratio );
}





int comparex( int *a , int *b )
{
    return( how_many[*b] - how_many[*a] ) ;
}


/* @(#)qsort.c	4.2 (Berkeley) 3/9/83 */


#define		THRESH		4	/* threshold for insertion */
#define		MTHRESH		6	/* threshold for median */

int	qsz;			/* size of each record */
int	thresh;			/* THRESHold in chars */
int	mthresh;		/* MTHRESHold in chars */


void qsortx(char *base, int n, int size)
{
	register char c, *i, *j, *lo, *hi;
	char *min, *max;

	if (n <= 1)
		return;
	qsz = size;
	thresh = qsz * THRESH;
	mthresh = qsz * MTHRESH;
	max = base + n * qsz;
	if (n >= THRESH) {
		qst(base, max);
		hi = base + thresh;
	} else {
		hi = max;
	}
	/*
	 * First put smallest element, which must be in the first THRESH, in
	 * the first position as a sentinel.  This is done just by searching
	 * the first THRESH elements (or the first n if n < THRESH), finding
	 * the min, and swapping it into the first position.
	 */
	for (j = lo = base; (lo += qsz) < hi; )
		if (comparex(j, lo) > 0)
			j = lo;
	if (j != base) {
		/* swap j into place */
		for (i = base, hi = base + qsz; i < hi; ) {
			c = *j;
			*j++ = *i;
			*i++ = c;
		}
	}
	/*
	 * With our sentinel in place, we now run the following hyper-fast
	 * insertion sort.  For each remaining element, min, from [1] to [n-1],
	 * set hi to the index of the element AFTER which this one goes.
	 * Then, do the standard insertion sort shift on a character at a time
	 * basis for each element in the frob.
	 */
	for (min = base; (hi = min += qsz) < max; ) {
		while (comparex(hi -= qsz, min) > 0)
			/* void */;
		if ((hi += qsz) != min) {
			for (lo = min + qsz; --lo >= min; ) {
				c = *lo;
				for (i = j = lo; (j -= qsz) >= hi; i = j)
					*i = *j;
				*i = c;
			}
		}
	}
}

/*
 * qst:
 * Do a quicksort
 * First, find the median element, and put that one in the first place as the
 * discriminator.  (This "median" is just the median of the first, last and
 * middle elements).  (Using this median instead of the first element is a big
 * win).  Then, the usual partitioning/swapping, followed by moving the
 * discriminator into the right place.  Then, figure out the sizes of the two
 * partions, do the smaller one recursively and the larger one via a repeat of
 * this code.  Stopping when there are less than THRESH elements in a partition
 * and cleaning up with an insertion sort (in our caller) is a huge win.
 * All data swaps are done in-line, which is space-losing but time-saving.
 * (And there are only three places where this is done).
 */

void qst(char *base, char *max)
{
	register char c, *i, *j, *jj;
	register int ii;
	char *mid, *tmp;
	int lo, hi;

	/*
	 * At the top here, lo is the number of characters of elements in the
	 * current partition.  (Which should be max - base).
	 * Find the median of the first, last, and middle element and make
	 * that the middle element.  Set j to largest of first and middle.
	 * If max is larger than that guy, then it's that guy, else compare
	 * max with loser of first and take larger.  Things are set up to
	 * prefer the middle, then the first in case of ties.
	 */
	lo = max - base;		/* number of elements as chars */
	do	{
		mid = i = base + qsz * ((lo / qsz) >> 1);
		if (lo >= mthresh) {
			j = (comparex((jj = base), i) > 0 ? jj : i);
			if (comparex(j, (tmp = max - qsz)) > 0) {
				/* switch to first loser */
				j = (j == jj ? i : jj);
				if (comparex(j, tmp) < 0)
					j = tmp;
			}
			if (j != i) {
				ii = qsz;
				do	{
					c = *i;
					*i++ = *j;
					*j++ = c;
				} while (--ii);
			}
		}
		/*
		 * Semi-standard quicksort partitioning/swapping
		 */
		for (i = base, j = max - qsz; ; ) {
			while (i < mid && comparex(i, mid) <= 0)
				i += qsz;
			while (j > mid) {
				if (comparex(mid, j) <= 0) {
					j -= qsz;
					continue;
				}
				tmp = i + qsz;	/* value of i after swap */
				if (i == mid) {
					/* j <-> mid, new mid is j */
					mid = jj = j;
				} else {
					/* i <-> j */
					jj = j;
					j -= qsz;
				}
				goto swap;
			}
			if (i == mid) {
				break;
			} else {
				/* i <-> mid, new mid is i */
				jj = mid;
				tmp = mid = i;	/* value of i after swap */
				j -= qsz;
			}
		swap:
			ii = qsz;
			do	{
				c = *i;
				*i++ = *jj;
				*jj++ = c;
			} while (--ii);
			i = tmp;
		}
		/*
		 * Look at sizes of the two partitions, do the smaller
		 * one first by recursion, then do the larger one by
		 * making sure lo is its size, base and max are update
		 * correctly, and branching back.  But only repeat
		 * (recursively or by branching) if the partition is
		 * of at least size THRESH.
		 */
		i = (j = mid) + qsz;
		if ((lo = j - base) <= (hi = max - i)) {
			if (lo >= thresh)
				qst(base, j);
			base = i;
			lo = hi;
		} else {
			if (hi >= thresh)
				qst(i, max);
			max = j;
		}
	} while (lo >= thresh);
}
