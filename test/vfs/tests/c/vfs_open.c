#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 This file tests open().
 
 Permission flags: 
	O_RDONLY = 0
	O_WRONLY = 1
	O_RDWR = 2

	O_APPEND = 8
	O_CREAT = 512
	O_TRUNC = 1024
	O_EXCL = 2048
**/

/**
 * Runs all of the tests in this file.
 **/
void runOpenTests(){
	open_rdwr_noFile();
	open_rdwr_open();
	open_rdwr_dir();
	open_rdwr_openWriteAndRead();
	open_rdwrAppend();
	open_rdwrCreat();
	open_rdwrCreatExcl();
	open_rdwrTrunc();
	open_rdonly_noFile();
	open_rdonly_read();
	open_rdonly_write();
	open_rdonlyAppend();
	open_rdonlyCreat();
	open_rdonlyCreatExcl();
	open_wronly_read();
	open_wronly_write();
	open_wronly_dir();
	open_wronlyAppend();
	open_wronlyCreat();
	open_wronlyCreatInNonexistantDir();
	open_wronlyCreat_read();
	open_wronlyCreatExcl();
	open_wronlyTrunc();
	open_255chars();
	open_hugeName();
	open_emptyName();
	open_notDir();
	open_rdonlyfs_rdonly();
	open_rdonlyfs_rdwr();
	open_rdonlyfs_creat();
}

/**
 * RDWR alone, file does not exist.
 **/
void open_rdwr_noFile(){
	const char* testName = "open_rdwr_NoFile";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;

	// Open when there is no file.
	flags = O_RDWR;
	errno = 0;
	theFile = open( testName, flags );
	savedErrno = errno;
	close( theFile );

	if( savedErrno == ENOENT ){
		if( theFile == -1 ){
			printPass( testName );
		}else{
			printFail( testName, "Expected invalid file descriptor" );
		}
	}else{
		printFail( testName, "Expected ENOENT" );
	}
}

/**
 * RDWR alone, just open.
 **/
void open_rdwr_open(){
	const char* testName = "open_rdwrOpen";
	int theFile = -1;
	int flags = -1;
	
	// Create it.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	close( theFile );

	// Open it.
	flags = O_RDWR;
	theFile = open( testName, flags );
	close( theFile );

	if( theFile > -1 ){
		printPass( testName );
	}else{
		printFail( testName, "Expected valid file descriptor" );
	}
}

/**
 * RDWR
 * Open a directory as RDWR; this should cause errno=EISDIR.
 **/
void open_rdwr_dir(){
	const char* testName = "open_rdwr_dir";
	const char* dirName = "/open_rdwr_dir";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	
	// Create a directory.
	if( mkdir( dirName, S_IRWXU | S_IRWXG | S_IRWXO ) == 0 ){
		flags = O_RDWR;
		errno = 0;
		theFile = open( dirName, flags );
		savedErrno = errno;
		close( theFile );
		if( (theFile == -1) && (savedErrno == EISDIR) ){
			printPass( testName );
		}else{
			printFail( testName, "Should not have been able to open a directory RDWR; fails due to ALC-357. (Fixed)" );
		}
	}else{
		printFail( testName, "Unable to create directory" );
	}
}

/**
 * RDWR alone, write and read.
 **/
void open_rdwr_openWriteAndRead(){
	const char* testName = "open_rdwr_openWriteAndRead";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	memset( bytesActual, 0, gBufLarge );
	
	// Create it.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	close( theFile );
	
	// Write.
	flags = O_RDWR;
	theFile = open( testName, flags );
	size_t numBytesWritten = write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Read.
	flags = O_RDWR;
	theFile = open( testName, flags );
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	close( theFile );

	if( (strcmp( bytesActual, gData1 ) == 0) && (numBytesRead == strlen( gData1 )) && (numBytesWritten == strlen( gData1 )) ){
		printPass( testName );
	}else{
		printFail( testName, "Read/write was not as expected" );
	}
}

