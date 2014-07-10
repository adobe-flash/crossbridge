#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "../../framework/vfs.h"

void runClockgettimeTests(){
	clock_gettime_basic();
}

/**
 * Get the time, wait a few seconds, and get it again.
 **/
void clock_gettime_basic(){
	const char* testName = "clock_gettime_basic";
	struct timespec tp;
	time_t t1 = -1;
	time_t t2 = -1;
	int result = -1;

	for( int i = 0; i < 4; ++i ){
		clock_gettime( CLOCK_REALTIME, &tp );
		t1 = tp.tv_sec;
		printf("t1=%d\n", t1);

		sleep( 2 );
		
		clock_gettime( CLOCK_REALTIME, &tp );
		t2 = tp.tv_sec;
		printf("t2=%d\n", t2);

		time_t elapsed = t2 - t1;

		// Remember rounding.
		if( elapsed >= 2 && elapsed <= 3 ){
			result = 0;
		}else{
			result = -1;
			break;
		}
	}

	if( result == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Elapsed time was incorrect." );
	}	
}