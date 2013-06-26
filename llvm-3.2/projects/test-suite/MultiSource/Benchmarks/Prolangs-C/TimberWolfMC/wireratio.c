/* #include "custom.h" */
#include "port.h"
#define ABS(value)   ( (value)>=0 ? (value) : -(value) ) 

double Nterm1(double k);
double Nterm2(double k);
double Nterm3(double k);
double Nterm4(double k);
double Nterm5(double k);
double NNterm1(double k);
double NNterm2(double k);
double NNterm3(double k);
double NNterm4(double k);
double NNterm5(double k);
double Dterm1(double k);
double Dterm2(double k);
double Dterm3(double k);
double Dterm4(double k);
double DDterm1(double k);
double DDterm2(double k);
double DDterm3(double k);
double DDterm4(double k);
double factorial( int n );
double combination( int n , int k );
double probability( int k , int h , int i , int N );
double getptree( int h , int L , int N );
double probtree( double atatime , int N , double numdraws );
void findratio( double *res1 , double *res2 );

double rootN , N , CC ;
double a , bb , c ;
extern FILE *fpo ;



double wireratio( int numofcells, double cellspernet, double netsperd,
                  double dnetspercell )
{

double result1 , result2 , match ;



N = (double) numofcells ;
CC = probtree(  ((cellspernet >= 1.0) ? (cellspernet) : 1.0) ,
		((numofcells >= 2) ? (numofcells - 1) : 1)   ,
		((dnetspercell >= 1.0) ? (dnetspercell) : 1.0)    ) ;

bb = 100.0 ;
a = 0.01 ;

findratio( &result1 , &result2 ) ;
match = result2 - result1 ;

a = netsperd - 2.5 ;
if( a > 0 ) {
    a = 0.01 + ((a + 2.5) * 1.25) ;
} else {
    a = 0.01 ;
}
bb = pow( 10.0 , - cellspernet + 3.3 ) ;

findratio( &result1 , &result2 ) ;

fprintf(fpo,"\n\n_________________________________________________________________________\n\n");
fprintf(fpo,"DISTINCT NETS PER CELL ---------------- : %.2f\n", dnetspercell);
fprintf(fpo,"NETS PER DISTINCT NET  ---------------- : %.2f\n", netsperd ) ;
fprintf(fpo,"CELLS PER DISTINCT NET ---------------- : %.2f\n", cellspernet );
fprintf(fpo,"AVE. # CELLS CONNECTED TO A CELL ------ : %.2f\n", CC ) ;
fprintf(fpo,    "_________________________________________________________________________\n");

/*
printf("Improvement RATIO: %6.2f\n\n", result1 ) ;
printf("NEW IMPROVED: Improvement RATIO: %6.2f\n\n", result2 - match ) ;
*/

return( result2 - match ) ;
}





