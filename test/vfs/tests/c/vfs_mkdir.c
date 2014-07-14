#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * This file tests mkdir().
 * int mkdir(const char *path, mode_t mode);
 * Note that permissions (mode param) are ignored in flascc.
 **/

/**
 * Run the tests in this file.
 **/
void runMkdirTests(){
	mkdir_basic();
	mkdir_leadingSlash();
	mkdir_trailingSlash();
	mkdir_slashes();
	mkdir_addFile();
	mkdir_addDir();
	mkdir_eexist1();
	mkdir_eexist2();
	mkdir_eexist3();
	mkdir_dotdot();
	mkdir_mixedDots();
	mkdir_tooHigh();
	mkdir_pathElementMissing();
	mkdir_emptyString();
	mkdir_onlySlash();
	mkdir_enotdir();
	mkdir_fileTimes1();
	mkdir_fileTimes2();
	mkdir_rdonlyfs();
}

/**
 * Make a directory.
 **/
void mkdir_basic(){
	const char* testName = "mkdir_basic";
	errno = 0;
	int ret = mkdir( testName, 0777 );
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( testName ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Could not find the directory" );
		}
	}else{
		printFail( testName, "Failed to create the directory.  Fails due to ALC-359 (fixed)." );
	}
	
}

/**
 * Make a directory with a leading slash.
 **/
void mkdir_leadingSlash(){
	const char* testName = "mkdir_leadingSlash";
	const char* dirName = "/mkdir_leadingSlash";
	
	errno = 0;
	int ret = mkdir( dirName, 0777 );
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( dirName ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Could not find the directory" );
		}
	}else{
		printFail( testName, "Failed to create the directory." );
	}
	
}

/**
 * Make a directory with a trailing slash.
 **/
void mkdir_trailingSlash(){
	const char* testName = "mkdir_trailingSlash";
	const char* dirName = "mkdir_trailingSlash/";
	errno = 0;
	int ret = mkdir( dirName, 0777 );
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( dirName ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Could not find the directory" );
		}
	}else{
		printFail( testName, "Failed to create the directory.  Fails due to ALC-359 and possibly ALC-364 (fixed)." );
	}	
}

/**
 * Make a directory with leading and trailing slashes.
 **/
void mkdir_slashes(){
	const char* testName = "mkdir_slashes";
	const char* dirName = "/mkdir_slashes/";
	errno = 0;
	int ret = mkdir( dirName, 0777 );
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( dirName ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Could not find the directory" );
		}			
	}else{
		printFail( testName, "Failed to create the directory.  Fails due to ALC-359 and possibly ALC-364 (fixed)." );
	}	

}

/**
 * Add a file to a directory created by mkdir and open it. 
 * This is done in *many* other test cases but I'm doing it again 
 * here so we can run the mkdir() tests and know we hit the
 * test case.
 **/
void mkdir_addFile(){
	const char* testName = "mkdir_addFile";
	const char* dirName = "/mkdir_addFile";
	const char* fileName = "/mkdir_addFile/foo.txt";
	int theFile = 0;
	errno = 0;
	int ret = mkdir( dirName, 0777 );
	if( (errno == 0) && (ret == 0) ){
		errno = 0;
		theFile = open( fileName, O_RDWR | O_CREAT );
		close( theFile );

		if( errno == 0 && theFile > -1 ){
			if( searchForFile( fileName ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "Could not find the file" );
			}			
		}else{
			printFail( testName, "Failed to open the file." );
		}
	}else{
		printFail( testName, "Failed to create the directory." );
	}
}

/**
 * Add a directory to a directory.  Again, this is done in lots of
 * places, but I like having a defined test for it...
 **/
void mkdir_addDir(){
	const char* testName = "mkdir_addDir";
	const char* dir1Name = "/mkdir_addDir1";
	const char* dir2Name = "/mkdir_addDir1/mkdir_addDir2";

	int ret = mkdir( dir1Name, 0 );
	errno = 0;
	ret = mkdir( dir2Name, 0 );
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( dir2Name ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Could not find the directory" );
		}
	}else{
		printFail( testName, "Failed to create the directory." );
	}
}

/**
 * Try to make a directory which already exists as a directory.
 **/
void mkdir_eexist1(){
	const char* testName = "mkdir_eexist1";
	const char* dirName = "/mkdir_eexist1";

	int ret = mkdir( dirName, 0777 );
	errno = 0;	
	ret = mkdir( dirName, 0777 );
	if( (errno == EEXIST) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EEXIST and -1." );
	}
}

/**
 * Try to make a directory which already exists as a file.
 **/
void mkdir_eexist2(){
	const char* testName = "mkdir_eexist2";
	const char* dirName = "/mkdir_eexist2";
	const char* fileName = "/mkdir_eexist2/foo.txt";
	
	mkdir( dirName, 0777 );
	int theFile = open( fileName, O_RDWR | O_CREAT );
	close( theFile );
		
	errno = 0;		
	int ret = mkdir( fileName, 0777 );

	if( (errno == EEXIST) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EEXIST and -1." );
	}
}

/**
 * Put a . in the path.  This should cause EEXIST.
 **/
void mkdir_eexist3(){
	const char* testName = "mkdir_eexist3";
	const char* dir1Name = "/mkdir_eexist3";
	const char* dir2Name = "/mkdir_eexist3/.";
	
	mkdir( dir1Name, 0777 );
	errno = 0;
	int ret = mkdir( dir2Name, 0777 );
	if( (errno == EEXIST) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EEXIST and -1." );
	}
}


