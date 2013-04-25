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

void runReaddirTests(){
	readdir_oneFile();
	readdir_filesAndDirs();
	//readdir_ebadf();
}

/**
 * This is a lot like getdirentries().
 **/
void readdir_oneFile(){
	const char* testName = "readdir_oneFile";
	const char* dirName = "/readdir_oneFile";
	const char* fileName = "/readdir_oneFile/foo.txt";
	DIR* dirPtr = 0;
	const char* expectedResult = "foo.txt";
	struct dirent* dirEntry = 0;
	
	// Make a directory with a file.
	mkdir( dirName, 0777 );
	int theFile = open( fileName, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	errno = 0;
	dirPtr = opendir( dirName );
	
	if( (errno == 0) && (dirPtr > 0) ){
		dirEntry = readdir( dirPtr );
		
		if( (dirEntry > 0) && (errno == 0) ){
			if( strcmp( dirEntry->d_name, expectedResult ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "File name was not correct" );
			}
		}else{
			printFail( testName, "Call to readdir() failed" );
		}
	}else{
		printFail( testName, "Unable to open the directory" );
	}
}

/**
 * Read several files and directories.
 **/
void readdir_filesAndDirs(){
	const char* testName = "/readdir_filesAndDirs";
	const char* mainDirName = "/readdir_filesAndDirs/main";
	const char* subdir1Name = "/readdir_filesAndDirs/main/dir1";
	const char* subdir2Name = "/readdir_filesAndDirs/main/directory 2";
	const char* subdir3Name = "/readdir_filesAndDirs/main/dir1/dir3"; // This should not get returned.
	const char* file1Name = "/readdir_filesAndDirs/main/file1";
	const char* file2Name = "/readdir_filesAndDirs/main/file number 2";
	const char* expected1 = "dir1";
	const char* expected2 = "directory 2";
	const char* expected3 = "file1";
	const char* expected4 = "file number 2";
	const char* ignore1 = ".";
	const char* ignore2 = "..";
	int found1, found2, found3, found4 = 0;
	
	int theFile = 0;
	DIR* dirPtr = 0;
	struct dirent* dirEntry;
	int numItems = 0;
	
	// Make a directory with files and directories.
	mkdir( testName, 0777 );
	mkdir( mainDirName, 0777 );
	mkdir( subdir1Name, 0777 );
	mkdir( subdir2Name, 0777 );
	mkdir( subdir3Name, 0777 );
	
	theFile = open( file1Name, O_CREAT | O_RDWR, S_IRWXU );
	write( theFile, gData1, 5 );
	close( theFile );
	
	theFile = open( file2Name, O_CREAT | O_RDWR, S_IRWXU );
	write( theFile, gData1, 5 );
	close( theFile );
	
	// Open the directory and get info. about it.
	dirPtr = opendir( mainDirName );
	
	while( dirEntry = readdir( dirPtr ) ){
		if( (strcmp( dirEntry->d_name, ignore1 ) == 0) || (strcmp( dirEntry->d_name, ignore2 ) == 0) ){
			//printf( "Ignoring %s\n", dirEntry->d_name );
		}else{		
			++numItems;

			// Check values.  A dirent only has d_ino (not supported) and d_name.
			if( strcmp( dirEntry->d_name, expected1 ) == 0 ){
				found1 = 1;
			}else if( strcmp( dirEntry->d_name, expected2 ) == 0 ){
				found2 = 1;
			}else if( strcmp( dirEntry->d_name, expected3 ) == 0 ){
				found3 = 1;
			}else if( strcmp( dirEntry->d_name, expected4 ) == 0 ){
				found4 = 1;
			}
		}
	}
	
	if( numItems == 4 ){
		if( found1 && found2 && found3 && found4 ){
			printPass( testName );
		}else{
			printFail( testName, "Missing an item" );
		}
	}else{
		printFail( testName, "Found wrong number of items" );
	}
	
	closedir( dirPtr );
}

/**
 * Trigger ebadf.
 * Commenting out this test.  It's not a required part of the API.  See https://bugs.adobe.com/jira/browse/ALC-416.
 **/
/**
void readdir_ebadf(){
	const char* testName = "readdir_ebadf";

	mkdir( testName, 0777 );
	DIR* dirPtr = opendir( testName );
	closedir( dirPtr );
	errno = 0;
	struct dirent* direntPtr = readdir( dirPtr );

	//printf( "error: %s, dirPtr: %lu, direntPtr: %lu\n", strerror( errno ), dirPtr, direntPtr );
	
	if( (direntPtr == 0) && (errno == EBADF) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected null pointer and EBADF." );
	}
}
**/


