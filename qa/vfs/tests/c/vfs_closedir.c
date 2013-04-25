#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

void runClosedirTests(){
	closedir_basic();
	//closedir_twice(); Erring gracefully "may" be supported.  We don't.
}

/**
 * Call closedir() and verify the descriptor is invalid.
 **/
void closedir_basic(){
	const char* testName = "closedir_basic";
	const char* dirName = "/closedir_basic";
	DIR* dirPtr = 0;
	int ret = -1;
	int dirFileDesc = -1;	
		
	// Create a directory and open it.
	mkdir( dirName, 0777 );
	dirPtr = opendir( dirName );
	errno = 0;
	ret = closedir( dirPtr );
	
	if( (errno == 0) && (ret == 0) ){
		// Be sure it's invalid.
		dirFileDesc = dirfd( dirPtr );
		if( dirFileDesc == -1 ){
			printPass( testName );
		}else{
			printFail( testName, "Expected an invalid descriptor to be returned" );
		}
	}else{
		printFail( testName, "The closedir() call failed" );
	}	
}

/**
 * Call closedir() twice on the same DIR.
 **/
void closedir_twice(){
	const char* testName = "closedir_twice";
	DIR* dirPtr;
	int ret = -1;

	// Create a directory and open it.
	mkdir( testName, 0777 );
	dirPtr = opendir( testName );
	closedir( dirPtr );
	
	printFail( testName, "Fails due to ALC-417" );

	//closedir( dirPtr );	
}