/**
 * Write and read using RDWR | APPEND.
 **/
void open_rdwrAppend(){
	const char* testName = "open_rdwrAppend";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	char bytesExpected[ gBufLarge ];
	
	memset( bytesActual, 0, gBufLarge );
	memset( bytesExpected, 0, gBufLarge );
	strcpy( bytesExpected, gData1 );
	strcat( bytesExpected, gData2 );
	
	// Create a file to use (tested above).
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Open, append, close.
	flags = O_RDWR | O_APPEND;
	theFile = open( testName, flags );
	write( theFile, gData2, strlen( gData2 ) );
	close( theFile );
	
	// Open, read, close.
	flags = O_RDWR | O_APPEND;
	theFile = open( testName, flags );	
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	close( theFile );
	
	if( (strcmp( bytesActual, bytesExpected ) == 0) && (numBytesRead == strlen( bytesExpected )) ){
		printPass( testName );
	}else{
		printFail( testName, "Read/write was not as expected" );
	}
}

/**
 * Use CREAT to create a file.
 * Write and read using RDWR | CREAT.
 **/
void open_rdwrCreat(){
	const char* testName = "open_rdwrCreat";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	memset( bytesActual, 0, gBufLarge );
	
	// Create, write, close.
	flags = O_RDWR | O_CREAT; 
	theFile = open( testName, flags );
	size_t numBytesWritten = write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Open, read, close.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	close( theFile );
	
	if( (strcmp( bytesActual, gData1 ) == 0) && (numBytesRead == strlen( gData1 )) && (numBytesWritten == strlen( gData1 )) ){
		printPass( testName );
	}else{
		printFail( testName, "Read/write was not as expected" );
	}
}

/**
 * Test RDWR | CREAT | EXCL.
 * If O_CREAT and O_EXCL are set, open() shall fail if the file exists.
 * Do this when the file does not exist, then when it exists.
 **/
void open_rdwrCreatExcl(){
	const char* testName = "open_rdwrCreatExcl";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;

	// The file does not exist yet.
	flags = O_RDWR | O_CREAT | O_EXCL;
	theFile = open( testName, flags );
	size_t numBytesWritten = write( theFile, gData1, strlen( gData1 ) );
	close( theFile );	

	if( (theFile != -1) && (numBytesWritten == strlen( gData1 )) ){
		// Now that the file exists, try to open it again.
		errno = 0;
		theFile = open( testName, flags );
		savedErrno = errno;
		close( theFile );
		if( (theFile == -1) && (savedErrno == EEXIST) ){
			printPass( testName );
		}else{
			printFail( testName, "Expected an invalid file descriptor and errno to be EEXIST" );
		}
	}else{
		printFail( testName, "Was not able to create and/or write to the file" );
	}
}

/**
 * Test RDWR | TRUNC
 * If the file exists and is a regular file, and the file is successfully opened
 * O_RDWR or O_WRONLY, its length shall be truncated to 0, and the mode and owner
 * shall be unchanged. (Using with O_RDONLY is undefined.)
 **/
void open_rdwrTrunc(){
	const char* testName = "open_rdwrTrunc";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	memset( bytesActual, 0, gBufLarge );
	
	// Create a file to test with.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );	
	
	// Open with the truncation flag and read.  It should be empty.
	flags = O_RDWR | O_TRUNC;
	theFile = open( testName, flags );
	close( theFile );
	
	flags = O_RDWR;
	theFile = open( testName, flags );
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	close( theFile );	
	
	if( numBytesRead == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Should not have found any data" );
	}
}

/**
 * RDONLY alone, file does not exist.
 **/
void open_rdonly_noFile(){
	const char* testName = "open_rdonlyNoFile";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	
	// Open when there is no file.
	flags = O_RDONLY;
	errno = 0;
	theFile = open( testName, flags );
	savedErrno = errno;
	close( theFile );
	
	if( savedErrno == ENOENT ){
		if( theFile == -1 ){
			printPass( testName );
		}else{
			printFail( testName, "Expected invalid file descriptor" );
		}
	}else{
		printFail( testName, "Expected ENOENT" );
	}
}

