#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**f
 * NOTE: Test lightly.
 **/

void runRewindTests(){
	rewind_file();	
	rewind_empty();
	rewind_ebadf();
}

/**
 * Read a few bytes from a file, rewind, and be sure we
 * end up back at the beginning.
 **/
void rewind_file(){
	const char* testName = "rewind_basic";
	const char* expectedResult = "zbcde";
	FILE* pFile = 0;
	char* readBuf[ gBufLarge ];
	char* bytesRead[ gBufLarge ];
	int ret = -1;
	
	memset( readBuf, 0, gBufLarge );
	memset( bytesRead, 0, gBufLarge );
	
	// Create a file to test with.
	int theFile = open( testName, O_CREAT | O_RDWR, S_IRWXU );
	write( theFile, gData1, 5 );
	close( theFile );
	
	pFile = fopen( testName, "r+" );
	ret = fread( readBuf, sizeof( char ), 3, pFile );
	if( ret == 3 ){
		// Confirm the file's current offset.
		if( ftell( pFile ) == 3 ){
			rewind( pFile );
			if( ftell( pFile ) == 0 ){
				// Now let's just do a write and read it back.
				fputc( 'z', pFile );
				fflush( pFile );

				read( open( testName, O_RDONLY ), bytesRead, gBufLarge );
				close( theFile );
				if( strcmp( (const char*)bytesRead, expectedResult ) == 0 ){
					printPass( testName );
				}else{
					printFail( testName, "File contents were not correct." );
				}
			}else{
				printFail( testName, "Offset is not correct after rewind" );
			}
		}else{
			printFail( testName, "Offset is not correct after reading" );
		}
	}else{
		printFail( testName, "Read failed" );
	}
	
	fclose( pFile );
}

/**
 * Rewind an empty file.
 **/
void rewind_empty(){
	const char* testName = "rewind_empty";
	FILE* pFile = 0;
	
	// Create a file to test with.
	int theFile = open( testName, O_CREAT | O_RDWR, S_IRWXU );
	close( theFile );
	
	pFile = fopen( testName, "r+" );
	errno = 0;
	rewind( pFile );

	if( (errno == 0) && (ftell( pFile ) == 0) ){
		printPass( testName );
	}else{
		printFail( testName, "Offset is not correct after rewind" );
	}

	fclose( pFile );	
}

/**
 * Trigger ebadf.
 **/
void rewind_ebadf(){
	const char* testName = "rewind_ebadf";
	FILE* pFile = 0;
	
	pFile = fopen( testName, "w+" );
	fclose( pFile );
	errno = 0;
	rewind( pFile );
	
	if( errno == EBADF ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF" );
	}
	
	fclose( pFile );	
}
