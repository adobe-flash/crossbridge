#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * This file tests read(). 
**/

/**
 * Run the tests in this file.
 **/
void runReadTests(){
	
	read_updateAccessTime();
	read_emptyFile();
	read_noBytes();
	read_oneByte();	
	read_subsequentReads();
	read_beyondEOF();
	read_ebadf();	
	read_lseekSet();
	read_lseekCur();
	read_lseekEnd();
	read_lseekBeyondEnd1();
	read_lseekBeyondEnd2();
	read_lseekBeyondEnd3();
}

/**
 * The file's access time should be updated.
 **/
void read_updateAccessTime(){
	const char* testName = "read_updateAccessTime";
	int theFile = -1;
	time_t time1 = -1; // time_t is an int32.
	time_t time2 = -1;
	struct stat file_stats;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );

	// Open, read, get the access time, wait.  Then read again and compare access times.
	theFile = open( testName, O_RDWR );
	read( theFile, bytesRead, 1 );
	stat( testName, &file_stats );
	time1 = file_stats.st_atime;
	sleep( 1 );

	read( theFile, bytesRead, 1 );
	stat( testName, &file_stats );
	time2 = file_stats.st_atime;
	close( theFile );	
	
	if( time2 >= (time1 + 1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected new access time to be > 1 second after old access time.  Fails due to ALC-358." );
	}
}

/**
 * Read an empty file.
 **/
void read_emptyFile(){
	const char* testName = "read_emptyFile";
	int theFile = -1;
	int i = 0;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file containing no data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	close( theFile );

	// Read.
	theFile = open( testName, O_RDWR );
	size_t numBytesRead = read( theFile, bytesRead, gBufSmall );
	close( theFile );
	
	if( numBytesRead == 0 ){
		for( i = 0; i < gBufSmall; ++i ){
			if( bytesRead[ i ] != 0 ){
				printFail( testName, "Did not expect the buffer to be modified." );
				return;
			}
		}
		printPass( testName );
	}else{
		printFail( testName, "Expected to read 0 bytes." );
	}
}

/**
 * Read zero bytes.
 **/
void read_noBytes(){
	const char* testName = "read_noBytes";
	int theFile = -1;
	int i = 0;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Read.
	theFile = open( testName, O_RDWR );
	size_t numBytesRead = read( theFile, bytesRead, 0 );
	close( theFile );
	
	if( numBytesRead == 0 ){
		for( i = 0; i < gBufSmall; ++i ){
			if( bytesRead[ i ] != 0 ){
				printFail( testName, "Did not expect the buffer to be modified." );
				return;
			}
		}
		printPass( testName );
	}else{
		printFail( testName, "Expected to read 0 bytes." );
	}
}

/**
 * Read one byte.
 **/
void read_oneByte(){
	const char* testName = "read_oneByte";
	int theFile = -1;
	char bytesRead[ gBufSmall ];
	char bytesExpected[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	memset( bytesExpected, 0, gBufSmall );
	bytesExpected[ 0 ] = gData1[ 0 ];
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Read a byte.
	theFile = open( testName, O_RDWR );
	size_t numBytesRead = read( theFile, bytesRead, 1 );
	close( theFile );
	
	if( numBytesRead == 1 ){
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not what was expected." );
		}
	}else{
		printFail( testName, "Expected to read 1 byte." );
	}
}

/**
 * Read several times.
 **/
void read_subsequentReads(){
	const char* testName = "read_subsequentReads";
	int theFile = -1;
	char bytesRead[ gBufSmall ];
	char bytesExpected[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	memset( bytesExpected, 0, gBufSmall );
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Read a byte.  Don't close.
	bytesExpected[ 0 ] = gData1[ 0 ];
	theFile = open( testName, O_RDWR );
	size_t numBytesRead = read( theFile, bytesRead, 1 );
	
	if( numBytesRead == 1 ){
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			// Read the next byte.
			bytesExpected[ 0 ] = gData1[ 1 ];
			numBytesRead = read( theFile, bytesRead, 1 );
		
			if( numBytesRead == 1 ){
				if( strcmp( bytesRead, bytesExpected ) == 0 ){
					printPass( testName );
				}else{
					printFail( testName, "Data was not what was expected after the second read" );
				}
			}else{
				printFail( testName, "Expected to read 1 byte for the second read" );
			}
		}else{
			printFail( testName, "Data was not what was expected after the first read" );
		}
	}else{
		printFail( testName, "Expected to read 1 byte for the first read" );
	}
			
	close( theFile );
}

