#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * NOTE: There are only a couple basic tests here; this is not done
 * being tested.
 * NOTE: This method only works on files, not directories.
 **/

void runUnlinkTests(){
	unlink_file();
	unlink_enoent();
	unlink_enotdir();
	unlink_recreate();
	unlink_rdonlyfs();
}

/**
 * Unlink a file and be sure it isn't there.
 **/
void unlink_file(){
	const char* testName = "unlink_file";
	int theFile = -1;
	int ret = 0;
	struct stat statbuf;
	
	theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	errno = 0;
	ret = stat( testName, &statbuf );
	
	if( (ret == 0) && (errno == 0) ){ 
		ret = unlink( testName );

		if( (ret == 0) && (errno == 0) ){
			ret = stat( testName, &statbuf );
			if( (ret == -1) && (errno == ENOENT) ){
				printPass( testName );
			}else{
				printFail( testName, "Expected ret = -1 and errno = ENOENT" );
			}
		}else{
			printFail( testName, "Unlink() failed." );
		}
	}else{
		printFail( testName, "Failed to create the test file" );
	}
}

/**
 * Unlink a file that does not exist.
 **/
void unlink_enoent(){
	const char* testName = "unlink_enoent";
	
	errno = 0;
	int ret = unlink( testName );
	
	if( (ret == -1) && (errno == ENOENT) ){
		printPass( testName );
	}else{
		printFail( testName, "Unlink should have failed" );
	}
}

/**
 * "A component of the path prefix is not a directory."
 **/
void unlink_enotdir(){
	const char* testName = "unlink_enotdir";
	const char* dir1Name = "unlink_enotdir";
	const char* fileName = "unlink_enotdir/foo";
	const char* dir2Name = "unlink_enotdir/foo/foodir";
	
	mkdir( dir1Name, 0777 );
	
	int theFile = open( fileName, O_RDWR | O_CREAT );
	close( theFile );
	
	errno = 0;
	int ret = unlink( dir2Name );
	
	if( (ret == -1) && (errno == ENOTDIR) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ret == -1 and errno == ENOTDIR.  Fails due to ALC-400. (Fixed)" );
	}
}

/**
 * Unlink a file and create a new one with the same name (typical use case).
 **/
void unlink_recreate(){
	const char* testName = "unlink_recreate";
	int theFile = -1;
	int ret = 0;
	struct stat statbuf;
	
	theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	unlink( testName );
	
	if( stat( testName, &statbuf ) == -1 ){
		theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
		write( theFile, gData1, strlen( gData1 ) );
		close( theFile );
		
		if( stat( testName, &statbuf ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "File was not recreated" );
		}
	}else{
		printFail( testName, "File was not removed" );
	}
}

/**
 * Try to call unlink on the read only file system.
 **/
void unlink_rdonlyfs(){
	const char* testName = "unlink_rdonlyfs";
	const char* expectedError = "Read-only file system";	
	char* error = 0;	
	struct stat statbuf;
	
	errno = 0;
	int ret = unlink( "compressed_readonly/basic");
	int savedErrno = errno;
	
	if( ret != -1 ){
		printFail( testName, "Should not have been able to call unlink on the read only file system." );
		return;
	}
	
	if( savedErrno != 30 || strcmp( error = strerror( savedErrno ), expectedError ) != 0 ){
		printFail( testName, "The error was not correct." );
		return;
	}
	
	// Be sure the file is still there.
	if( stat( "compressed_readonly/basic", &statbuf ) != 0 ){
		printFail( testName, "The file is gone." );
		return;
	}
	
	printPass( testName );
	return;
}