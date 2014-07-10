#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * NOTE: There are only a couple basic tests here; this is not done
 * being tested.
 **/

void runFflushTests(){
	fflush_basic();
	fflush_ebadf();
}

/**
 * Create a file, write, read.
 **/
void fflush_basic(){
	const char* testName = "fflush_basic";
	int ret = -1;
	int theFile = -1;
	FILE* pFile = 0;
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	// Write.
	pFile = fopen( testName, "w+" );
	fwrite( gData1, sizeof( char ), strlen( gData1 ), pFile );

	// Read what was written.
	theFile = open( testName, O_RDONLY );
	read( theFile, bytesRead, strlen( gData1 ) );
	close( theFile );

	if( strlen( bytesRead ) == 0 ){		
		// Flush.
		errno = 0;
		ret = fflush( pFile );

		if( (ret == 0) && (errno == 0) ){
			// Read what was written.
			theFile = open( testName, O_RDONLY );
			read( theFile, bytesRead, strlen( gData1 ) );
			close( theFile );
			if( strcmp( bytesRead, gData1 ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "Flushed data was not correct" );
			}
		}else{
			printFail( testName, "Expected ret == 0 and errno == 0" );
		}
		
	}else{
		printFail( testName, "Looks like flushing already happened, so we cannot continue with this test" );
	}

	fclose( pFile );
}

/**
 * Trigger ebadf.
 **/
void fflush_ebadf(){
	const char* testName = "fflush_ebadf";
	FILE* pFile;
	
	// Write.
	pFile = fopen( testName, "w+" );
	fclose( pFile );
	errno = 0;
	int ret = fflush( pFile );
	
	if( (errno == EBADF) && (ret == EOF) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF and EOF" );
	}

}