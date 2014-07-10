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
 * NOTE: Test lightly.
 **/

void runFwriteTests(){
	fwrite_basic();
	fwrite_subsequent();
	fwrite_ebadf();
}

/**
 * Create a file, write, read.
 **/
void fwrite_basic(){
	const char* testName = "fwrite_basic";
	int theFile = -1;
	int ret = -1;
	int savedErrno = -1;
	FILE* pFile = 0;
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	// Create a file for testing.
	theFile = open( testName, O_CREAT, S_IRWXU );
	close( theFile );

	// Write.
	pFile = fopen( testName, "w+" );
	errno = 0;
	ret = fwrite( gData1, sizeof( char ), strlen( gData1 ), pFile );
	savedErrno = errno;
	fclose( pFile );
	
	if( (ret == strlen( gData1 )) && (savedErrno == 0) ){
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, strlen( gData1 ) );
		close( theFile );
		
		if( strcmp( bytesRead, gData1 ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not correct" );
		}
	}else{
		printFail( testName, "Expected errno = 0 and correct number of bytes read" );
	}
}

/**
 * Write a couple times.
 **/
void fwrite_subsequent(){
	const char* testName = "fwrite_subsequent";
	const char* expected1 = "abc";
	const char* expected2 = "abcde";
	int theFile = -1;
	int ret = -1;
	int savedErrno = -1;
	FILE* pFile = 0;
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	// Create a file for testing.
	theFile = open( testName, O_CREAT, S_IRWXU );
	close( theFile );
	
	// Write.
	pFile = fopen( testName, "w+" );
	errno = 0;
	ret = fwrite( gData1, sizeof( char ), 3, pFile );
	savedErrno = errno;
	fflush( pFile );

	theFile = open( testName, O_RDONLY );
	read( theFile, bytesRead, strlen( gData1 ) );
	close( theFile );	
	
	if( (ret == 3) && (savedErrno == 0) && (strcmp( bytesRead, expected1 ) == 0) ){
		errno = 0;
		ret = fwrite( gData1 + 3, sizeof( char ), 2, pFile );
		savedErrno = errno;
		fflush( pFile );
		
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, strlen( gData1 ) );
		close( theFile );	

		if( (ret == 2) && (savedErrno == 0) && (strcmp( bytesRead, expected2 ) == 0) ){
			printPass( testName );
		}else{
			printFail( testName, "Second write failed" );
		}	
	}else{
		printFail( testName, "First write failed" );
	}

	fclose( pFile );
}

/**
 * Attempt to write w/o appropriate permission.
 **/
void fwrite_ebadf(){
	const char* testName = "fwrite_ebadf";
	int theFile = -1;
	int savedErrno = -1;
	int ret = 0;
	FILE* pFile = 0;
	
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_CREAT, S_IRWXU );
	close( theFile );
	
	// Write.
	pFile = fopen( testName, "r" );
	errno = 0;
	ret = fwrite( testName, 1, 1, pFile );
	savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == EBADF) && (ret == 0) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF and a null pointer returned" );
	}
}