#include <stdio.h>
#include <stdlib.h>

#define  nil		0
#define	 false		0
#define  true		1
#define  bubblebase	1.61f
#define  dnfbase 	3.5f
#define  permbase 	1.75f
#define  queensbase 1.83f
#define  towersbase 2.39f
#define  quickbase 	1.92f
#define  intmmbase 	1.46f
#define  treebase 	2.5f
#define  mmbase 	0.0f
#define  fpmmbase 	2.92f
#define  puzzlebase	0.5f
#define  fftbase 	0.0f
#define  fpfftbase 	4.44f
    /* Towers */
#define maxcells 	 18

    /* Intmm, Mm */
#define rowsize 	 40

    /* Puzzle */
#define size	 	 511
#define classmax 	 3
#define typemax 	 12
#define d 		     8

    /* Bubble, Quick */
#define sortelements 5000
#define srtelements  500

    /* fft */
#define fftsize 	 256 
#define fftsize2 	 129  
/*
type */
    /* Perm */
#define permrange     10

   /* tree */
struct node {
	struct node *left,*right;
	int val;
};

    /* Towers */ /*
    discsizrange = 1..maxcells; */
#define    stackrange	3
/*    cellcursor = 0..maxcells; */
struct    element {
	int discsize;
	int next;
};
/*    emsgtype = packed array[1..15] of char;
*/
    /* Intmm, Mm */ /*
    index = 1 .. rowsize;
    intmatrix = array [index,index] of integer;
    realmatrix = array [index,index] of real;
*/
    /* Puzzle */ /*
    piececlass = 0..classmax;
    piecetype = 0..typemax;
    position = 0..size;
*/
    /* Bubble, Quick */ /*
    listsize = 0..sortelements;
    sortarray = array [listsize] of integer;
*/
    /* FFT */
struct    complex { float rp, ip; } ;
/*
    carray = array [1..fftsize] of complex ;
    c2array = array [1..fftsize2] of complex ;
*/

float value, fixed, floated;

    /* global */
long    seed;  /* converted to long for 16 bit WR*/

    /* Perm */
int    permarray[permrange+1];
/* converted pctr to unsigned int for 16 bit WR*/
unsigned int    pctr;

    /* tree */
struct node *tree;

    /* Towers */
int	   stack[stackrange+1];
struct element    cellspace[maxcells+1];
int    freelist,  movesdone;

    /* Intmm, Mm */

int   ima[rowsize+1][rowsize+1], imb[rowsize+1][rowsize+1], imr[rowsize+1][rowsize+1];
float rma[rowsize+1][rowsize+1], rmb[rowsize+1][rowsize+1], rmr[rowsize+1][rowsize+1];

    /* Puzzle */
int	piececount[classmax+1],	class[typemax+1], piecemax[typemax+1];
int	puzzl[size+1], p[typemax+1][size+1], n, kount;

    /* Bubble, Quick */
int sortlist[sortelements+1], biggest, littlest, top;

    /* FFT */
struct complex    z[fftsize+1], w[fftsize+1], e[fftsize2+1];
float    zr, zi;

void Initrand () {
    seed = 74755L;   /* constant to long WR*/
}

int Rand () {
    seed = (seed * 1309L + 13849L) & 65535L;  /* constants to long WR*/
    return( (int)seed );     /* typecast back to int WR*/
}



    /* A compute-bound program from Forest Baskett. */

int Fit (int i, int j) {
	int k;
	for ( k = 0; k <= piecemax[i]; k++ )
	    if ( p[i][k] ) if ( puzzl[j+k] ) return (false);
	return (true);
}

int Place (int i, int j) {
	int k;
	for ( k = 0; k <= piecemax[i]; k++ ) if ( p[i][k] ) puzzl[j+k] = true;
	piececount[class[i]] = piececount[class[i]] - 1;
	for ( k = j; k <= size; k++ ) if ( ! puzzl[k] ) return (k);
	return (0);
}

void Remove (int i, int j) {
	int k;
	for ( k = 0; k <= piecemax[i]; k++ ) if ( p[i][k] ) puzzl[j+k] = false;
	piececount[class[i]] = piececount[class[i]] + 1;
}

int Trial (int j) {
	int i, k;
	kount = kount + 1;
	for ( i = 0; i <= typemax; i++ )
	    if ( piececount[class[i]] != 0 )
		if ( Fit (i, j) ) {
		    k = Place (i, j);
		    if ( Trial(k) || (k == 0) )return (true);
			else Remove (i, j);
		}
	return (false);
}