/**
 * RDONLY alone, read.
 **/
void open_rdonly_read(){
	const char* testName = "open_rdonly_read";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	memset( bytesActual, 0, gBufLarge );
	
	// Create it.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Read.
	flags = O_RDONLY;
	theFile = open( testName, flags );
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	close( theFile );
	
	if( (strcmp( bytesActual, gData1 ) == 0) && (numBytesRead == strlen( gData1 )) ){
		printPass( testName );
	}else{
		printFail( testName, "Read was not as expected" );
	}
}

/**
 * RDONLY alone, write
 **/
void open_rdonly_write(){
	const char* testName = "open_rdonly_write";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	char bytesActual[ gBufLarge ];
	memset( bytesActual, 0, gBufLarge );
	
	// Create it.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Write.
	flags = O_RDONLY;
	theFile = open( testName, flags );
	errno = 0;
	write( theFile, gData1, strlen( gData1 ) );
	savedErrno = errno;
	close( theFile );
	
	if( savedErrno == EBADF ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF" );
	}
}
	
/**
 * Test RDONLY | APPEND
 * It seems this would be undefined in the standard
 * but hey, RDONLY seems to trump APPEND, which sounds good to me, so let's
 * keep it that way.
 **/
void open_rdonlyAppend(){
	const char* testName = "open_rdonlyAppend";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	char bytesActual[ gBufLarge ];
	memset( bytesActual, 0, gBufLarge );
	
	// Create a file to test with.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );	
	
	// Open with the read only flag and append.  Writing should fail.
	flags = O_RDONLY | O_APPEND;
	theFile = open( testName, flags );
	errno = 0;
	size_t numBytesWritten = write( theFile, gData2, strlen( gData2 ) );
	savedErrno = errno;
	close( theFile );	
	
	if( numBytesWritten == -1 ){
		if( savedErrno == EBADF ){ 		
			// Make sure the contents did not change.
			flags = O_RDONLY;
			theFile = open( testName, flags );
			read( theFile, bytesActual, gBufLarge );
			if( strcmp( bytesActual, gData1 ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "Data did not match" );
			}
		}else{
			printFail( testName, "Expected EBADF" );
		}
	}else{
		printFail( testName, "Should not have been able to write any data" );
	}
}

/**
 * RDONLY | CREAT
 **/
void open_rdonlyCreat( void ){
	const char* testName = "open_rdonlyCreat";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	memset( bytesActual, 0, gBufLarge );
	
	// Create it and attempt to write.
	flags = O_RDONLY | O_CREAT;
	theFile = open( testName, flags );
	size_t numBytesWritten = write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	if( numBytesWritten == -1 ){
		// Make sure the file was created and is empty.
		flags = O_RDONLY;
		theFile = open( testName, flags );
		size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
		if( numBytesRead == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Found unexpected data" );
		}
	}else{
		printFail( testName, "Should not have been able to write any data" );
	}
	
}

/**
 * RDONLY | CREAT | EXCL
 * If O_CREAT and O_EXCL are set, open() shall fail if the file exists.
 * Do this when the file does not exist, then when it exists.
 **/
void open_rdonlyCreatExcl(){
	const char* testName = "open_rdonlyCreatExcl";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	
	// The file does not exist yet.
	flags = O_RDWR | O_CREAT | O_EXCL;
	theFile = open( testName, flags );
	size_t numBytesWritten = write( theFile, gData1, strlen( gData1 ) );
	close( theFile );	
	
	if( (theFile != -1) && (numBytesWritten == strlen( gData1 )) ){
		// Now that the file exists, try to open it again.
		errno = 0;
		theFile = open( testName, flags );
		savedErrno = errno;
		close( theFile );
		if( (theFile == -1) && (savedErrno == EEXIST) ){
			printPass( testName );
		}else{
			printFail( testName, "Expected an invalid file descriptor and errno to be EEXIST" );
		}
	}else{
		printFail( testName, "Was not able to create and/or write to the file" );
	}
}