/**
 * Put a .. in the path.
 **/
void mkdir_dotdot(){
	const char* testName = "mkdir_dotdot";
	const char* dir1Name = "/mkdir_dotdot";
	const char* dir2Name = "/mkdir_dotdot/subdir";
	const char* dir3Name = "/mkdir_dotdot/subdir/../subdir2"; 
	const char* expectedDir = "/mkdir_dotdot/subdir2"; 
	
	mkdir( dir1Name, 0777 );
	mkdir( dir2Name, 0777 );
	errno = 0;
	int ret = mkdir( dir3Name, 0777 );
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( expectedDir ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Could not find the directory" );
		}
	}else{
		printFail( testName, "Expected errno and ret to be 0.  Fails due to ALC-345 (fixed)." );

	}
}

/**
 * Combine . and ..
 **/
void mkdir_mixedDots(){
	const char* testName = "mkdir_mixedDots";
	const char* dir1Name = "/mkdir_mixedDots";
	const char* dir2Name = "/mkdir_mixedDots/subdir";
	const char* dir3Name = "/mkdir_mixedDots/subdir/.././subdir/./../subdir2";
	const char* dirExpected = "/mkdir_mixedDots/subdir2";
	
	mkdir( dir1Name, 0777 );
	mkdir( dir2Name, 0777 );
	errno = 0;
	int ret = mkdir( dir3Name, 0777 );
	if( (errno == 0) && (ret == 0 ) ){
		if( searchForDir( dirExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Could not find the directory" );
		}	
	}else{
		printFail( testName, "Expected errno and ret to be 0.  Fails due to ALC-345 (fixed)." );
	}
}

/**
 * Try to .. above the root directory.  A native app just stays at the root.
 **/
void mkdir_tooHigh(){
	const char* testName = "mkdir_tooHigh";
	const char* dirName = "/../mkdir_tooHigh";
	const char* dirExpected = "/mkdir_tooHigh";

	errno = 0;
	int ret = mkdir( dirName, 0777 );
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( dirExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Could not find the directory" );
		}	
	}else{
		printFail( testName, "Expected errno and ret to be 0.  Fails due to ALC-345 (fixed)." );		
	}
}

/**
 * A component of the path is missing.
 **/
void mkdir_pathElementMissing(){
	const char* testName = "mkdir_pathElementMissing";
	const char* dir1Name = "/mkdir_pathElementMissing";
	const char* dir2Name = "/mkdir_pathElementMissing/invalid/foo";
	
	mkdir( dir1Name, 0777 );
	errno = 0;		
	int ret = mkdir( dir2Name, 0777 );

	if( (errno == ENOENT) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ENOENT and -1." );
	}
}

/**
 * The path is an empty string.
 **/
void mkdir_emptyString(){
	const char* testName = "mkdir_emptyString";
	const char* dirName = "";
	
	int ret = mkdir( dirName, 0777 );
	
	if( (errno == ENOENT) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ENOENT and -1.  Fails due to ALC-365. (Fixed)" );	
	}
}

/**
 * The path is a slash.
 **/
void mkdir_onlySlash(){
	const char* testName = "onlySlash";
	const char* dirName = "/";
	
	errno = 0;
	int ret = mkdir( dirName, 0777 );
	
	if( (errno == EEXIST) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EEXIST and -1" );
	}
}


/**
 * An element in the path is a file but is being treated like a directory.
 **/
void mkdir_enotdir(){
	const char* testName = "mkdir_enotdir";
	const char* dir1Name = "/mkdir_enotdir";
	const char* fileName = "/mkdir_enotdir/file";
	const char* dir2Name = "/mkdir_enotdir/file/subdir";
	
	mkdir( dir1Name, 0777 );
	int theFile = open( fileName, O_RDWR | O_CREAT );
	close( theFile );

	errno = 0;
	int ret = mkdir( dir2Name, 0777 );
	if( (errno == ENOTDIR) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ENOTDIR and -1.  Fails due to ALC-356." );

	}
}

/**
 * Confirm that the directory has correct time properties when created.
 **/
void mkdir_fileTimes1(){
	const char* testName = "mkdir_fileTimes1";
	printFail( testName, "Fails due to ALC-358" );
}

/**
 * Confirm that the directory's parent has correct time properties.
 **/
void mkdir_fileTimes2(){
	const char* testName = "mkdir_fileTimes2";
	printFail( testName, "Fails due to ALC-358" );
}

/**
 * Try to create a directory in the read only file system.
 **/
void mkdir_rdonlyfs(){
	const char* testName = "mkdir_rdonlyfs";
	const char* expectedError = "Read-only file system";	
	char* error = 0;	
	
	errno = 0;
	int ret = mkdir( "compressed_readonly/mkdir_rdonlyfs", 0777 );
	int savedErrno = errno;
	
	if( ret != -1 ){
		printFail( testName, "Should not have been able to call mkdir on the read only file system." );
		return;
	}
	
	if( savedErrno != 30 || strcmp( error = strerror( savedErrno ), expectedError ) != 0 ){
		printFail( testName, "The error was not correct." );
		return;
	}
	
	printPass( testName );
	return;
}
