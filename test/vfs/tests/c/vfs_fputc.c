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

void runFputcTests(){
	fputc_basic();
	fputc_subsequent();
	fputc_ebadf();
}

/**
 * Create a file, write, read.
 **/
void fputc_basic(){
	const char* testName = "fputc_basic";
	const char* expectedResult = "E";
	char ret = '\0';
	int theFile = -1;
	int savedErrno = -1;
	FILE* pFile = 0;

	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_CREAT, S_IRWXU );
	close( theFile );
	
	// Put.
	pFile = fopen( testName, "w+" );
	errno = 0;
	ret = fputc( 'E', pFile );
	savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == 0) && (ret == 'E') ){
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, 1 );
		close( theFile );
		if( strcmp( expectedResult, bytesRead ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "File contents were not correct" );
		}
	}else{
		printFail( testName, "Expected errno = 0 and return value of \'E\'" );
	}
	
	fclose( pFile );
}

/**
 * Call subsquent times.
 **/
void fputc_subsequent(){
	const char* testName = "fputc_subsequent";
	const char* expected1 = "a";
	const char* expected2 = "ab";
	char ret = '\0';
	int theFile = -1;
	int savedErrno = -1;
	FILE* pFile = 0;
	
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_CREAT, S_IRWXU );
	close( theFile );
	
	// Put.
	pFile = fopen( testName, "w+" );
	errno = 0;
	ret = fputc( 'a', pFile );
	savedErrno = errno;
	fflush( pFile );

	// Try to read 5 characters even though there's only one.
	theFile = open( testName, O_RDONLY );
	read( theFile, bytesRead, 5 );
	close( theFile );

	if( (savedErrno == 0) && (ret == 'a') && (strcmp( bytesRead, expected1 ) == 0) ){
		errno = 0;
		ret = fputc( 'b', pFile );
		savedErrno = errno;
		fflush( pFile );
		
		// Try to read 5 characters even though there's only one.
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, 5 );
		close( theFile );
		
		if( (savedErrno == 0) && (ret == 'b') && (strcmp( bytesRead, expected2 ) == 0) ){
			printPass( testName );
		}else{
			printFail( testName, "Second putc failed" );
		}
	}else{
		printFail( testName, "First putc failed" );
	}

	fclose( pFile );
}

/**
 * Attempt to write w/o appropriate permission.
 **/
void fputc_ebadf(){
	const char* testName = "fputc_ebadf";
	int theFile = -1;
	int savedErrno = -1;
	int ret = 0;
	FILE* pFile = 0;
	
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_CREAT, S_IRWXU );
	close( theFile );
	
	// Put.
	pFile = fopen( testName, "r" );
	errno = 0;
	ret = fputc( 'E', pFile );
	savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == EBADF) && (ret == EOF) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF and a null pointer returned" );
	}
}