/**
 * RDONLY | TRUNC is undefined.
 **/

/**
 * WRONLY
 * Try to read a file.
 **/
void open_wronly_read(){
	const char* testName = "open_wronly_read";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	char bytesActual[ gBufLarge ];
	
	// Create a file.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Read it.
	flags = O_WRONLY;
	theFile = open( testName, flags );
	errno = 0;
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	savedErrno = errno;
	close( theFile );
	if( (savedErrno == EBADF) && (numBytesRead == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Should not have been able to read" );
	}	
}

/**
 * WRONLY
 * Write to a file.
 **/
void open_wronly_write(){
	const char* testName = "open_wronly_write";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	
	// Create a file.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	close( theFile );
	
	// Write to it.
	flags = O_WRONLY;
	theFile = open( testName, flags );
	errno = 0;
	size_t bytesWritten = write( theFile, gData1, strlen( gData1 ) );
	savedErrno = errno;
	close( theFile );

	if( bytesWritten == strlen( gData1 ) ){
		printPass( testName );
	}else{
		printFail( testName, "Was not able to write" );
	}	
}

/**
 * WRONLY
 * Open a directory as WRONLY; this should cause errno=EISDIR.
 **/
void open_wronly_dir(){
	const char* testName = "open_wronly_dir";
	const char* dirName = "/open_wronly_dir";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	
	// Create a directory.
	if( mkdir( dirName, S_IRWXU | S_IRWXG | S_IRWXO ) == 0 ){
		flags = O_WRONLY;
		errno = 0;
		theFile = open( dirName, flags );
		savedErrno = errno;
		close( theFile );
		if( (theFile == -1) && (savedErrno == EISDIR) ){
			printPass( testName );
		}else{
			printFail( testName, "Should not have been able to open a directory WRONLY; fails due to ALC-357. (Fixed)" );
		}
	}else{
		printFail( testName, "Unable to create directory" );
	}
}

/**
 * WRONLY | APPEND
 **/
void open_wronlyAppend(){
	const char* testName = "open_wronlyAppend";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	char bytesExpected[ gBufLarge ];
	
	memset( bytesActual, 0, gBufLarge );
	memset( bytesExpected, 0, gBufLarge );
	strcpy( bytesExpected, gData1 );
	strcat( bytesExpected, gData2 );
	
	// Create a file to append to.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Open, append, close.
	flags = O_WRONLY | O_APPEND;
	theFile = open( testName, flags );
	write( theFile, gData2, strlen( gData2 ) );
	close( theFile );
	
	// Open, read, close.
	flags = O_RDONLY;
	theFile = open( testName, flags );	
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	close( theFile );
	
	if( (strcmp( bytesActual, bytesExpected ) == 0) && (numBytesRead == strlen( bytesExpected )) ){
		printPass( testName );
	}else{
		printFail( testName, "Read was not as expected" );
	}
}

/**
 * WRONLY | CREAT
 **/
void open_wronlyCreat(){
	const char* testName = "open_wronlyCreat";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	
	// Create a file.
	flags = O_WRONLY | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// See if it got created.
	flags = O_RDONLY;
	theFile = open( testName, flags );
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	close( theFile );
	
	if( numBytesRead == strlen( gData1 ) ){
		printPass( testName );
	}else{
		printFail( testName, "numBytesRead was not correct" );
	}
}

/**
 * WRONLY | CREAT
 **/
void open_wronlyCreatInNonexistantDir(){
	const char* testName = "open_wronlyCreatInNonexistantDir";
	char bytesActual[ gBufLarge ];
	
	int flags = O_WRONLY | O_CREAT;
	int theFile = open( "/open_wronlyCreatInNonexistantDir/foo.txt", flags );
	
	if( theFile == -1 ){
		printPass( testName );
	}else{
		printFail( testName, "The file should not have been created. Fails due to ALC-306. (Fixed)" );
	} 	
}

/**
 * WRONLY | CREAT
 * Neg. test: Try to read the file when combined with CREAT.  
 * Maybe CREAT causes something inappropriate.
 **/
void open_wronlyCreat_read(){
	const char* testName = "open_wronlyCreat_read";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	char bytesActual[ gBufLarge ];
	
	// Create a file.
	flags = O_WRONLY | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	errno = 0;
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	savedErrno = errno;
	close( theFile );
	
	if( (savedErrno == EBADF) && (numBytesRead == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Should not have been able to read" );
	}
}

/**
 * Test WRONLY | CREAT | EXCL.
 * If O_CREAT and O_EXCL are set, open() shall fail if the file exists.
 * Do this when the file does not exist, then when it exists.
 **/
void open_wronlyCreatExcl(){
	const char* testName = "open_wronlyCreatExcl";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	
	// The file does not exist yet.
	flags = O_WRONLY | O_CREAT | O_EXCL;
	theFile = open( testName, flags );
	size_t numBytesWritten = write( theFile, gData1, strlen( gData1 ) );
	close( theFile );	
	
	if( (theFile != -1) && (numBytesWritten == strlen( gData1 )) ){
		// Now that the file exists, try to open it again.
		errno = 0;
		theFile = open( testName, flags );
		savedErrno = errno;
		close( theFile );
		if( (theFile == -1) && (savedErrno == EEXIST) ){
			printPass( testName );
		}else{
			printFail( testName, "Expected an invalid file descriptor and errno to be EEXIST" );
		}
	}else{
		printFail( testName, "Was not able to create and/or write to the file" );
	}
}

/**
 * WRONLY | TRUNC
 **/
void open_wronlyTrunc(){
	const char* testName = "open_wronlyTrunc";
	int theFile = -1;
	int flags = -1;
	char bytesActual[ gBufLarge ];
	
	// Create a file.
	flags = O_WRONLY | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );

	// Now open as WRONLY | TRUNC to truncate it.
	flags = O_WRONLY | O_TRUNC;
	theFile = open( testName, flags );
	close( theFile );	

	// Verify that the file is now empty.
	flags = O_RDWR;
	theFile = open( testName, flags );
	size_t numBytesRead = read( theFile, bytesActual, gBufLarge );
	close( theFile );	
	
	if( numBytesRead == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Should not have found any data" );
	}
}

/*******************************************************************************
********************************************************************************
* Various other open tests not really dependent on flags.
********************************************************************************
********************************************************************************/

/**
 * 255 character file name (standard on Windows)
 **/
void open_255chars(){
	const char* testName = "open_255chars";
	int nameSize = 256;
	int theFile = -1;
	int flags = -1;
	int i = 0;
	int savedErrno = 0;
	char fileName[ nameSize ];
	memset( fileName, 0, nameSize );

	// Let the last entry be 0.
	for( i = 0; i < nameSize - 1; ++i ){
		fileName[ i ] = 'a';
	}	
	
	// Create new.
	flags = O_RDWR | O_CREAT;
	errno = 0;
	theFile = open( fileName, flags );
	savedErrno = errno;
	close( theFile );
	
	if( (theFile > -1) && (savedErrno == 0) ){
		printPass( testName );
	}else{
		printFail( testName, "Did not create the file" );
	}	
}

/**
 * Big file name
 **/
void open_hugeName(){
	const char* testName = "open_hugeName";
	int nameSize = 1000001;
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	int i = 0;
	char bigName[ nameSize ];
	memset( bigName, 0, nameSize );

	// Let the last entry be 0.
	for( i = 0; i < nameSize - 1; ++i ){
		bigName[ i ] = 'a';
	}
	
	// Create new.
	flags = O_RDWR | O_CREAT;
	errno = 0;
	theFile = open( bigName, flags );
	savedErrno = errno;
	close( theFile );
		
	if( (theFile > -1) && (savedErrno == 0) ){
		printPass( testName );
	}else{
		printFail( testName, "Did not create the file" );
	}	
}

/**
 * Empty file name
 **/
void open_emptyName(){
	const char* testName = "open_emptyName";
	const char* fileName = "";
	int theFile = -1;
	int savedErrno = 0;

	// Create new.
	errno = 0;
	theFile = open( fileName, O_RDWR | O_CREAT );
	savedErrno = errno;
	close( theFile );
	
	if( (theFile == -1) && (savedErrno == 2) ){
		printPass( testName );
	}else{
		printFail( testName, "Should not have been able to call open() with an empty string." );
	}	
}

/**
 * Element of the path is a file instead of a directory.
 * (This triggers ENOTDIR.)
 **/
void open_notDir(){
	const char* testName = "open_notDir";
	const char* invalidPath = "open_notDir/foo.txt";
	int theFile = -1;
	int flags = -1;
	int savedErrno = 0;
	
	// Create a file.
	flags = O_RDWR | O_CREAT;
	theFile = open( testName, flags );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );

	// Now treat the file like a directory.
	flags = O_RDWR | O_CREAT;
	errno = 0;
	theFile = open( invalidPath, flags );
	savedErrno = errno;
	close( theFile );

	if( (theFile == -1) && (savedErrno == ENOTDIR) ){
		printPass( testName );
	}else{
		printFail( testName, "Should not have been able to create the file.  Fails due to ALC-356. (Fixed)" );
	}	
}

