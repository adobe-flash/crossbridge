#include "AS3/AS3.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "../../framework/vfs.h"

void runEmbeddedVFSTests(){
	embedded_basic();
	embedded_empty();
	embedded_singleQuotes();
	embedded_grave();
	embedded_space();
	embedded_space2();
	embedded_longName();
	embedded_longDir();
	//embedded_unicode();
	embedded_dotFile();
	embedded_dotDotFile();
	embedded_dotDir();
	embedded_dotDotDir();

	// I/O API
	embedded_statFile();
	embedded_statDir();
	embedded_open();
	embedded_close();
	embedded_read();
	embedded_write();
	embedded_newFile();
	embedded_newDir();
	embedded_getdirentries();
	embedded_gif();
	embedded_json();
	embedded_xml();
	embedded_png();
	embedded_open_rdonlyfs_rdonly();
	embedded_open_rdonlyfs_rdwr();
	embedded_open_rdonlyfs_creat();
}

/**
 * Simple file.
 **/
void embedded_basic(){
	const char* testName = "embedded_basic";
	const char* fileName = "embedded/basic";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Empty file.
 **/
void embedded_empty(){
	const char* testName = "embedded_empty";
	const char* fileName = "embedded/empty";
	
	verifyFileContents( testName, fileName, 0 );
}

/**
 * Single quotes
 **/
void embedded_singleQuotes(){
	const char* testName = "embedded_singleQuotes";
	const char* fileName = "embedded/\'squote\'";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Grave
 **/
void embedded_grave(){
	const char* testName = "embedded_grave";
	const char* fileName = "embedded/`grave";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a space in its name.
 **/
void embedded_space(){
	const char* testName = "embedded_space";
	const char* fileName = "embedded/spa ce";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a space at the beginning of its name.
 **/
void embedded_space2(){
	const char* testName = "embedded_space2";
	const char* fileName = "embedded/ space2";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a long name.
 **/
void embedded_longName(){
	const char* testName = "embedded_longName";
	const char* fileName = "embedded/start255chars34567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012end.txt";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a long name.
 **/
void embedded_longDir(){
	const char* testName = "embedded_longDir";
	const char* fileName = "embedded/start255chars345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234end/thefile.txt";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Unicode
 **/
/**
void embedded_unicode(){
	const char* testName = "embedded_unicode";
	const char* fileName = "unicode/ﾃﾄﾅ";
	verifyFileContents( testName, fileName, gData1 );
}
**/

/**
 * File starting with a dot.
 **/
void embedded_dotFile(){
	const char* testName = "embedded_dotFile";
	const char* fileName = "embedded/.dot";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * File starting with 2 dots.
 **/
void embedded_dotDotFile(){
	const char* testName = "embedded_dotDotFile";
	const char* fileName = "embedded/..dotdot";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Dir starting with a dot.
 **/
void embedded_dotDir(){
	const char* testName = "embedded_dotDir";
	const char* fileName = "embedded/.dotdir/file.txt";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Dir starting with 2 dots.
 **/
void embedded_dotDotDir(){
	const char* testName = "embedded_dotDotDir";
	const char* fileName = "embedded/..dotdotdir/file.txt";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Call stat() on an embedded file.
 **/
void embedded_statFile(){
	const char* testName = "embedded_statFile";
	const char* fileName = "embedded/basic";
	struct stat statbuf;
	int ret = 0;
	
	errno = 0;
	ret = stat( fileName, &statbuf );
	
	if( (errno == 0) && (ret == 0) ){
		if( statbuf.st_mode == S_IFREG ){
			if( statbuf.st_nlink == 1 ){
				if( statbuf.st_blksize == gBufBlockSize ){
					if( statbuf.st_blocks == 1 ){
						if( statbuf.st_size == 5 ){
							printPass( testName );
						}else{
							printFail( testName, "Expected size = 4096" );
						}							
					}else{
						printFail( testName, "Expected st_blocks = 1" );
					}
				}else{
					printFail( testName, "Expected st_blksize = 4096" );
				}
			}else{
				printFail( testName, "Expected st_nlink = 1" );
			}
		}else{
			printFail( testName, "Expected st_mode = S_IFREG" );
		}
	}else{
		printFail( testName, "Expected errno == 0 and ret == 0" );
	}
}

/**
 * Call stat() on an embedded dir.
 **/
void embedded_statDir(){
	const char* testName = "embedded_statDir";
	const char* dirName = "embedded";
	struct stat statbuf;
	int ret = 0;

	errno = 0;
	ret = stat( dirName, &statbuf );

	if( (errno == 0) && (ret == 0) ){
		if( statbuf.st_mode == S_IFDIR ){
			if( statbuf.st_nlink == 1 ){
				if( statbuf.st_blksize == 4096 ){
					if( statbuf.st_blocks == 0 ){
						if( statbuf.st_size == 0 ){
							printPass( testName );
						}else{
							printFail( testName, "Expected size = 0" );
						}							
					}else{
						printFail( testName, "Expected st_blocks = 0" );
					}
				}else{
					printFail( testName, "Expected st_blksize = 4096" );
				}
			}else{
				printFail( testName, "Expected st_nlink = 1" );
			}
		}else{
			printFail( testName, "Expected st_mode = S_IFDIR" );
		}
	}else{
		//printf( "test: %s, dir: %s, ret: %d, error: %s\n", testName, dirName, ret, strerror( errno ) );
		printFail( testName, "Expected errno == 0 and ret == 0.  Fails due to ALC-423. (Fixed)" );
	}
}

/** 
 * Create a new directory.  Write to it and read from it.
 **/
void embedded_newDir(){
	const char* testName = "embedded_newDir";
	const char* dirName = "embedded/newdir";

	errno = 0;
	int ret = mkdir( dirName, 0777 );
	int savedErrno = errno;
	
	if( (ret == 0) && (savedErrno == 0) ){
		printPass( testName );
	}else{
		//printf( "embedded_newDir: ret=%d, error=%s\n", ret, strerror( savedErrno ) );
		printFail( testName, "Fails due to ALC-426. (Fixed)" );
	}
}

void embedded_newFile(){
	const char* testName = "embedded_newFile";
	const char* fileName = "embedded/newfile";
	char bytesRead[ gBufSmall ];
	int ret = 0;
	
	errno = 0;
	int theFile = open( fileName, O_RDWR | O_CREAT );
	int savedErrno = errno;
	if( theFile > -1 ){
		ret = write( theFile, gData1, strlen( gData1 ) );
		
		if( ret == strlen( gData1 ) ){
			close( theFile );
			theFile = open( fileName, O_RDONLY );
			memset( bytesRead, 0, gBufSmall );
			size_t numBytesRead = read( theFile, bytesRead, strlen( gData1 ) );
			close( theFile );
			
			if( strcmp( bytesRead, gData1 ) == 0 ){
				printPass( testName );
			}else{
				//printf( "embedded_newFile: expected=%s, actual=%s", gData1, bytesRead );
				printFail( testName, "Data was not as expected" );
			}
		}else{
			printFail( testName, "File write failed" );
		}
	}else{
		//printf( "embedded_newFile: ret=%d, error=%s\n", ret, strerror( savedErrno ) );
		printFail( testName, "Fails due to ALC-426. (Fixed)" );
	}
}

/**
 * Open an embedded file.
 **/
void embedded_open(){
	const char* testName = "embedded_open";
	const char* fileName = "embedded/basic";
	
	errno = 0;
	int theFile = open( fileName, O_RDONLY );
	int savedErrno = errno;
	
	if( (theFile > -1) && (errno == 0) ){
		printPass( testName );
	}else{
		//printf( "embedded_open: theFile=%d, error=%s\n", theFile, strerror( savedErrno ) );
		printFail( testName, "Expected valid file descriptor" );
	}
	
	close( theFile );	
}

/**
 * Try to use a closed file.
 **/
void embedded_close(){
	const char* testName = "embedded_close";
	const char* fileName = "embedded/basic";
	int ret = 0;

	errno = 0;
	int theFile = open( fileName, O_RDONLY );
	
	if( ( theFile > -1 ) && ( errno == 0 ) ){
		close( theFile );
		
		// Try to write to the closed filedesc.
		errno = 0;
		ret = write( theFile, gData1, strlen( gData1 ) );
		
		if( (errno == EBADF) && (ret == -1) ){
			printPass( testName );
		}else{
			printFail( testName, "Expected EBADF and a return value of -1" );
		}
	}else{
		printFail( testName, "Could not open the file" );
	}
}

/**
 * Read a byte.
 **/
void embedded_read(){
	const char* testName = "embedded_read";
	const char* fileName = "embedded/basic";
	int theFile = -1;
	char bytesRead[ gBufSmall ];
	char bytesExpected[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	memset( bytesExpected, 0, gBufSmall );
	bytesExpected[ 0 ] = gData1[ 0 ];
	
	// Read a byte.
	theFile = open( fileName, O_RDONLY );
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
 * Write some bytes.
 **/
void embedded_write(){
	const char* testName = "embedded_write";
	const char* fileName = "embedded/writeToMe";
	const char* bytesExpected = "ab01234";
	int theFile = -1;
	int retVal = 0;
	int savedErrno = 0;
	char bytesRead[ gBufSmall ];
	memset( bytesRead, 0, gBufSmall );
	
	// Open the file for TRUNC and write some fresh data.
	theFile = open( fileName, O_RDWR );

	write( theFile, gData1, strlen( gData1 ) );
	savedErrno = errno;	
	close( theFile );
	
	// Starting at the third byte, overwrite with 5 bytes from gData2.
	theFile = open( fileName, O_RDWR );
	lseek( theFile, 2, SEEK_SET );

	errno = 0;
	retVal = write( theFile, gData2, 5 );
	savedErrno = errno;
	close( theFile );

	// See what we got.
	theFile = open( fileName, O_RDONLY );
	read( theFile, bytesRead, gBufSmall );
	close( theFile );
	
	if( (retVal == 5) && (savedErrno == 0) ){
		if( strcmp( bytesRead, bytesExpected ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Data was not as expected" );
		}
	}else{
		printFail( testName, "Expected retVal == 5 and errno == 0" );
	}

}

/**
 * Read a directory.
 **/
void embedded_getdirentries(){
	const char* testName = "embedded_getdirentries";
	const char* dirName = "embedded/dir";
	const char* fileName = "embedded/dir/file_in_dir.txt";
	const char* expectedName = "file_in_dir.txt";
	char buf[ gBufBlockSize ];
	int ret = 0;
	int theFile = 0;
	long basep = 0;
	struct dirent* dirEntry = 0;
	
	memset( buf, 0, gBufBlockSize );
	
	// Open the directory and get info. about it.
	theFile = open( dirName, O_RDONLY );
	errno = 0;
	ret = getdirentries( theFile, buf, gBufBlockSize, &basep );
	int savedErrno = errno;
	
	if( (ret > 0) && (errno == 0) ){
		dirEntry = (struct dirent*)buf;
		
		if( dirEntry->d_fileno > 0 ){
			if( dirEntry->d_reclen > 0 ){
				if( dirEntry->d_type == DT_REG ){
					if( dirEntry->d_namlen == strlen( expectedName ) ){
						if( strcmp( dirEntry->d_name, expectedName ) == 0 ){
							printPass( testName );
						}else{
							printFail( testName, "d_name was not correct" );
						}						
					}else{
						printFail( testName, "d_namelen was not correct" );
					}
				}else{
					printFail( testName, "d_type was not correct" );
				}
			}else{
				printFail( testName, "d_reclen was not correct" );
			}
		}else{
			printFail( testName, "d_fileno was not correct" );
		}
	}else{
		printFail( testName, "Call to getdirentries() failed.  Fails due to bug ALC-425. (Fixed)" );
	}
}

/**
 * Read a loaded gif.
 **/
void embedded_gif(){
	const char* testName = "embedded_gif";
	const char* fileName = "embedded/testgif.gif";
	int dataOK = 0;
	size_t numBytesRead = 0;
	uint8_t bytesRead[ gBufLarge ];
	int theFile = open( fileName, O_RDONLY );
	int savedErrno = errno;
	
	memset( bytesRead, 0, gBufLarge );		
	
	if( theFile > -1 ){
		numBytesRead = read( theFile, bytesRead, gBufLarge );
		for( int i = 0; i < numBytesRead; ++i ){
			if( bytesRead[ i ] != testGifBytes[ i ] ){
				dataOK = 0;
			}else{
				dataOK = 1;
			}
			
			if( !dataOK ){
				break;
			}
		}
		
		if( dataOK ){
			printPass( testName );
		}else{
			printFail( testName, "Data incorrect" );
		}
	}else{
		printf("Could not find the gif.");
	}
	
}
/**
 * Check a loaded json.
 **/
void embedded_json(){
	const char* testName = "embedded_json";
	const char* fileName = "embedded/testjson.json";
	int dataOK = 0;
	size_t numBytesRead = 0;
	uint8_t bytesRead[ gBufLarge ];
	int theFile = open( fileName, O_RDONLY );
	int savedErrno = errno;
	
	memset( bytesRead, 0, gBufLarge );		
	
	if( theFile > -1 ){
		numBytesRead = read( theFile, bytesRead, gBufLarge );
		
		for( int i = 0; i < numBytesRead; ++i ){
			
			if( bytesRead[ i ] != testJsonBytes[ i ] ){
				dataOK = 0;
			}else{
				dataOK = 1;
			}
			
			if( !dataOK ){
				break;
			}
		}
		
		if( dataOK ){
			printPass( testName );
		}else{
			printFail( testName, "Data incorrect" );
		}
	}else{
		printf("Could not find the json file.");
	}	
}

/**
 * Check loaded xml.
 **/
void embedded_xml(){
	const char* testName = "embedded_xml";
	const char* fileName = "embedded/testxml.xml";
	int dataOK = 0;
	size_t numBytesRead = 0;
	uint8_t bytesRead[ gBufLarge ];
	int theFile = open( fileName, O_RDONLY );
	int savedErrno = errno;
	
	memset( bytesRead, 0, gBufLarge );		
	
	if( theFile > -1 ){
		numBytesRead = read( theFile, bytesRead, gBufLarge );
		
		for( int i = 0; i < numBytesRead; ++i ){
			if( bytesRead[ i ] != testXmlBytes[ i ] ){
				dataOK = 0;
			}else{
				dataOK = 1;
			}
			
			if( !dataOK ){
				break;
			}
		}
		
		if( dataOK ){
			printPass( testName );
		}else{
			printFail( testName, "Data incorrect" );
		}
	}else{
		printf("Could not find the xml file.");
	}
}

/**
 * Check loaded png.
 **/
void embedded_png(){
	const char* testName = "embedded_png";
	const char* fileName = "embedded/testpng.png";
	int dataOK = 0;
	size_t numBytesRead = 0;
	uint8_t bytesRead[ gBufHuge ];
	int theFile = open( fileName, O_RDONLY );
	int savedErrno = errno;
	
	memset( bytesRead, 0, gBufHuge );		
	
	if( theFile > -1 ){
		numBytesRead = read( theFile, bytesRead, gBufHuge );
		
		for( int i = 0; i < numBytesRead; ++i ){
			if( bytesRead[ i ] != testPngBytes[ i ] ){
				dataOK = 0;
			}else{
				dataOK = 1;
			}
			
			if( !dataOK ){
				break;
			}
		}
		
		if( dataOK ){
			printPass( testName );
		}else{
			printFail( testName, "Data incorrect" );
		}
	}else{
		printf("Could not find the png file.");
	}
}

/**
 * Open a file in the read only file system as read only.
 **/
void embedded_open_rdonlyfs_rdonly(){
	const char* testName = "embedded_open_rdonlyfs_rdonly";
	char* bytesRead = ( char* )calloc( gBufSmall, sizeof( char ) );
	int numBytesRead = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "embedded_readonly/basic", O_RDONLY );
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
void embedded_open_rdonlyfs_rdwr(){
	const char* testName = "embedded_open_rdonlyfs_rdwr";
	const char* expectedError = "Read-only file system";
	char* error = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "embedded_readonly/basic", O_RDWR );
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
void embedded_open_rdonlyfs_creat(){
	const char* testName = "embedded_open_rdonlyfs_creat";
	const char* expectedError = "Read-only file system";
	char* error = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "embedded_readonly/open_rdonlyfs_creat", O_CREAT );
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
