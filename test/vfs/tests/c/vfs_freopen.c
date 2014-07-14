#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**f
 * NOTE: Test lightly only.
 **/
void runFreopenTests(){
	freopen_new_file();
	freopen_new_mode();
}

/**
 * Use freopen() to use the same stream on a different file.
 **/
void freopen_new_file(){
	const char* testName = "freopen_new_file";
	const char* file1 = "freopen_new_file_1";
	const char* file2 = "freopen_new_file_2";
	FILE* pFile1 = 0;
	FILE* pFile2 = 0;
	int numBytesWritten = 0;
	int numBytesRead = 0;
	char* bytesRead = (char*)calloc( gBufLarge, sizeof( char ) );
	
	if( bytesRead == 0 ){
		printFail( testName, "calloc() fail" );
		return;
	}
	
	// This is the file we will switch to (abcde).
	pFile1 = fopen( file1, "w+" );
	numBytesWritten = fwrite( gData1, sizeof( char ), strlen( gData1 ), pFile1 );
	if( numBytesWritten != strlen( gData1 ) ){
		printFail( testName, "Unable to write to test file." );
		return;
	}
	fclose( pFile1 );
	
	// This is the file we will start writing to and then abandon (0123456789).
	pFile2 = fopen( file2, "w+" );
	numBytesWritten = fwrite( gData2, sizeof( char ), strlen( gData2 ), pFile2 );
	if( numBytesWritten != strlen( gData2 ) ){
		printFail( testName, "Unable to write to test file." );
		return;
	}
	
	// Now switch to the first file without flushing or closing
	// the second one and verify that we're reading it.
	pFile2 = freopen( file1, "r", pFile2 );
	numBytesRead = fread( bytesRead, sizeof( char ), strlen( gData1 ), pFile2 );
	if( numBytesRead != strlen( gData1 ) ){
		printFail( testName, "Failed to read from the reopened file." );
		return;
	}
	
	if( strcmp( bytesRead, gData1 ) != 0 ){
		printFail( testName, "Incorrect data reading the reopened file." );
		return;
	}
	
	// Now switch back to the second file.  It should have automatically been
	// flushed and closed, so we should be able to read 012... from it.
	memset( bytesRead, 0, gBufLarge );
	pFile2 = freopen( file2, "r", pFile2 );
	numBytesRead = fread( bytesRead, sizeof( char ), strlen( gData2 ), pFile2 );
	if( numBytesRead != strlen( gData2 ) ){
		printFail( testName, "Failed to read from the second reopened file." );
		return;
	}
	
	if( strcmp( bytesRead, gData2 ) != 0 ){
		printFail( testName, "Incorrect data reading the second reopened file." );
		return;
	}
	
	printPass( testName );
}

/**
 * Use freopen() to reopen the same file with a different mode.
 **/
void freopen_new_mode(){
	const char* testName = "freopen_new_mode";
	FILE* pFile = 0;
	int numBytesWritten = 0;
	int numBytesRead = 0;
	char* bytesRead = (char*)calloc( gBufLarge, sizeof( char ) );
	
	if( bytesRead == 0 ){
		printFail( testName, "calloc() fail" );
		return;
	}
	
	// Create the file and write to it.
	pFile = fopen( testName, "w+" );
	numBytesWritten = fwrite( gData1, sizeof( char ), strlen( gData1 ), pFile );
	if( numBytesWritten != strlen( gData1 ) ){
		printFail( testName, "Unable to write to test file." );
		return;
	}

	// Now reopen the same file as read only and be sure we can't write to it.
	pFile = freopen( 0, "r", pFile );
	errno = 0;
	numBytesWritten = fwrite( gData2, sizeof( char ), strlen( gData2 ), pFile );
	if( errno != EBADF ){
		printFail( testName, "Expected EBADF trying to write in read-only mode." );
		return;
	}
	
	// Check the data in the file.
	numBytesRead = fread( bytesRead, sizeof( char ), strlen( gData1 ), pFile );
	if( strcmp( bytesRead, gData1 ) != 0 ){
		printFail( testName, "Data should not have changed." );
		return;
	}
	
	fclose( pFile );
	printPass( testName );
}