/**
 * Try to read too much (into a large enough buffer).
 **/
void read_beyondEOF(){
	const char* testName = "read_beyondEOF";
	int theFile = -1;
	char bytesRead[ gBufSmall ];
	char bytesExpected[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	memset( bytesExpected, 0, gBufSmall );
	strcpy( bytesExpected, gData1 );
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Read, passing in a size larger than the file.  
	// e.g. gBufSmall is 16, and the file contains a five character string.
	theFile = open( testName, O_RDWR );
	size_t numBytesRead = read( theFile, bytesRead, gBufSmall );
	close( theFile );
	
	if( numBytesRead == strlen( gData1 ) ){
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not what was expected after the read" );
		}
	}else{
		printFail( testName, "Unexpected number of bytes read" );
	}
}

/**
 * Use an invalid file descriptor.
 **/
void read_ebadf(){
	const char* testName = "read_ebadf";
	int savedErrno = 0;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	errno = 0;
	int numBytesRead = read( -1, bytesRead, 1 );
	savedErrno = errno;
	
	if( numBytesRead == -1 ){
		if( savedErrno == EBADF ){
			printPass( testName );
		}else{
			printFail( testName, "Expected errno EBADF" );
		}
	}else{
		printFail( testName, "Expected bytes read to be -1" );
	}
}

/**
 * Use lseek SEEK_SET to specify a location in the file to begin reading.
 * The goal here is NOT to do a full testing of lseek.  The goal is just
 * to be sure lseek can use the filedes to set the location for reading.
 **/
