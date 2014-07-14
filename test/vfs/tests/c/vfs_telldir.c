#include "../../framework/vfs.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/**
 * Adding a test for telldir() (from an email thread).
 **/

void runTelldirTests(){
	//telldir_manyFiles();
}

/**
 * Call telldir() while reading a director with many files.
 **/
void telldir_manyFiles(){
	const char* testName = "/Users/rv/telldir_manyFiles";
	char savedName[ gBufSmall ];
	char* savedNamePtr = savedName;
	DIR* dirPtr = 0;
	struct dirent* dirEntry = 0;
	long savedPosition = 0;

	memset( savedName, 0, gBufSmall );
	mkdir( "/Users", 0777 );
	mkdir( "/Users/rv", 0777 );
	mkdir( testName, 0777 );
	
	createFiles( testName, gData1, 131072 );
	dirPtr = opendir( testName );

	// Read some entries to advance.
	readdir( dirPtr );
	readdir( dirPtr );
	readdir( dirPtr );
	
	// Save the file where we are.
	savedPosition = telldir( dirPtr );
	dirEntry = readdir( dirPtr );
	strcpy( savedName, dirEntry->d_name );
	
	// Advance further.
	readdir( dirPtr );
	readdir( dirPtr );
	readdir( dirPtr );

	// Be sure we aren't just sitting on the same file.
	dirEntry = readdir( dirPtr );
	if( strcmp( savedName, dirEntry->d_name ) != 0 ){

		// Use seekdir to go back to the position stored when we used telldir()
		seekdir( dirPtr, savedPosition );					
		dirEntry = readdir( dirPtr );
		if( strcmp( savedName, dirEntry->d_name ) == 0 ){
			printPass( testName );
		}else{
			printf( "Ended up at file %s\n", dirEntry->d_name );
			printFail( testName, "Wrong file" );
		}
	}else{
		printFail( testName, "Looks like we just stayed in one place" );
	}
}