/**
 * Open a file in the read only file system as read only.
 **/
void open_rdonlyfs_rdonly(){
	const char* testName = "open_rdonlyfs_rdonly";
	char* bytesRead = ( char* )calloc( gBufSmall, sizeof( char ) );
	int numBytesRead = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "compressed_readonly/basic", O_RDONLY );
	if( errno != 0 || theFile == -1 ){
		printFail( testName, "Unable to open readonly file as O_RDONLY." );
		return;
	}
	
	errno = 0;
	numBytesRead = read( theFile, bytesRead, gBufSmall );
	if( errno != 0 || numBytesRead == -1 ){
		printFail( testName, "Unable to read readonly file as O_RDONLY." );
		close( theFile );
		return;		
	}
	
	if( strcmp( bytesRead, gData1 ) != 0 ){
		printFail( testName, "Data was not correct." );
		close( theFile );
		return;		
	}
	
	close( theFile );
	
	printPass( testName );
	return;
}

/**
 * Try to open a file in the read only file system as read/write.
 **/
void open_rdonlyfs_rdwr(){
	const char* testName = "open_rdonlyfs_rdwr";
	const char* expectedError = "Read-only file system";
	char* error = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "compressed_readonly/basic", O_RDWR );
	int savedErrno = errno;
	
	if( theFile != -1 ){
		printFail( testName, "Should not have been able to open a file as O_RDWR on the read only file system." );
		close( theFile );
		return;
	}
	
	if( savedErrno != 30 || strcmp( error = strerror( savedErrno ), expectedError ) != 0 ){
		printFail( testName, "The error was not correct." );
		return;
	}
	
	printPass( testName );
	return;
}

/**
 * Try to create a file in a read only file system.
 **/
void open_rdonlyfs_creat(){
	const char* testName = "open_rdonlyfs_creat";
	const char* expectedError = "Read-only file system";
	char* error = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "compressed_readonly/open_rdonlyfs_creat", O_CREAT );
	int savedErrno = errno;
	
	if( theFile != -1 ){
	 	printFail( testName, "Should not have been able to create a file on the read only file system." );
		close( theFile );
		return;
	}
	
	if( savedErrno != 30 || strcmp( error = strerror( savedErrno ), expectedError ) != 0 ){
		printFail( testName, "The error was not correct." );
		return;
	}
	
	printPass( testName );
	return;
}
