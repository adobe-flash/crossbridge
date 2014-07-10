#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * NOTE: Test lightly.
 **/

void runPrintfTests(){
	printf_noflags();
	printf_types();
	printf_fieldwidth();
	printf_precision();
	printf_conversionMod();
}


/**
 * Print w/o flags.
 **/
void printf_noflags(){
	const char* testName = "printf_noflags";
	char bytesRead[ gBufLarge ];
	int theFile = -1;
	FILE* pFile = fopen( testName, "w+" );
	errno = 0;
	
	int ret = fprintf( pFile, gData1 );
	int savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == 0) && (ret == strlen( gData1 )) ){
		memset( bytesRead, 0, gBufLarge );
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, strlen( gData1 ) );
		close( theFile );
		
		if( strcmp( bytesRead, gData1 ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not correct" );
		}
		
	}else{
		printFail( testName, "Expected errno == 0 and to return the number of bytes." );
	}
}

/**
 * Print some types.
 **/
void printf_types(){
	const char* testName = "printf_types";
	const char* bytesExpected = "hello, 999999999999.000000, 4";
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	double theDouble = 999999999999.0;
	int theInt = 4;
	const char* theString = "hello";
	
	int theFile = -1;
	FILE* pFile = fopen( testName, "w+" );
	errno = 0;

	int ret = fprintf( pFile, "%s, %f, %d", theString, theDouble, theInt );	
	int savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == 0) && (ret == 29) ){

		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, gBufLarge );
		close( theFile );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not correct" );
		}
		
	}else{
		printFail( testName, "Expected errno == 0 and to return the number of bytes." );
	}
}


/**
 * Specify a field width.
 **/
void printf_fieldwidth(){
	const char* testName = "printf_fieldwidth";
	const char* bytesExpected = "         4";
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	int theInt = 4;

	int theFile = -1;
	FILE* pFile = fopen( testName, "w+" );
	errno = 0;
	
	int ret = fprintf( pFile, "%10d", theInt );	
	int savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == 0) && (ret == 10) ){		
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, gBufLarge );
		close( theFile );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not correct" );
		}
		
	}else{
		printFail( testName, "Expected errno == 0 and to return the number of bytes." );
	}
}

/**
 * Specify a precision.
 **/
void printf_precision(){
	const char* testName = "printf_precision";
	const char* bytesExpected = "2.0000";
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	float theFloat = 2.0;
	
	int theFile = -1;
	FILE* pFile = fopen( testName, "w+" );
	errno = 0;
	
	int ret = fprintf( pFile, "%.4f", theFloat );	
	int savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == 0) && (ret == 6) ){		
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, gBufLarge );
		close( theFile );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not correct" );
		}
		
	}else{
		printFail( testName, "Expected errno == 0 and to return the number of bytes." );
	}
}

/**
 * Specify a conversion modifier.
 **/
void printf_conversionMod(){
	const char* testName = "printf_conversionMod";
	const char* bytesExpected = "0xf";
	char bytesRead[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	
	int theInt = 15;
	
	int theFile = -1;
	FILE* pFile = fopen( testName, "w+" );
	errno = 0;
	
	int ret = fprintf( pFile, "0x%x", theInt );
	int savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == 0) && (ret == 3) ){
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, gBufLarge );
		close( theFile );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not correct" );
		}
		
	}else{
		printFail( testName, "Expected errno == 0 and to return the number of bytes." );
	}
}
