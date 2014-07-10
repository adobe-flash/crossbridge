#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

using namespace std;

/**
 * Test lseek();
 **/
void runLseekTests(){

	lseek_ebadf();
	lseek_einvalWhence();
	lseek_doesNotExpand();

	lseek_set_setNeg();
	lseek_set_setZero();
	lseek_set_setOne();	
	lseek_set_setLast();
	lseek_set_setBeyondEnd();
	
	lseek_end_setNeg();
	lseek_end_setZero();
	lseek_end_setOne();
	lseek_end_setLast();
	lseek_end_setBeyondEnd();

	lseek_cur_startMinusOne();
	lseek_cur_start0();
	lseek_cur_startPlusOne();
	lseek_cur_endMinusOne();
	lseek_cur_end0();
	lseek_cur_endPlusOne();
}

/**
 * Test EBADF.
 **/
void lseek_ebadf(){
	const char* testName = "lseek_ebadf";
	errno = 0;
	int ret = lseek( -1, 0, SEEK_SET );
	
	if( (errno == EBADF) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF and -1" );
	}
}

/**
 * Should get EINVAL if whence param. is wrong.
 * SEEK_SET=1
 * SEEK_CUR=2
 * SEEK_END=3
 **/
void lseek_einvalWhence(){
	const char* testName = "lseek_einvalWhence";
	
	// Create a file for testing.
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );

	errno = 0;
	int ret = lseek( theFile, 0, SEEK_END + 1 );

	if( (errno == EINVAL) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EINVAL and -1" );
	}
	
	close( theFile );	
}

/**
 * lseek() should not expand a file, even if you seek
 * beyond the end of it.  (You'd need to write to expand.)
 **/
void lseek_doesNotExpand(){
	const char* testName = "lseek_doesNotExpand";
	struct stat statbuf1;
	struct stat statbuf2;
	int origSize = 0;
	
	// Create it.
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Get the size and close.
	theFile = open( testName, O_RDONLY );
	close( theFile );
	stat( testName, &statbuf1 );
	
	// Seek and close.
	theFile = open( testName, O_RDWR );
	lseek( theFile, 10, SEEK_END );
	close( theFile );
	
	// It should not have grown.
	stat( testName, &statbuf2 );
	
	if( statbuf1.st_size == statbuf2.st_size ){
		printPass( testName );
	}else{
		printFail( testName, "Expected size before and after to be the same" );
	}
}

/**
 * SEEK_SET, offset results in neg
 **/
void lseek_set_setNeg(){
	const char* testName = "lseek_set_setNeg";
	int ret = -1;

	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	errno = 0;
	ret = lseek( theFile, -1, SEEK_SET );
	
	if( (errno == EINVAL) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EINVAL and -1" );
	}
	
	close( theFile );	
}

/**
 * SEEK_SET, offset results in 0
 **/
void lseek_set_setZero(){
	const char* testName = "lseek_set_setZero";
	const char* bytesExpected = "0bcde";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek somewhere, then to 0.
	lseek( theFile, 3, SEEK_SET );
	errno = 0;
	ret = lseek( theFile, 0, SEEK_SET );
	
	if( (ret == 0) && (errno == 0) ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 0" );
	}
	
	close( theFile );	
}


/**
 * SEEK_SET, offset results in 1.
 **/
void lseek_set_setOne(){
	const char* testName = "lseek_set_setOne";
	const char* bytesExpected = "a0cde";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	errno = 0;
	
	// Seek
	ret = lseek( theFile, 1, SEEK_SET );
	
	if( (ret == 1) && (errno == 0) ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 1" );
	}
	
	close( theFile );
}

/**
 * SEEK_SET, offset is the last character.
 **/
void lseek_set_setLast(){
	const char* testName = "lseek_set_setLast";
	const char* bytesExpected = "abcd0";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	errno = 0;
	
	// Seek
	ret = lseek( theFile, strlen( gData1 ) - 1, SEEK_SET );
	
	if( (ret == 4) && (errno == 0) ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 4" );
	}
	
	close( theFile );
}

/**
 * SEEK_SET, go beyond the end (expand the file).
 **/
void lseek_set_setBeyondEnd(){
	const char* testName = "lseek_set_setBeyondEnd";
	const char* bytesExpected = "abcde0";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	ret = lseek( theFile, strlen( gData1 ), SEEK_SET );
	
	if( ret == 5 ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) + 1 );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 5" );
	}
	
	close( theFile );
}

/**
 * SEEK_END, offset results in neg.
 **/
void lseek_end_setNeg(){
	const char* testName = "lseek_end_setNeg";
	int ret = -1;
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Now seek to the end minus length + 1.
	theFile = open( testName, O_RDWR );
	errno = 0;
	ret = lseek( theFile, 0 - strlen( gData1 ) - 1, SEEK_END );
	
	if( (errno == EINVAL) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EINVAL and -1" );
	}
	
	close( theFile );
}

