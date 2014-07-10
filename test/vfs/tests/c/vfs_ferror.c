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
 * NOTE: Test lightly.
 **/

void runFerrorTests(){
	ferror_error();
	ferror_noError();
}

void ferror_error(){
	const char* testName = "ferror_error";
	FILE* pFile = fopen( testName, "r" );
	int ret = 0;
	fputc( 'x', pFile );
	ret = ferror( pFile );
	fclose( pFile );

	if( ret == 1 ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ferror to provide an error code" );
	}
	
}

void ferror_noError(){
	const char* testName = "ferror_noError";
	FILE* pFile = fopen( testName, "w+" );
	int ret = 0;
	fputc( 'x', pFile );
	ret = ferror( pFile );
	fclose( pFile );
	
	if( ret == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ferror to provide no error code" );
	}
	
}
