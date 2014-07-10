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
 * NOTE: There are only a couple basic tests here; this is not done
 * being tested.
 **/

void runFputsTests(){
	fputs_basic();
	fputs_subsequent();
	fputs_ebadf();
}

/**
 * Create a file, write, read.
 * fputs() returns "a non-negative number" (no details provided) according
 * to some references, and zero according to other references.  
 **/
void fputs_basic(){
	const char* testName = "fputs_basic";
	int ret = 0;
	int theFile = -1;
	int savedErrno = -1;
	FILE* pFile = 0;	
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_CREAT, S_IRWXU );
	close( theFile );
	
	// Put.
	pFile = fopen( testName, "w" );
	errno = 0;
	ret = fputs( gData1, pFile );
	savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == 0) && (ret == 0 ) ){
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, strlen( gData1 ) );
		close( theFile );
		
		if( strcmp( gData1, bytesRead ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "File contents were not correct" );
		}
	}else{
		printFail( testName, "Expected errno == 0 and return value == 0." );
	}
}

/**
 * Put subsequent times.
 **/
void fputs_subsequent(){
	const char* testName = "fputs_subsequent";
	const char* expected1 = "abcde";
	const char* expected2 = "abcde0123456789";
	int ret = 0;
	int theFile = -1;
	int savedErrno = -1;
	FILE* pFile = 0;	
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_CREAT, S_IRWXU );
	close( theFile );
	
	// Put.
	pFile = fopen( testName, "w" );
	errno = 0;
	ret = fputs( gData1, pFile );
	savedErrno = errno;
	fflush( pFile );

	// Read.
	theFile = open( testName, O_RDONLY );
	read( theFile, bytesRead, gBufSmall );
	close( theFile );

	if( (savedErrno == 0) && (ret == 0) && (strcmp( bytesRead, expected1 ) == 0) ){
		errno = 0;
		ret = fputs( gData2, pFile );
		savedErrno = errno;
		fflush( pFile );

		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, gBufSmall );
		close( theFile );

		if( (savedErrno == 0) && (ret == 0) && (strcmp( bytesRead, expected2 ) == 0) ){
			printPass( testName );
		}else{
			printFail( testName, "Second put failed" );
		}
	}else{
		printFail( testName, "First put failed" );
	}
	
	fclose( pFile );	
}

/**
 * Attempt to write w/o appropriate permission.
 **/
void fputs_ebadf(){
	const char* testName = "fputs_ebadf";
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
	ret = fputs( testName, pFile );
	savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == EBADF) && (ret == EOF) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF and a null pointer returned" );
	}
}