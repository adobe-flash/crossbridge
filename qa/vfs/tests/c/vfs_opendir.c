#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

using namespace std;

/**
 * NOTE: This is to be tested lightly.
 **/

void runOpendirTests(){
	opendir_basic();
	opendir_enoent();
	opendir_enotdir();
}

/**
 * Basic test to open a directory with opendir().
 **/
void opendir_basic(){
	const char* testName = "opendir_basic";
	const char* dirName = "/opendir_basic";
	const char* fileName = "/opendir_basic/foo.txt";
	DIR* dirPtr = 0;
	int dirFileDesc = -1;
	char* buf = (char*) calloc( gBufBlockSize, 1 );
	long basep = 0;
	int ret = -1;
	struct dirent* dirEntry = 0;
	const char* expectedFile = "foo.txt";

	// Create a directory with a file in it.
	mkdir( dirName, 0777 );
	int theFile = open( fileName, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	errno = 0;
	dirPtr = opendir( dirName );

	if( (errno == 0) && (dirPtr > 0) ){

		// Be sure it's valid.
		dirFileDesc = dirfd( dirPtr );
		ret = getdirentries( dirFileDesc, buf, gBufBlockSize, &basep );
		
		if( (ret > 0) && (errno == 0) ){
			dirEntry = (struct dirent*)buf;
			if( strcmp( dirEntry->d_name, expectedFile ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "File name was not correct." );
			}
		}else{
			printFail( testName, "Expected ret > 0 and errno = 0" );
		}			
	}else{
		printFail( testName, "The opendir() call failed." );
	}	
}

/**
 * Call opendir with a path which does not exist.
 **/
void opendir_enoent(){
	const char* testName = "opendir_enoent";
	errno = 0;
	DIR* dirPtr = opendir( "opendir_enoent_invalid" );
	
	if( (dirPtr == 0) && (errno == ENOENT) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected null pointer and ENOENT" );
	}
}

/**
 * Call opendir with a path which has a file treated like a directory.
 **/
void opendir_enotdir(){
	const char* testName = "opendir_enotdir";
	const char* dir1Name = "opendir_enotdir";
	const char* fileName = "opendir_enotdir/file";
	const char* dir2Name = "opendir_enotdir/file/subdir";

	mkdir( dir1Name, 0777 );

	int theFile = open( fileName, O_RDWR | O_CREAT, S_IRWXU );
	close( theFile );

	errno = 0;
	DIR* dirPtr = opendir( dir2Name );

	if( (dirPtr == 0) && (errno == ENOTDIR) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected null pointer and ENOTDIR.  Fails due to ALC-413." );
	}
}