void findratio( double *res1 , double *res2 )
{


double m , k , x , y , diff , N1 , N2 , D1 , D2 ;
int i , j , now , last , savei , savej , lessThanM ;

rootN = sqrt( N ) ;
m = rootN - 1 ;



lessThanM = 1 ;
diff = 1000000.0 ;

for( i = 0 ; i < 1000000 ; i++ ) {
    j = (i == 0) ? 10 : 0 ;
    for( ; j < 100 ; j++ ) {
	k = (double) i + ( (double) j / 100.0 ) ;
	if( k > m ) {
	    lessThanM = 0 ;
	    savei = i ;
	    savej = j ;
	    i = 1000000 ;
	    break ;
	}

	x = (k / (6.0 * N)) * (  (k * k * k)  +  
			(2.0 - 8.0 * rootN) * (k * k)  +
			(12.0 * N - 12.0 * rootN - 1.0) * k  +
			(12.0 * N - 4.0 * rootN - 2.0) ) ;
	now = (x > CC) ? 1 : -1 ;
	if( !( i == 0 && j == 10) ) {
	    if( now != last ) {
		if( diff <= ABS(x - CC) ) {
		    k = k - 0.01 ;
		}
		i = 1000000 ;
		break ;
	    } else {
		last = now ;
		diff = ABS( x - CC ) ;
	    }
	} else {
	    last = now ;
	}
    }
}
if( ! lessThanM ) {
    diff = 1000000.0 ;
    i = savei ;
    j = savej ;

    for( ; i < 1000000 ; i++ ) {
	if( i > savei ) {
	    j = 0 ;
	}
	for( ; j < 100 ; j++ ) {
	    k = (double) i + ( (double) j / 100.0 ) ;

	    x = (1.0 / (6.0 * N)) * (  - (k * k * k * k)  +  
		    (8.0 * rootN - 2.0) * (k * k * k)  +
		    (12.0 * rootN - 24.0 * N + 1.0) * (k * k)  +
		    (32.0 * N * rootN - 24.0 * N - 4.0 * rootN + 2.0) * k +

		    (2.0) * (m * m * m * m)  -  
		    (16.0 * rootN - 4.0) * (m * m * m)  -
		    (24.0 * rootN - 36.0 * N + 2.0) * (m * m)  -
		    (32.0 * N * rootN - 36.0 * N + 4.0) * m   ) ;
	    now = (x > CC) ? 1 : -1 ;
	    if( ! (i == savei && j == savej) ) {
		if( now != last ) {
		    if( diff <= ABS(x - CC) ) {
			k = k - 0.01 ;
		    }
		    i = 1000000 ;
		    break ;
		} else {
		    last = now ;
		    diff = ABS( x - CC ) ;
		}
	    } else {
		last = now ;
	    }
	}
    }
}

/*
printf("\nThe value of  k  of your circuit is: %6.2f\n", k ) ;
printf("The value of  m  of your circuit is: %6.2f\n\n", m ) ;
*/


if( k <= m ) {
    x = 0.4 * ( ((2.0) * (k * k * k * k)  +
		 (5.0 - 15.0 * rootN) * (k * k * k)  +
		 (20.0 * N - 30.0 * rootN) * (k * k)  +
		 (30.0 * N - 15.0 * rootN - 5.0) * (k)  +
		 (10.0 * N - 2.0))  / 
				(6.0 * CC * N / k) ) ;
} else {
    x = ( - 2.0) * (k * k * k * k * k)  +
	(15.0 * rootN - 5.0) * (k * k * k * k)  +
	(30.0 * rootN - 40.0 * N) * (k * k * k)  +
	(40.0 * N * rootN - 60.0 * N + 5.0 * rootN + 5.0) * (k * k) +
	(40.0 * N * rootN - 20.0 * N - 10.0 * rootN + 2.0) * (k) ;

    x += (4.0) * (m * m * m * m * m)  -
	 (30.0 * rootN - 10.0) * (m * m * m * m)  -
	 (60.0 * rootN - 60.0 * N) * (m * m * m)  -
	 (40.0 * N * rootN - 90.0 * N + 20.0 * rootN + 10.0) * (m * m) -
	 (40.0 * N * rootN - 30.0 * N - 10.0 * rootN + 4.0) * (m) ;
    x = 0.4 * x / (6.0 * CC * N) ;

}


y = (2.0 / 3.0) * sqrt( N ) ;

*res1 = y / x ;


c = a + bb ;

if( k > m ) {
    N1 =  Nterm1(m) - Nterm1(1.0) +
	  Nterm2(m) - Nterm2(1.0) +
	  Nterm3(m) - Nterm3(1.0) +
	  Nterm4(m) - Nterm4(1.0) +
	  Nterm5(m) - Nterm5(1.0) ;
    D1 =  Dterm1(m) - Dterm1(1.0) +
	  Dterm2(m) - Dterm2(1.0) +
	  Dterm3(m) - Dterm3(1.0) +
	  Dterm4(m) - Dterm4(1.0) ;
} else {
    N1 =  Nterm1(k) - Nterm1(1.0) +
	  Nterm2(k) - Nterm2(1.0) +
	  Nterm3(k) - Nterm3(1.0) +
	  Nterm4(k) - Nterm4(1.0) +
	  Nterm5(k) - Nterm5(1.0) ;
    D1 =  Dterm1(k) - Dterm1(1.0) +
	  Dterm2(k) - Dterm2(1.0) +
	  Dterm3(k) - Dterm3(1.0) +
	  Dterm4(k) - Dterm4(1.0) ;
}


if( k > m ) {
    N2 =  NNterm1(k) - NNterm1(m) +
	  NNterm2(k) - NNterm2(m) +
	  NNterm3(k) - NNterm3(m) +
	  NNterm4(k) - NNterm4(m) +
	  NNterm5(k) - NNterm5(m) ;
    D2 =  DDterm1(k) - DDterm1(m) +
	  DDterm2(k) - DDterm2(m) +
	  DDterm3(k) - DDterm3(m) +
	  DDterm4(k) - DDterm4(m) ;

    x = (N1 + N2) / (D1 + D2) ;
} else {
    x = N1 / D1 ;
}

*res2 = y / x ;

return ;
}


