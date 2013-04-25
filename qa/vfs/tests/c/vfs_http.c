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

void runWebVFSTests(){
	web_basic();
	web_empty();
	web_singleQuotes();
	web_grave();
	web_space();
	web_space2();
	web_longName();
	web_longDir();
	web_dotFile();
	web_dotDotFile();
	web_dotDir();
	web_dotDotDir();
	
	// I/O API
	web_statFile();
	web_statDir();
	web_open();
	web_close();
	web_read();
	web_write();
	web_newFile();
	web_newDir();
	web_getdirentries();
	web_gif();
	web_json();
	web_xml();
	web_png();
	web_open_rdonlyfs_rdonly();
	web_open_rdonlyfs_rdwr();
	web_open_rdonlyfs_creat();
}

/**
 * Simple file.
 **/
void web_basic(){
	const char* testName = "web_basic";
	const char* fileName = "webvfs/basic";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Empty file.
 **/
void web_empty(){
	const char* testName = "web_empty";
	const char* fileName = "webvfs/empty";
	
	verifyFileContents( testName, fileName, 0 );
}

/**
 * Single quotes
 **/
void web_singleQuotes(){
	const char* testName = "web_singleQuotes";
	const char* fileName = "webvfs/\'squote\'";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Grave
 **/
void web_grave(){
	const char* testName = "web_grave";
	const char* fileName = "webvfs/`grave";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a space in its name.
 **/
void web_space(){
	const char* testName = "web_space";
	const char* fileName = "webvfs/spa ce";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a space at the beginning of its name.
 **/
void web_space2(){
	const char* testName = "web_space2";
	const char* fileName = "webvfs/ space2";
	
	//printFail( testName, "Fails due to ALC-422" );
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a long name.
 **/
void web_longName(){
	const char* testName = "web_longName";
	const char* fileName = "webvfs/start012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678end.txt";	
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a long name.
 **/
void web_longDir(){
	const char* testName = "web_longDir";
	const char* fileName = "webvfs/start0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890end/thefile.txt";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Unicode
 **/
/**
 void web_unicode(){
 const char* testName = "web_unicode";
 const char* fileName = "unicode/ﾃﾄﾅ";
 verifyFileContents( testName, fileName, gData1 );
 }
 **/

/**
 * File starting with a dot.
 **/
void web_dotFile(){
	const char* testName = "web_dotFile";
	const char* fileName = "webvfs/.dot";
	
	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * File starting with 2 dots.
 **/
void web_dotDotFile(){
	const char* testName = "web_dotDotFile";
	const char* fileName = "webvfs/..dotdot";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Dir starting with a dot.
 **/
void web_dotDir(){
	const char* testName = "web_dotDir";
	const char* fileName = "webvfs/.dotdir/file.txt";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Dir starting with 2 dots.
 **/
void web_dotDotDir(){
	const char* testName = "web_dotDotDir";
	const char* fileName = "webvfs/..dotdotdir/file.txt";
	
	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Call stat() on a file.
 **/
void web_statFile(){
	const char* testName = "web_statFile";
	const char* fileName = "webvfs/basic";
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
 * Call stat() on a dir.
 **/
void web_statDir(){
	const char* testName = "web_statDir";
	const char* dirName = "webvfs";
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
		printFail( testName, "Expected errno == 0 and ret == 0.  Fails due to ALC-423." );
	}
}

/** 
 * Create a new directory.
 **/
void web_newDir(){
	const char* testName = "web_newDir";
	const char* dirName = "webvfs/newdir";
	
	errno = 0;
	int ret = mkdir( dirName, 0777 );
	int savedErrno = errno;
	
	if( (ret == 0) && (savedErrno == 0) ){
		printPass( testName );
	}else{
		//printf( "web_newDir: ret=%d, error=%s\n", ret, strerror( savedErrno ) );
		printFail( testName, "Fails due to ALC-426." );
	}
}

void web_newFile(){
	const char* testName = "web_newFile";
	const char* fileName = "webvfs/newfile";
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
				//printf( "web_newFile: expected=%s, actual=%s", gData1, bytesRead );
				printFail( testName, "Data was not as expected" );
			}
		}else{
			printFail( testName, "File write failed" );
		}
	}else{
		//printf( "web_newFile: ret=%d, error=%s\n", ret, strerror( savedErrno ) );
		printFail( testName, "Fails due to ALC-426." );
	}
}

/**
 * Open a file.
 **/
void web_open(){
	const char* testName = "web_open";
	const char* fileName = "webvfs/basic";
	
	errno = 0;
	int theFile = open( fileName, O_RDONLY );
	int savedErrno = errno;
	
	if( (theFile > -1) && (errno == 0) ){
		printPass( testName );
	}else{
		printf( "web_open: theFile=%d, error=%s\n", theFile, strerror( savedErrno ) );
		printFail( testName, "Expected valid file descriptor" );
	}
	
	close( theFile );	
}

/**
 * Try to use a closed file.
 **/
void web_close(){
	const char* testName = "web_close";
	const char* fileName = "webvfs/basic";
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
void web_read(){
	const char* testName = "web_read";
	const char* fileName = "webvfs/basic";
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
void web_write(){
	const char* testName = "web_write";
	const char* fileName = "webvfs/writeToMe";
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
void web_getdirentries(){
	const char* testName = "web_getdirentries";
	const char* dirName = "webvfs/dir";
	const char* fileName = "webvfs/dir/file_in_dir.txt";
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
 * Check a loaded gif.
 **/
void web_gif(){
	const char* testName = "web_gif";
	const char* fileName = "webvfs/testgif.gif";
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
void web_json(){
	const char* testName = "web_json";
	const char* fileName = "webvfs/testjson.json";
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
void web_xml(){
	const char* testName = "web_xml";
	const char* fileName = "webvfs/testxml.xml";
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
void web_png(){
	const char* testName = "web_png";
	const char* fileName = "webvfs/testpng.png";
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
void web_open_rdonlyfs_rdonly(){
	const char* testName = "web_open_rdonlyfs_rdonly";
	char* bytesRead = ( char* )calloc( gBufSmall, sizeof( char ) );
	int numBytesRead = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "webvfs_readonly/basic", O_RDONLY );
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
void web_open_rdonlyfs_rdwr(){
	const char* testName = "web_open_rdonlyfs_rdwr";
	const char* expectedError = "Read-only file system";
	char* error = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "webvfs_readonly/basic", O_RDWR );
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
void web_open_rdonlyfs_creat(){
	const char* testName = "web_open_rdonlyfs_creat";
	const char* expectedError = "Read-only file system";
	char* error = 0;
	int theFile = -1;
	
	errno = 0;
	theFile = open( "webvfs_readonly/open_rdonlyfs_creat", O_CREAT );
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
