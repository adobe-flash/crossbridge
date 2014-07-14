#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * This file tests write().
 * ssize_t write(int fildes, const void *buf, size_t nbyte);
**/

/**
 * Run the tests in this file.
 **/
void runWriteTests(){

	// Write to a new file: Done by open() tests.
	// Open as APPEND and write: Done by open() tests.
	// Open as RDONLY and write: Done by open() tests.
	// Write beyond the end of the file: Done by read() tests.

	write_overwrite();
	write_several();
	write_subsequentOverwrite();
	write_subsequentAppend();
	write_read();
	write_nByteTooBig();
	write_nByteZero();
	write_nByteZeroError();
	write_timeFields();
	write_ebadf();
}

/**
 * Overwrite a byte in a file.
 **/
void write_overwrite(){
	const char* testName = "write_overwrite";
	const char* bytesExpected = "ab0de";
	int theFile = -1;
	int retVal = 0;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	
	// Overwrite the third byte of the file with the first byte of gData2.
	lseek( theFile, 2, SEEK_SET );
	retVal = write( theFile, gData2, 1 );
	close( theFile );
	
	// See what we got.
	theFile = open( testName, O_RDWR | O_CREAT );
	read( theFile, bytesRead, gBufSmall );
	close( theFile );
	
	if( (retVal == 1) ){
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not as expected" );
		}
	}else{
		printFail( testName, "Expected retVal to show one byte written" );
	}
}

/**
 * Write several bytes, overwriting, extending the file.
 **/
void write_several(){
	const char* testName = "write_several";
	const char* bytesExpected = "ab01234";
	int theFile = -1;
	int retVal = 0;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	
	// Starting at the third byte, overwrite with 5 bytes from gData2.
	lseek( theFile, 2, SEEK_SET );
	retVal = write( theFile, gData2, 5 );
	close( theFile );
	
	// See what we got.
	theFile = open( testName, O_RDWR | O_CREAT );
	read( theFile, bytesRead, gBufSmall );
	close( theFile );
	
	if( (retVal == 5) ){
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not as expected" );
		}
	}else{
		printFail( testName, "Expected retVal to show one byte written" );
	}
}

/**
 * Write several times without seeking; verify the file offset is incremented.
 **/
void write_subsequentOverwrite(){
	const char* testName = "write_subsequentOverwrite";
	const char* bytesExpected = "aababc6789";
	int theFile = -1;
	int retVal = 0;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData2, strlen( gData2 ) ); // "0123456789"
	close( theFile );
	
	// Do subsequent writes.
	theFile = open( testName, O_RDWR );
	retVal = write( theFile, gData1, 1 ); // "a123456789"
	if( retVal == 1 ){
		retVal = write( theFile, gData1, 2 ); // "aab3456789"
		if( retVal == 2 ){
			retVal = write( theFile, gData1, 3 ); // "aababc6789"
			close( theFile );
			if( retVal == 3 ){
				// See what we got.
				theFile = open( testName, O_RDWR );
				read( theFile, bytesRead, gBufSmall );
				
				if( strcmp( bytesRead, bytesExpected ) == 0 ){
					printPass( testName );
				}else{
					printFail( testName, "Data was not as expected" );
				}
			}else{
				printFail( testName, "Expected to write three bytes" );
			}
		}else{
			close( theFile );
			printFail( testName, "Expected to write two bytes" );
		}
	}else{
		close( theFile );
		printFail( testName, "Expected to write one byte" );
	}	
}

/**
 * Write several times without seeking; verify the file offset is incremented.
 **/
void write_subsequentAppend(){
	const char* testName = "write_subsequentAppend";
	const char* bytesExpected = "0123450123012";
	int theFile = -1;
	int retVal = 0;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) ); // "abcde"
	close( theFile );
	
	// Do subsequent writes.
	theFile = open( testName, O_RDWR );
	retVal = write( theFile, gData2, 6 ); // "012345"
	if( retVal == 6 ){
		retVal = write( theFile, gData2, 4 ); // "0123450123"
		if( retVal == 4 ){
			retVal = write( theFile, gData2, 3 ); // "0123450123012"
			close( theFile );
			if( retVal == 3 ){
				// See what we got.
				theFile = open( testName, O_RDWR );
				read( theFile, bytesRead, gBufSmall );
				if( strcmp( bytesRead, bytesExpected ) == 0 ){
					printPass( testName );
				}else{
					printFail( testName, "Data was not as expected" );
				}
			}else{
				printFail( testName, "Expected to write three bytes" );
			}
		}else{
			close( theFile );
			printFail( testName, "Expected to write two bytes" );
		}
	}else{
		close( theFile );
		printFail( testName, "Expected to write one byte" );
	}	
}