double Nterm1( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return( ((k*k*k*k) / 6.0) * ((exa / (a)) - (exc / (c))) ) ;
}


double Nterm2( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k * k * k) * 
	(  ((2.0 / 3.0) * ((exa / (a * a)) - (exc / (c * c))))
	   - rootN * ((exa / (a)) - (exc / (c)))   ) ) ;
}


double Nterm3( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k * k) * 
	(  (2.0 * ((exa / (a * a * a)) - (exc / (c * c * c))))
       - ((3.0 * rootN) * ((exa / (a * a)) - (exc / (c * c))))
       + (N * ((exa / (a)) - (exc / (c))))   ) ) ;
}



double Nterm4( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k) * 
    (  (4.0 * ((exa / (a * a * a * a)) - (exc / (c * c * c * c))))
   - ((6.0 * rootN) * ((exa / (a * a * a)) - (exc / (c * c * c))))
       + ((2.0 * N) * ((exa / (a * a)) - (exc / (c * c))))   ) ) ;
}



double Nterm5( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((4.0 * ((exa / (a*a*a*a*a)) - (exc /(c * c * c * c * c))))
   - ((6.0 * rootN) * ((exa / (a*a*a*a)) - (exc / (c * c * c * c))))
       + ((2.0 * N) * ((exa / (a*a*a)) - (exc / (c * c * c)))) );
}


double Dterm1( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return(((k * k * k) / 6.0) * ((exa / (a)) - (exc / (c)))) ;
}



double Dterm2( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k * k) * 
	(  ((1.0 / 2.0) * ((exa / (a * a)) - (exc / (c * c))))
	   - rootN * ((exa / (a)) - (exc / (c)))   ) ) ;
}



double Dterm3( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k) * 
	(  ((exa / (a * a * a)) - (exc / (c * c * c)))
	   - ((2.0 * rootN) * ((exa / (a * a)) - (exc / (c * c))))
	   + (N * ((exa / (a)) - (exc / (c))))   ) ) ;
}


double Dterm4( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return( ((exa / (a * a * a * a)) - (exc / (c * c * c * c))) - 
    ((2.0 * rootN) * ((exa / (a * a * a)) - (exc / (c * c * c))))
	   + (N * ((exa / (a * a)) - (exc / (c * c)))) ) ;
}


double NNterm1( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return( ((k*k*k*k) / -6.0) * ((exa / (a)) - (exc / (c))) ) ;
}


double NNterm2( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k * k * k) * 
	(  ((-2.0 / 3.0) * ((exa / (a * a)) - (exc / (c * c))))
	   + rootN * ((exa / (a)) - (exc / (c)))   ) ) ;
}


double NNterm3( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k * k) * 
	(  (-2.0 * ((exa / (a * a * a)) - (exc / (c * c * c))))
       + ((3.0 * rootN) * ((exa / (a * a)) - (exc / (c * c))))
       - (2.0 * N * ((exa / (a)) - (exc / (c))))   ) ) ;
}



double NNterm4( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k) * 
    (  (-4.0 * ((exa / (a * a * a * a)) - (exc / (c * c * c * c))))
   + ((6.0 * rootN) * ((exa / (a * a * a)) - (exc / (c * c * c))))
       - ((4.0 * N) * ((exa / (a * a)) - (exc / (c * c))))   
       + (((4.0 / 3.0) * N * rootN) * ((exa / a) - (exc / c)))   ) ) ;
}



double NNterm5( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((-4.0 * ((exa / (a*a*a*a*a)) - (exc /(c * c * c * c * c))))
   + ((6.0 * rootN) * ((exa / (a*a*a*a)) - (exc / (c * c * c * c))))
   - ((4.0 * N) * ((exa / (a*a*a)) - (exc / (c * c * c))))
   + (((4.0 / 3.0) * N * rootN) * ((exa / (a * a)) - (exc / (c * c ))))  );
}


