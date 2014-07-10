#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * This file tests close(). 
 **/

/**
 * Run the tests in this file.
 **/
void runCloseTests(){
	close_basic();
	close_twice();
	close_multipleRefs();
}

/**
 * Close a file and try to reuse the fildes.
 **/
void close_basic(){
	const char* testName = "close_basic";
	int theFile = -1;
	int flags = -1;
	int writeRet = 0;
	
	// Create a file.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	close( theFile );
	
	// Write
	errno = 0;
	writeRet = write( theFile, gData1, strlen( gData1 ) );
	
	if( (errno == EBADF) && (writeRet == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF and a return value of -1" );
	}
}

/**
 * Close a file and try to reuse the fildes by closing it.
 **/
void close_twice(){
	const char* testName = "close_twice";
	int theFile = -1;
	int flags = -1;
	int closeRet = 0;
	
	// Create a file.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	closeRet = close( theFile );

	if( closeRet == 0 ){
		errno = 0;
		closeRet = close( theFile );
		
		if( (errno == EBADF) && (closeRet == -1) ){
			printPass( testName );
		}else{
			printFail( testName, "Expected EBADF and a return value of -1" );
		}
	}else{
		printFail( testName, "Expected return value of 0" );
	}
}

/**
 * Open a file several times and close just some of them.
 * This is also a general test for multiple file descriptors
 * for the same file.
 **/
void close_multipleRefs(){
	const char* testName = "close_multipleRefs";
	int theFileCREAT = -1;
	int theFileWRONLY = -1;
	int theFileRDONLY = -1;
	int ret = 0;
	char bytesRead[ gBufSmall ];
	char bytesExpected[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	memset( bytesExpected, 0, gBufSmall );
	strcpy( bytesExpected, gData1 );
	
	// Create a file.
	theFileCREAT = open( testName, O_CREAT );
	theFileWRONLY = open( testName, O_WRONLY );
	theFileRDONLY = open( testName, O_RDONLY );

	ret = close( theFileCREAT );
	if( ret == 0 ){
		ret = write( theFileWRONLY, gData1, strlen( gData1 ) );
		if( ret == strlen( gData1 ) ){	
			ret = close( theFileWRONLY );
			if( ret == 0 ){
				ret = read( theFileRDONLY, bytesRead, strlen( gData1 ) );
				if( ret == strlen( gData1 ) ){
					ret = close( theFileRDONLY );
					if( ret == 0 ){
						// Now further access should fail.
						ret = close( theFileCREAT );
						if( (ret == -1) && (errno == EBADF) ){
							printPass( testName );
						}else{
							printFail( testName, "Expected final return of -1 and errno of EBADF" );
						}
					}else{
						printFail( testName, "Third close was not successful" );
					}
				}else{
					printFail( testName, "Read was not successful" );
				}
			}else{
				printFail( testName, "Second close was not successful" );
			}
		}else{
			printFail( testName, "Write was not successful" );
		}
	}else{
		printFail( testName, "First close was not successful" );
	}

	// Just in case...
	close( theFileWRONLY );
	close( theFileRDONLY );
}