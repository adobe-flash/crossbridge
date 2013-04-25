#include "AS3/AS3.h"
#include <iostream>
#include <sys/time.h>

using namespace std;

#define NUM_ITERATIONS 2000000 // 2 million times

int timeval_subtract( timeval, timeval );

void doEx1( int );
void doEx2( int );
void doEx3( int );
void doEx4( int );
void doEx5( int );

int main( int argc, char* argv[] ){
	int i = 0;
	struct timeval pre_time;
	struct timeval post_time;
	int elapsed = 0;
	struct timezone* zone = 0;

	inline_as3( "import com.adobe.flascc.Console;\n" );	
	inline_as3( "trace( \"Try/catch across functions\" );\n" );

	gettimeofday( &pre_time, zone );

	for( i = 0; i < NUM_ITERATIONS; i++ ){
		try{
			doEx1( i );
		}catch( int j ){
			if( j % 100000 == 0 ){
				inline_as3( "trace( %0 );\n" : : "r"(j) );
			}
		}	
	}

	gettimeofday( &post_time, zone );
	elapsed = timeval_subtract( post_time, pre_time  );	
	
	inline_as3( "trace( %0 );\n" : : "r"(i) );
	inline_as3( "new Console().qaSendInfo(\"PERF_Exception across functions_MS=\", %0, true );\n" : : "r"(elapsed) );
	return 0;
}

void doEx1( int i ){
	doEx2( i );
}

void doEx2( int i ){
	doEx3( i );
}

void doEx3( int i ){
	doEx4( i );
}

void doEx4( int i ){
	doEx5( i );
}

void doEx5( int i ){
	throw( i );
}

int timeval_subtract( timeval t1, timeval t2 ){
	return ( ( (t1.tv_sec - t2.tv_sec) * 1000000) + 
			( t1.tv_usec - t2.tv_usec + 500 ) ) / 1000;
}