/**
 * Write and immediately read.  Be sure the read is from the new offset.
 **/
void write_read(){
	const char* testName = "write_read";
	const char* bytesExpected = "c";
	int theFile = -1;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) ); // "abcde"
	close( theFile );
	
	// Write and read.  The read should be the byte after the write.
	theFile = open( testName, O_RDWR );
	write( theFile, gData2, 2 ); // "01cde"
	read( theFile, bytesRead, 1 ); // c
	close( theFile );
	
	// See what we got.
	if( strcmp( bytesRead, bytesExpected ) == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Data was not as expected" );
	}
}

/**
 * Write with the bytes to write greater than the bytes in the buffer
 * we're writing from.
 **/
void write_nByteTooBig(){
	const char* testName = "write_nByteTooBig";
	int theFile = -1;
	char bytesRead[ gBufLarge ];
	char bytesExpected[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	memset( bytesExpected, 0, gBufLarge );
	strcpy( bytesExpected, gData1 );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, 2 * (strlen( gData1 )) );
	close( theFile );
	
	// We should have gData1 and a bunch of 0's.
	theFile = open( testName, O_RDWR );
	read( theFile, bytesRead, 2 * (strlen( gData1 )) );
	close( theFile );
	
	if( strcmp( bytesRead, bytesExpected ) == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Data was not as expected" );
	}
}

/**
 * Write with nbytes set to 0.
 **/
void write_nByteZero(){
	const char* testName = "write_nByteZero";
	int theFile = -1;
	char bytesRead[ gBufLarge ];
	char bytesExpected[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	memset( bytesExpected, 0, gBufLarge );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, 0 );
	close( theFile );
	
	// Read the nothingness.  How Zen.
	theFile = open( testName, O_RDWR );
	read( theFile, bytesRead, gBufLarge );
	close( theFile );
	
	if( strcmp( bytesRead, bytesExpected ) == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Data was not as expected" );
	}
}

/**
 * Write with nbytes set to 0 and experience a problem.
 * (e.g. Perform a test write().)
 **/
void write_nByteZeroError(){
	const char* testName = "write_nByteZeroError";
	int theFile = -1;
	int retVal = 0;
	int savedErrno = 0;
	char bytesRead[ gBufLarge ];
	char bytesExpected[ gBufLarge ];
	memset( bytesRead, 0, gBufLarge );
	memset( bytesExpected, 0, gBufLarge );
	
	// Create a file for testing.
	theFile = open( testName, O_RDONLY | O_CREAT );
	errno = 0;
	retVal = write( theFile, gData1, 0 );
	savedErrno = errno;
	close( theFile );
	
	// See what's in the file now (hopefully nothing!)
	theFile = open( testName, O_RDWR );
	read( theFile, bytesRead, gBufLarge );
	close( theFile );
	
	if( retVal == -1 ){
		if( errno == EBADF ){
			if( strcmp( bytesRead, bytesExpected ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "Data was not as expected" );
			}
		}else{
			printFail( testName, "Expected EBADF" );
		}
	}else{
		printFail( testName, "Expected return of -1" );
	}
}

/**
 * Verify that time fields are updated when writing.
 * Currently not implemented; see ALC-358/
 **/
void write_timeFields(){
	const char* testName = "write_timeFields";
	printFail( testName, "Fails due to ALC-358" );
}

/**
 * Pass in an invalid file descriptor.
 **/
void write_ebadf(){
	const char* testName = "write_ebadf";
	int theFile = -1;
	int retVal = 0;
	
	errno = 0;
	retVal = write( theFile, gData1, 1 );

	if( errno == EBADF ){
		if( retVal == -1 ){
			printPass( testName );
		}else{
			printFail( testName, "Expected retVal to be -1" );
		}
	}else{
		printFail( testName, "Expected EBADF" );	
	}
}