/**
 * SEEK_END, offset results in zero
 **/
void lseek_end_setZero(){
	const char* testName = "lseek_end_setZero";
	const char* bytesExpected = "0bcde";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek somewhere, then to 0.
	lseek( theFile, 3, SEEK_SET );
	errno = 0;
	ret = lseek( theFile, 0 - strlen( gData1 ), SEEK_END );
	
	if( (errno == 0) && (ret == 0) ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 0" );
	}
	
	close( theFile );	
}

/**
 * SEEK_END, offset results in 1.
 **/
void lseek_end_setOne(){
	const char* testName = "lseek_end_setOne";
	const char* bytesExpected = "a0cde";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	ret = lseek( theFile, 0 - strlen( gData1 ) + 1, SEEK_END );
	
	if( ret == 1 ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			//printf( "bytesRead: %s, bytesExpected: %s", bytesRead, bytesExpected );
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 1" );
	}
	
	close( theFile );
}

/**
 * SEEK_END, seek to the last character.
 **/
void lseek_end_setLast(){
	const char* testName = "lseek_end_setLast";
	const char* bytesExpected = "abcd0";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	errno = 0;
	
	// Seek
	ret = lseek( theFile, -1, SEEK_END );
	
	if( (ret == 4) && (errno == 0) ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 4" );
	}
	
	close( theFile );
}

/**
 * SEEK_END, append to the file.
 **/
void lseek_end_setBeyondEnd(){
	const char* testName = "lseek_end_setBeyondEnd";
	const char* bytesExpected = "abcde0";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	ret = lseek( theFile, 0, SEEK_END );

	if( ret == 5 ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) + 1 );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 5" );
	}
	
	close( theFile );
}

/**
 * SEEK_CUR, start at the beginning and go back.
 **/
void lseek_cur_startMinusOne(){
	const char* testName = "lseek_cur_startMinusOne";
	int ret = -1;
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	lseek( theFile, 0, SEEK_CUR );
	errno = 0;
	ret = lseek( theFile, -1, SEEK_CUR );
	
	if( (ret == -1) && (errno == EINVAL) ){
			printPass( testName );
	}else{
		printFail( testName, "Expected -1 and EINVAL" );
	}

	close( theFile );
}

/**
 * SEEK_CUR, start at the beginning and stay.
 **/
void lseek_cur_start0(){
	const char* testName = "lseek_cur_start0";
	const char* bytesExpected = "0bcde";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	ret = lseek( theFile, 0, SEEK_CUR );
	
	if( ret == 0 ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) + 1 );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 0" );
	}
	
	close( theFile );
}

/**
 * SEEK_CUR, start at the beginning and go forward.
 **/
void lseek_cur_startPlusOne(){
	const char* testName = "lseek_cur_startPlusOne";
	const char* bytesExpected = "a0cde";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	ret = lseek( theFile, 1, SEEK_CUR );
	
	if( ret == 1 ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) + 1 );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 1" );
	}
	
	close( theFile );
}

/**
 * SEEK_CUR, start at the end and go back to 2nd to last.
 **/
void lseek_cur_endMinusOne(){
	const char* testName = "lseek_cur_endMinusOne";
	const char* bytesExpected = "abc0e";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	ret = lseek( theFile, strlen( gData1 ) - 2, SEEK_CUR );
	
	if( ret == 3 ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) + 1 );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 3" );
	}
	
	close( theFile );
}

/**
 * SEEK_CUR, start at the end and stay.
 **/
void lseek_cur_end0(){
	const char* testName = "lseek_cur_end0";
	const char* bytesExpected = "abcd0";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	ret = lseek( theFile, strlen( gData1 ) - 1, SEEK_CUR );
	
	if( ret == 4 ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) + 1 );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 4" );
	}
	
	close( theFile );
}

/**
 * SEEK_CUR, start at the end and go forward (append).
 **/
void lseek_cur_endPlusOne(){
	const char* testName = "lseek_cur_endPlusOne";
	const char* bytesExpected = "abcde0";
	char bytesRead[ gBufSmall ];
	int ret = -1;
	
	memset( bytesRead, 0, gBufSmall );
	
	int theFile = open( testName, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	theFile = open( testName, O_RDWR );
	
	// Seek
	ret = lseek( theFile, strlen( gData1 ), SEEK_CUR );
	
	if( ret == 5 ){
		// Write a character and verify.
		write( theFile, gData2, 1 );
		close( theFile );
		theFile = open( testName, O_RDONLY );
		read( theFile, &bytesRead, strlen( gData1 ) + 1 );
		
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data did not match" );
		}
		
	}else{
		printFail( testName, "Expected 5" );
	}
	
	close( theFile );
}
