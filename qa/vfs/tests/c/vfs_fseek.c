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
 * NOTE: Just do a few sanity tests.  This uses lseek.
 **/
void runFseekTests(){
	fseek_set();
	fseek_end();
	fseek_cur();
}

/**
 * Basic test for SEEK_SET.
 **/
void fseek_set(){
	const char* testName = "fseek_set";
	const char* bytesExpected = "a0cde";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	FILE* filePtr = fopen( testName, "r+" );  // r+ is read/write without truncating.
	errno = 0;
	
	// Seek
	ret = fseek( filePtr, 1, SEEK_SET );
	
	if( (ret == 0) && (errno == 0) ){
		// Write a character and verify.
		fwrite( gData2, 1, 1, filePtr );
		fflush( filePtr );
		
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) );
		close( theFile );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 0" );
	}
	
	fclose( filePtr );
}

/**
 * Basic test for SEEK_END.
 **/
void fseek_end(){
	const char* testName = "fseek_end";
	const char* bytesExpected = "abcd0";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	FILE* filePtr = fopen( testName, "r+" );
	errno = 0;
	
	// Seek
	ret = fseek( filePtr, -1, SEEK_END );
	
	if( (ret == 0) && (errno == 0) ){
		// Write a character and verify.
		fwrite( gData2, 1, 1, filePtr );
		fflush( filePtr );

		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) );
		close( theFile );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 0" );
	}
	
	fclose( filePtr );
}

/**
 * Basic test for SEEK_CUR.
 **/
void fseek_cur(){
	const char* testName = "fseek_cur";
	const char* bytesExpected = "abc0e";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	FILE* filePtr = fopen( testName, "r+" );
	
	// Write 2 bytes, then fseek ahead 1 more.
	fwrite( gData1, 1, 2, filePtr );
	errno = 0;
	ret = fseek( filePtr, 1, SEEK_CUR );
	
	if( (ret == 0) && (errno == 0) ){
		// Write a character and verify.
		fwrite( gData2, 1, 1, filePtr );
		fflush( filePtr );
		
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) + 1 );
		close( theFile );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			//printf( "expected: %s, actual: %s\n", bytesExpected, bytesRead );
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 0" );
	}
	
	close( theFile );

}