void read_lseekSet(){
	const char* testName = "read_lseekSet";
	int theFile = -1;
	char bytesRead[ gBufSmall ];
	char bytesExpected[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	memset( bytesExpected, 0, gBufSmall );
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Use lseek & SEEK_SET to read the beginning.
	bytesExpected[ 0 ] = gData1[ 0 ];
	theFile = open( testName, O_RDWR );
	lseek( theFile, 0, SEEK_SET );
	size_t numBytesRead = read( theFile, bytesRead, 1 );
	close( theFile );
	
	if( (numBytesRead = 1) && (strcmp( bytesExpected, bytesRead ) == 0) ){
		// Use lseek & SEEK_SET to read the middle.
		bytesExpected[ 0 ] = gData1[ 2 ];
		theFile = open( testName, O_RDWR );
		lseek( theFile, 2, SEEK_SET );
		size_t numBytesRead = read( theFile, bytesRead, 1 );
		close( theFile );		

		if( (numBytesRead = 1) && (strcmp( bytesExpected, bytesRead ) == 0) ){
			// Use lseek & SEEK_SET to read the end.
			bytesExpected[ 0 ] = gData1[ 4 ];
			theFile = open( testName, O_RDWR );
			lseek( theFile, 4, SEEK_SET );
			size_t numBytesRead = read( theFile, bytesRead, 1 );
			close( theFile );			
			if( (numBytesRead = 1) && (strcmp( bytesExpected, bytesRead ) == 0) ){
				printPass( testName );
			}else{
				printFail( testName, "SEEK_SET at 4 failed" );
			}
		}else{
			printFail( testName, "SEEK_SET at 2 failed" );
		}
	}else{
		printFail( testName, "SEEK_SET at 0 failed" );
	}
}

/**
 * Use lseek SEEK_CUR to specify a location in the file to begin reading.
 * The goal here is NOT to do a full testing of lseek.  The goal is just
 * to be sure lseek can use the filedes to set the location for reading.
 **/
void read_lseekCur(){
	const char* testName = "read_lseekCur";
	int theFile = -1;
	char bytesRead[ gBufSmall ];
	char bytesExpected[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	memset( bytesExpected, 0, gBufSmall );
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Use lseek & SEEK_CUR to read the beginning.
	bytesExpected[ 0 ] = gData1[ 0 ];
	theFile = open( testName, O_RDWR );
	lseek( theFile, 0, SEEK_CUR );
	size_t numBytesRead = read( theFile, bytesRead, 1 );

	if( (numBytesRead = 1) && (strcmp( bytesExpected, bytesRead ) == 0) ){
		// Now "current" is the second byte, and we're going to seek three past that
		// so we should be reading the fifth byte now.
		bytesExpected[ 0 ] = gData1[ 4 ];
		lseek( theFile, 3, SEEK_CUR );
		numBytesRead = read( theFile, bytesRead, 1 );
		if( (numBytesRead = 1) && (strcmp( bytesExpected, bytesRead ) == 0) ){
			printPass( testName );
		}else{
			printFail( testName, "Second SEEK_CUR failed" );
		}
	}else{
		printFail( testName, "Initial SEEK_CUR failed" );
	}
	
	close( theFile );
}

/**
 * Use lseek SEEK_END to specify a location in the file to begin reading.
 * The goal here is NOT to do a full testing of lseek.  The goal is just
 * to be sure lseek can use the filedes to set the location for reading.
 **/
void read_lseekEnd(){
	const char* testName = "read_lseekEnd";
	int theFile = -1;
	char bytesRead[ gBufSmall ];
	char bytesExpected[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	memset( bytesExpected, 0, gBufSmall );
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Use lseek & SEEK_END to read the last byte.
	bytesExpected[ 0 ] = gData1[ 4 ];
	theFile = open( testName, O_RDWR );
	lseek( theFile, -1, SEEK_END );
	size_t numBytesRead = read( theFile, bytesRead, 1 );
	close( theFile );
	
	if( (numBytesRead = 1) && (strcmp( bytesExpected, bytesRead ) == 0) ){
		printPass( testName );
	}else{
		printFail( testName, "SEEK_END failed" );
	}
}

/**
 * Go beyond the end of the file, write some data, and read everything.
 * Also committing a slight sin by using this test to check that write()
 * beyond the end of the file returns the correct value.  My apologies
 * to the Unit Test Gods.
 **/
void read_lseekBeyondEnd1(){
	const char* testName = "read_lseekBeyondEnd1";
	int seekRet = 0;
	int theFile = -1;
	char bytesRead[ gBufLarge ];
	int pass = 1;
	int writeRet = 0;
	int i = 0;
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Use lseek to go beyond the end of the file and write some data.
	theFile = open( testName, O_RDWR );
	lseek( theFile, 5, SEEK_END );
	writeRet = write( theFile, gData2, strlen( gData2 ) );
	
	// The file now contains 'a' 'b' 'c' 'd' 'e' 00000 '0' '1' '2' '3' '4' '5' '6' '7' '8' '9'
	// Let's seek to key points in there and use read() to verify the data.
	// Due to the 0's, don't use strxxx() methods to verify.  Use ascii values.
	
	// Seek to the beginning and read it all.
	memset( bytesRead, 0, gBufLarge );
	errno = 0;
	seekRet = lseek( theFile, 0, SEEK_SET );
	if( writeRet == strlen( gData2 ) ){
		if( (errno == 0) && (seekRet == 0) ){
			errno = 0;
			size_t numBytesRead = read( theFile, bytesRead, 20 );
			if( (errno == 0) && (numBytesRead == 20) ){
				for( i = 0; i < 20; ++i ){
					if( i >= 0 && i <=4 ){
						if( bytesRead[ i ] != 97 + i ){
							printf( "\n" );
							printFail( testName, "One of the letters was wrong" );
							pass = 0;
						}else{
							printf( "%c", bytesRead[ i ] );
						}
					}else if( i >= 5 && i <= 9 ){
						if( bytesRead[ i ] != 0 ){
							printf( "\n" );
							printFail( testName, "One of the zeroes was wrong" );
							pass = 0;
						}else{
							printf( "%d", bytesRead[ i ] );
						}
					}else if( i >= 10 && i <= 19 ){
						if( bytesRead[ i ] != 48 + (i-10) ){
							printf( "\n" );
							printFail( testName, "One of the numbers was wrong" );
							pass = 0;
						}else{
							printf( "%c", bytesRead[ i ] );
						}		
					}else{
						printf( "\n" );
						printFail( testName, "Something is wrong with this test" );
						pass = 0;
					}
				}
			}else{
				printFail( testName, "There was a problem reading" );
			}
		}else{
			printFail( testName, "There was a problem seeking" );
		}
	}else{
		printFail( testName, "There was an invalid return value when writing beyond the end of the file" );
	}
	
	
	close( theFile );
	
	if( pass ){
		printf( "\n" );
		printPass( testName );
	}
}

/**
 * Go beyond the end of the file, write some data, and read the gap.
 **/
void read_lseekBeyondEnd2(){
	const char* testName = "read_lseekBeyondEnd2";
	int seekRet = 0;
	int theFile = -1;
	char bytesRead[ gBufLarge ];
	int pass = 1;
	int i = 0;
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Use lseek to go beyond the end of the file and write some data.
	theFile = open( testName, O_RDWR );
	lseek( theFile, 5, SEEK_END );
	write( theFile, gData2, strlen( gData2 ) );
	
	// The file now contains 'a' 'b' 'c' 'd' 'e' 00000 '0' '1' '2' '3' '4' '5' '6' '7' '8' '9'
	// Let's seek to key points in there and use read() to verify the data.
	// Due to the 0's, don't use strxxx() methods to verify.
	
	// Seek to the gap and read the zeroes that were automatically created.
	memset( bytesRead, 1, gBufLarge );
	errno = 0;
	seekRet = lseek( theFile, 5, SEEK_SET );
	if( (errno == 0) && (seekRet == 5) ){
		errno = 0;
		size_t numBytesRead = read( theFile, bytesRead, 5 );
		if( (errno == 0) && (numBytesRead == 5) ){
			for( i = 0; i < 20; ++i ){
				if( i >= 0 && i <=4 ){
					if( bytesRead[ i ] != 0 ){
						printf( "\n" );
						printFail( testName, "One of the zeroes was wrong." );
						pass = 0;
					}else{
						printf( "%d", bytesRead[ i ] );
					}
				}else if( i >= 5 && i <= 19 ){
					if( bytesRead[ i ] != 1 ){
						printf( "\n" );
						printFail( testName, "One of the ones was wrong." );
						pass = 0;
					}else{
						printf( "%d", bytesRead[ i ] );
					}
				}else{
					printf( "\n" );
					printFail( testName, "Something is wrong with this test." );
					pass = 0;
				}
			}
		}else{
			printFail( testName, "There was a problem reading." );
		}
	}else{
		printFail( testName, "There was a problem seeking." );
	}

	close( theFile );
	
	if( pass ){
		printf( "\n" );
		printPass( testName );
	}
}

/**
 * Go beyond the end of the file, write some data, and read those new bytes.
 **/
void read_lseekBeyondEnd3(){
	const char* testName = "read_lseekBeyondEnd3";
	int seekRet = 0;
	int theFile = -1;
	char bytesRead[ gBufLarge ];
	
	// Create a file containing data for testing.
	theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Use lseek to go beyond the end of the file and write some data.
	theFile = open( testName, O_RDWR );
	lseek( theFile, 5, SEEK_END );
	write( theFile, gData2, strlen( gData2 ) );
	
	// The file now contains 'a' 'b' 'c' 'd' 'e' 00000 '0' '1' '2' '3' '4' '5' '6' '7' '8' '9'
	// Let's seek to key points in there and use read() to verify the data.
	// Due to the 0's, don't use strxxx() methods to verify.
	
	// Seek to the new data and read the zeroes that were automatically created.
	memset( bytesRead, 0, gBufLarge );
	errno = 0;
	seekRet = lseek( theFile, -10, SEEK_END );
	if( (errno == 0) && (seekRet == 10) ){
		errno = 0;
		size_t numBytesRead = read( theFile, bytesRead, 10 );
		if( (errno == 0) && (numBytesRead == 10) && (strcmp( bytesRead, gData2 ) == 0) ){
			printPass( testName );
		}else{
			printFail( testName, "There was a problem reading." );
		}
	}else{
		printFail( testName, "There was a problem seeking." );
	}
	
	close( theFile );
}
