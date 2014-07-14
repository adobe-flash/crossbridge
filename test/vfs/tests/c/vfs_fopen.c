#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**f
 * NOTE: Test lightly only.
 **/
void runFopenTests(){
	fopen_basic();
	fopen_enoent();
	fopen_enotdir();
	fopen_einval();
	fopen_eisdir();
	fopen_emptyName();
}

/**
 * Use fopen() to create a file, then use stat() to confirm
 * that it's there.
 **/
void fopen_basic(){
	const char* testName = "fopen_basic";
	FILE* pFile = 0;
	int savedErrno = 0;
	struct stat statbuf;

	errno = 0;
	pFile = fopen( testName, "w+" );
	savedErrno = errno;
	fclose( pFile );
	
	if( (savedErrno == 0) && (pFile != 0) ){
		if( stat( testName, &statbuf ) == 0 ){
			printPass( testName );
		}
	}else{
		printFail( testName, "The fopen() call failed" );
	}
}

/**
 * Trigger enoent.
 **/
void fopen_enoent(){
	const char* testName = "fopen_enoent";
	errno = 0;
	FILE* pFile = fopen( "fopen_enoent_invalid", "r" );
	
	if( (errno == ENOENT) && (!pFile) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected fopen to fail" );
	}
}

/**
 * Trigger enotdir.
 **/
void fopen_enotdir(){
	const char* testName = "fopen_enotdir";
	const char* dir1 = "/fopen_enotdir";
	const char* file1 = "/fopen_enotdir/file1";
	const char* file2 = "/fopen_enotdir/file1/file2";
	int theFile = -1;

	mkdir( dir1, 0777 );
	theFile = open( file1, O_RDWR | O_CREAT, S_IRWXU );
	close( theFile );

	errno = 0;
	FILE* pFile = fopen( file2, "r" );

	if( (errno == ENOTDIR) && (!pFile) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ENOTDIR.  Fails due to ALC-414. (Fixed)" );
	}
}

/**
 * Trigger EINVAL
 **/
void fopen_einval(){
	const char* testName = "fopen_einval";
	errno = 0;
	FILE* pFile = fopen( "fopen_einval", "hotdog" );
	
	if( (errno == EINVAL) && (!pFile) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected fopen to fail" );
	}
}

/**
 * Use fopen() and fread() on a directory.  Setting EISDIR
 * is not documented AFAIK, but that's what happens in a
 * native app.
 **/
void fopen_eisdir(){
	const char* testName = "fopen_eisdir";
	
	mkdir( testName, 0777 );
	FILE *fin = fopen( testName, "r" );
	errno = 0;
	char data[ gBufSmall ];
	char* dataPtr = data;
	memset( dataPtr, 0, gBufSmall );
	int result = fread(&data, 1, 1, fin);

	if( errno == EISDIR ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EISDIR.  (ALC-418, fixed)" );
	}
}

/**
 * Empty file name
 **/
void fopen_emptyName(){
	const char* testName = "fopen_emptyName";
	const char* fileName = "";
	FILE* pFile = 0;
	int savedErrno = 0;
	
	// Create new.
	errno = 0;
	pFile = fopen( fileName, "r" );
	savedErrno = errno;
	
	if( (pFile == 0) && (savedErrno == 2) ){
		printPass( testName );
	}else{
		printFail( testName, "Should not have been able to call open() with an empty string." );
	}	
}
