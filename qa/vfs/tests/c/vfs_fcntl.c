#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * fcntl does a lot, but we are only supporting:
 *	F_DUPFD
 *	F_GETFL
 *	F_SETFL
 * for *FL, we only support O_APPEND.
 **/
void runFcntlTests(){
	fcntl_dupfd();
	fcntl_fgetfl();
	fcntl_fsetfl();
}

/**
 * Basic use of fcntl to get a lower file descriptor number.
 **/
void fcntl_dupfd(){

	const char* testName = "fcntl_dupfd";
	const char* file1 = "fcntl/fcntl_dupfd1.txt";
	const char* file2 = "fcntl/fcntl_dupfd2.txt";
	const char* file3 = "fcntl/fcntl_dupfd3.txt";
	const char* file4 = "fcntl/fcntl_dupfd4.txt";
	
	mkdir( "fcntl", 0777 );
	
	int ret = 0;
	char bytesRead[ gBufSmall ];
	char* bytesReadPtr = bytesRead;
	int desc1 = open( file1, O_RDWR | O_CREAT, S_IRWXU );
	int desc2 = open( file2, O_RDONLY | O_CREAT, S_IRWXU );
	int desc3 = open( file3, O_WRONLY | O_CREAT, S_IRWXU );
	int desc4 = open( file4, O_RDWR | O_CREAT, S_IRWXU );
	struct stat statbuf;
	
	memset( bytesReadPtr, 0, gBufSmall );
	ret = write( desc4, gData1, 5 );
	lseek( desc4, 0, SEEK_SET );

	close( desc1 );
	close( desc2 );
	close( desc3 );
	
	// At this point, calling fcntl with F_DUPFD should return the lowest
	// available file descriptor number, not lower than the 
	// 3rd parameter.  desc1's should not be used because it is too low.
	// desc3's is free, but too high because desc2's is available.  We should end up with
	// desc5 equal to the value of desc2, describing the same file that
	// desc4 is.

	int desc5 = fcntl( desc4, F_DUPFD, desc2 );

	if( desc5 == desc2 ){
		if( fstat( desc5, &statbuf ) == 0 ){
			// Read the file; we should find what we wrote to file4.
			ret = read( desc5, bytesReadPtr, strlen( gData1 ) );
			
			if( strcmp( bytesReadPtr, gData1 ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "Data was not correct" );
			}
	   }else{
		   printFail( testName, "stat() on desc5 failed" );
	   }

	}else{
		printFail( testName, "Descriptor was not correct" );
	}
	
	if( desc4 > 0 ){
		close( desc4 );
	}
	
	if( desc5 > 0 ){
		close( desc5 );
	}
	
}

/**
 * Look for O_APPEND
 **/
void fcntl_fgetfl(){
	const char* testName = "fcntl_fgetfl";
	int stat = 0;
	int desc = open( testName, O_RDWR | O_CREAT, S_IRWXU );

	close( desc );
	desc = open( testName, O_APPEND );
	stat = fcntl( desc, F_GETFL );
	
	if( stat == O_APPEND ){
		printPass( testName );
	}else{
		printFail( testName, "Expected O_APPEND.  Fails due to ALC-401." );
	}
}

/**
 * Set O_APPEND.
 **/
void fcntl_fsetfl(){
	const char* testName = "fcntl_fsetfl";
	int stat = 0;
	int desc = open( testName, O_RDWR | O_CREAT, S_IRWXU );

	close( desc );
	desc = open( testName, O_RDWR );
	fcntl( desc, F_SETFL, O_APPEND );
	stat = fcntl( desc, F_GETFL );
	
	if( stat == O_APPEND ){
		printPass( testName );
	}else{
		printFail( testName, "Expected O_APPEND.  Fails due to ALC-401." );
	}
}