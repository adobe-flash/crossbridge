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

void runFreadTests(){
	fread_basic();
	fread_subsequent();
	fread_ebadf();
}

/**
 * Create a file, write, read.
 **/
void fread_basic(){
	const char* testName = "fread_basic";
	int theFile = -1;
	int ret = 0;
	FILE* pFile = 0;
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );

	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );


	// Read it.
	pFile = fopen( testName, "r" );
	errno = 0;
	ret = fread( bytesRead, sizeof( char ), gBufLarge, pFile );

	if( (errno == 0) && (ret == strlen( gData1 )) ){
		if( strcmp( bytesRead, gData1 ) == 0 ){
			printPass( testName ); 
		}else{
			printFail( testName, "Data read was not correct" );
		}
	}else{
		printFail( testName, "Expected errno = 0 and a different string length." );
	}

	fclose( pFile );
}

/**
 * Read a few bytes, then read a few more.
 **/
void fread_subsequent(){
	const char* testName = "fread_subsequent";
	const char* expected1 = "abc";
	const char* expected2 = "abcde";
	int theFile = -1;
	int ret = 0;
	FILE* pFile = 0;
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Read it.
	pFile = fopen( testName, "r" );
	errno = 0;
	ret = fread( bytesRead, sizeof( char ), 3, pFile );
	
	if( (errno == 0) && (ret == 3) && (strcmp( bytesRead, expected1 ) == 0) ){
		ret = fread( bytesRead + 3, sizeof( char ), 2, pFile );	
		if( (errno == 0) && (ret == 2) && (strcmp( bytesRead, expected2 ) == 0) ){
			printPass( testName );
		}else{
			printFail( testName, "Second read failed" );
		}
	}else{
		printFail( testName, "First read failed" );
	}
	
	fclose( pFile );
}

/**
 * Trigger ebadf.
 **/
void fread_ebadf(){
	const char* testName = "fread_ebadf";
	int theFile = -1;
	int ret = 0;
	FILE* pFile = 0;
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Try to read it.
	pFile = fopen( testName, "a" );
	errno = 0;
	ret = fread( bytesRead, sizeof( char ), gBufLarge, pFile );
	
	if( (errno == EBADF) && (ret == 0) ){
		printPass( testName ); 
	}else{
		printFail( testName, "Expected EBADF and 0" );
	}
	
	fclose( pFile );
}