double DDterm1( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return(((k * k * k) / -6.0) * ((exa / (a)) - (exc / (c)))) ;
}



double DDterm2( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k * k) * 
	(  ((-1.0 / 2.0) * ((exa / (a * a)) - (exc / (c * c))))
         + (rootN * ((exa / (a)) - (exc / (c))))   ) ) ;
}



double DDterm3( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return((k) * 
	(  - ((exa / (a * a * a)) - (exc / (c * c * c)))
	   + ((2.0 * rootN) * ((exa / (a * a)) - (exc / (c * c))))
	   - ((2.0 * N) * ((exa / (a)) - (exc / (c))))   ) ) ;
}


double DDterm4( double k )
{
double exa , exc ;

exa = - exp( - (a * (k - 1.0)) ) ;
exc = - exp( - (c * (k - 1.0)) ) ;
return( - ((exa / (a * a * a * a)) - (exc / (c * c * c * c))) + 
    ((2.0 * rootN) * ((exa / (a * a * a)) - (exc / (c * c * c))))
    - ((2.0 * N) * ((exa / (a * a)) - (exc / (c * c))))
    + (((4.0 / 3.0) * N * rootN) * ((exa / (a)) - (exc / (c))))   );
}





double probtree( double atatime , int N , double numdraws )
{

int h , L ;
double res1, res2, res3, res4, result1, result2 ;
double fract1, fract2 ;


h = (int) atatime ;
L = (int) numdraws ;
fract1 = atatime - (double) h ;
fract2 = numdraws - (double) L ;

res1 = getptree( h , L , N ) ;
h++ ;
res2 = getptree( h , L , N ) ;

result1 = res1 + (fract1) * (res2 - res1) ;

h-- ;
L++ ;
res3 = getptree( h , L , N ) ;
h++ ;
res4 = getptree( h , L , N ) ;

result2 = res3 + (fract1) * (res4 - res3) ;

return( result1 + (fract2) * (result2 - result1) ) ;
}




double getptree( int h , int L , int N )
{

typedef struct array3d {
    int value ;
    double prob ;
} array3d ;

array3d *array ;
int bound , i , start , target , j ;
double numerator , denominator ;



bound = 1 ;
for( i = 1 ; i <= L ; i++ ) {
    bound *= (h + 1) ;
}
bound = (bound - 1) / h ;

start = 1 ;
for( i = 1 ; i < L ; i++ ) {
    start *= (h + 1) ;
}
start = (start - 1) / h + 1 ;


array = (array3d *) malloc( (1 + bound) * sizeof( array3d ) ) ;

array[1].value = h ;
array[1].prob  = 1.0 ;

for( i = 1 ; i <= start - 1 ; i++ ) {
    target = (h + 1) * i - (h - 1) ;
    for( j = 0 ; j <= h ; j++ , target++ ) {
	array[target].value = array[i].value + j ;
	array[target].prob  = array[i].prob * 
				probability( array[i].value , h , j , N ) ;
    }
}

numerator   = 0.0 ;
denominator = 0.0 ;
for( i = start ; i <= bound ; i++ ) {
    numerator += array[i].value * array[i].prob ;
}
for( i = start ; i <= bound ; i++ ) {
    denominator += array[i].prob ;
}
free( array ) ;
if( numerator < 0.000001 ) {
    return(0.0) ;
} else {
    return( numerator / denominator ) ;
}
}



double probability( int k , int h , int i , int N )
{
if( k + i > N ) {
    return( 0.0 ) ;
} else {
    return( combination( k , h - i ) * combination( N - k , i )  ) ;
}
}

double combination( int n , int k )
{

double result ;
int i ;

result = 1.0 ;
if( n - k > k ) {
    for( i = 0 ; i <= k - 1 ; i++ ) {
	result *= (double)( n - i ) ;
    }
    return( result / factorial( k )  ) ;
} else {
    for( i = n ; i >= k + 1 ; i-- ) {
	result *= (double)( i ) ;
    }
    return( result / factorial( n - k )  ) ;
}
}



double factorial( int n )
{

double result ;

result = 1.0 ;
for( ; n >= 1 ; n-- ) {
    result *= (double) n ; 
}

return( result ) ;
}