void Puzzle ()  {
    int i, j, k, m;
    for ( m = 0; m <= size; m++ ) puzzl[m] = true;
    for( i = 1; i <= 5; i++ )for( j = 1; j <= 5; j++ )for( k = 1; k <= 5; k++ )	puzzl[i+d*(j+d*k)] = false;
    for( i = 0; i <= typemax; i++ )for( m = 0; m<= size; m++ ) p[i][m] = false;
    for( i = 0; i <= 3; i++ )for( j = 0; j <= 1; j++ )for( k = 0; k <= 0; k++ ) p[0][i+d*(j+d*k)] = true;
    class[0] = 0;
    piecemax[0] = 3+d*1+d*d*0;
    for( i = 0; i <= 1; i++ )for( j = 0; j <= 0; j++ )for( k = 0; k <= 3; k++ )	p[1][i+d*(j+d*k)] = true;
    class[1] = 0;
    piecemax[1] = 1+d*0+d*d*3;
    for( i = 0; i <= 0; i++ )for( j = 0; j <= 3; j++ )for( k = 0; k <= 1; k++ )	p[2][i+d*(j+d*k)] = true;
    class[2] = 0;
    piecemax[2] = 0+d*3+d*d*1;
    for( i = 0; i <= 1; i++ )for( j = 0; j <= 3; j++ )for( k = 0; k <= 0; k++ )	p[3][i+d*(j+d*k)] = true;
    class[3] = 0;
    piecemax[3] = 1+d*3+d*d*0;
    for( i = 0; i <= 3; i++ )for( j = 0; j <= 0; j++ )for( k = 0; k <= 1; k++ )	p[4][i+d*(j+d*k)] = true;
    class[4] = 0;
    piecemax[4] = 3+d*0+d*d*1;
    for( i = 0; i <= 0; i++ )for( j = 0; j <= 1; j++ )for( k = 0; k <= 3; k++ )	p[5][i+d*(j+d*k)] = true;
    class[5] = 0;
    piecemax[5] = 0+d*1+d*d*3;
    for( i = 0; i <= 2; i++ )for( j = 0; j <= 0; j++ )for( k = 0; k <= 0; k++ )	p[6][i+d*(j+d*k)] = true;
    class[6] = 1;
    piecemax[6] = 2+d*0+d*d*0;
    for( i = 0; i <= 0; i++ )for( j = 0; j <= 2; j++ )for( k = 0; k <= 0; k++ )	p[7][i+d*(j+d*k)] = true;
    class[7] = 1;
    piecemax[7] = 0+d*2+d*d*0;
    for( i = 0; i <= 0; i++ )for( j = 0; j <= 0; j++ )for( k = 0; k <= 2; k++ )	p[8][i+d*(j+d*k)] = true;
    class[8] = 1;
    piecemax[8] = 0+d*0+d*d*2;
    for( i = 0; i <= 1; i++ )for( j = 0; j <= 1; j++ )for( k = 0; k <= 0; k++ )	p[9][i+d*(j+d*k)] = true;
    class[9] = 2;
    piecemax[9] = 1+d*1+d*d*0;
    for( i = 0; i <= 1; i++ )for( j = 0; j <= 0; j++ )for( k = 0; k <= 1; k++ )	p[10][i+d*(j+d*k)] = true;
    class[10] = 2;
    piecemax[10] = 1+d*0+d*d*1;
    for( i = 0; i <= 0; i++ )for( j = 0; j <= 1; j++ )for( k = 0; k <= 1; k++ )	p[11][i+d*(j+d*k)] = true;
    class[11] = 2;
    piecemax[11] = 0+d*1+d*d*1;
    for( i = 0; i <= 1; i++ )for( j = 0; j <= 1; j++ )for( k = 0; k <= 1; k++ )	p[12][i+d*(j+d*k)] = true;
    class[12] = 3;
    piecemax[12] = 1+d*1+d*d*1;
    piececount[0] = 13;
    piececount[1] = 3;
    piececount[2] = 1;
    piececount[3] = 1;
    m = 1+d*(1+d*1);
    kount = 0;
    if ( Fit(0, m) ) n = Place(0, m);
    else printf("Error1 in Puzzle\n");
    if ( ! Trial(n) ) printf ("Error2 in Puzzle.\n");
    else if ( kount != 2005 ) printf ( "Error3 in Puzzle.\n");
	 printf("%d\n", n);
	 printf("%d\n", kount);
}

int main()
{
	int i;
	for (i = 0; i < 100; i++) Puzzle();
	return 0;
}
