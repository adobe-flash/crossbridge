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

void runFcloseTests(){
	fclose_basic();
	fclose_ebadf();
}

/**
 * Close a file.
 **/
void fclose_basic(){
	const char* testName = "fclose_basic";
	char ret = '\0';
	FILE* pFile = 0;
	
	// Open a file, close it, then try to write to it.
	pFile = fopen( testName, "w+" );
	fclose( pFile );
	errno = 0;
	ret = fputc( 'L', pFile );

	if( (ret == EOF) && (errno == EBADF) ){
		printPass( testName );
	}else{
		printFail( testName, "Any attempt to use the FILE* should fail" );
	}
}

/**
 * Be sure close sets errno if it encounters a closed file.
 **/
void fclose_ebadf(){
	const char* testName = "fclose_ebadf";
	FILE* pFile = 0;
	
	// Open a file, close it, then try to write to it.
	pFile = fopen( testName, "w+" );
	fclose( pFile );	
	errno = 0;
	int ret = fclose( pFile );

	if( (ret == EOF) && (errno == EBADF) ){
		printPass( testName );
	}else{
		printFail( testName, "Any attempt to use the FILE* should fail" );
	}
}

