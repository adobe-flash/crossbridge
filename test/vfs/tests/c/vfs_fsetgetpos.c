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

void runFsetgetposTests(){
	fsetgetpos_basic();
	fsetpos_ungetc();
	fsetpos_ebadf();
	fgetpos_ebadf();
}

/**
 * Create a file, write, read.
 **/
void fsetgetpos_basic(){
	const char* testName = "fsetgetpos_basic";
	const char* expectedResult = "abade0";
	fpos_t pos;
	fpos_t endPos;
	FILE* pFile = 0;
	int ret = -1;
	int theFile = -1;
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );

	// Write abcde.
	pFile = fopen( testName, "w+" );
	fwrite( gData1, sizeof( char ), strlen( gData1 ), pFile );
	
	// Seek to the end and store that position.
	fseek( pFile, 0, SEEK_END );
	errno = 0;
	ret = fgetpos( pFile, &endPos );
	
	if( (ret == 0) && (errno == 0) ){
		// Seek to position 2 and store that position
		fseek( pFile, 2, SEEK_SET );
		errno = 0;
		ret = fgetpos( pFile, &pos );

		if( (ret == 0) && (errno == 0) ){
			// fsetpos to the stored end position and write something.
			errno = 0;
			ret = fsetpos( pFile, &endPos );
			if( (ret == 0) && (errno == 0) ){
				fwrite( gData2, 1, 1, pFile );
				
				// fsetpos to the stored position 2 and write something.
				errno = 0;
				ret = fsetpos( pFile, &pos );
				if( (ret == 0) && (errno == 0) ){
					fwrite( gData1, 1, 1, pFile );
					fflush( pFile );

					// See what happened.
					theFile = open( testName, O_RDONLY );
					read( theFile, bytesRead, strlen( gData1 ) + 1 );
					close( theFile );

					if( strcmp( bytesRead, expectedResult ) == 0 ){
						printPass( testName );
					}else{
						printFail( testName, "Data did not match" );
					}
				}else{
					printFail( testName, "Second fsetpos() failed" );
				}
			}else{
				printFail( testName, "First fsetpos() failed" );
			}
		}else{
			printFail( testName, "Second fgetpos() failed" );
		}
	}else{
		printFail( testName, "First fgetpos() failed" );
	}

	fclose( pFile );
}

/**
 * fsetpos() should undo ungetc().  This is more of an integration test.
 **/
void fsetpos_ungetc(){
	const char* testName = "fsetpos_ungetc";
	const char* expected1 = "1";
	const char* expected2 = "abcde";
	FILE* pFile = 0;
	int ret = -1;	
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	fpos_t posStart;
	
	// Create a file.
	pFile = fopen( testName, "w+" );
	fwrite( gData1, sizeof( char ), strlen( gData1 ), pFile );
	fseek( pFile, 0, SEEK_SET );
	fgetpos( pFile, &posStart );	
	fflush( pFile );
	
	// Replace the "c" in the stream with a "1" using ungetc and verify.
	fseek( pFile, 2, SEEK_SET );
	ret = ungetc( '1', pFile );
	if( (ret == '1') ){
		fread( bytesRead, 1, 1, pFile );
		if( strcmp( bytesRead, expected1 ) == 0 ){
			// Now call fsetpos() and verify that the ungetc() was cleared.
			memset( bytesRead, 0, gBufLarge );
			errno = 0;
			ret = fsetpos( pFile, &posStart );
			if( (errno == 0) && (ret == 0) ){
				fread( bytesRead, 1, strlen( gData1 ), pFile );
				if( strcmp( bytesRead, expected2 ) == 0 ){
					printPass( testName );
				}
			}else{
				printFail( testName, "Call to fsetpos failed" );
			}
		}else{
			printFail( testName, "Expected to read \"1\"" );
		}
	}else{
		printFail( testName, "Call to ungetc failed" );
	}

	fclose( pFile );
}

/**
 * Trigger EBADF with fsetpos
 **/
void fsetpos_ebadf(){
	const char* testName = "fsetpos_ebadf";
	FILE* pFile;
	fpos_t pos;
	
	// Write.
	pFile = fopen( testName, "w+" );
	fgetpos( pFile, &pos );	
	fclose( pFile );
	errno = 0;
	int ret = fsetpos( pFile, &pos );

	if( (errno == EBADF) && (ret == EOF) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF and EOF" );
	}
}

/**
 * Trigger EBADF with fgetpos
 **/
void fgetpos_ebadf(){
	const char* testName = "fgetpos_ebadf";
	FILE* pFile;
	fpos_t pos;
	
	// Write.
	pFile = fopen( testName, "w+" );
	fclose( pFile );
	errno = 0;
	int ret = fgetpos( pFile, &pos );	
	
	if( (errno == EBADF) && (ret == EOF) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF and EOF" );
	}
}
