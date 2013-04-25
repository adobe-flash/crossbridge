#include "AS3/AS3.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "../../framework/vfs.h"

void runCompressedVFSTests(){
	compressed_basic();
	compressed_empty();
	compressed_singleQuotes();
	compressed_grave();
	compressed_space();
	compressed_space2();
	compressed_longName();
	compressed_longDir();
	//compressed_unicode();
	compressed_dotFile();
	compressed_dotDotFile();
	compressed_dotDir();
	compressed_dotDotDir();

	// I/O API
	compressed_statFile();
	compressed_statDir();
	compressed_open();
	compressed_close();
	compressed_read();
	compressed_write();
	compressed_newFile();
	compressed_newDir();
	compressed_getdirentries();
	compressed_gif();
	compressed_json();
	compressed_xml();
	compressed_png();
}

/**
 * Simple file.
 **/
void compressed_basic(){
	const char* testName = "compressed_basic";
	const char* fileName = "compressed/basic";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Empty file.
 **/
void compressed_empty(){
	const char* testName = "compressed_empty";
	const char* fileName = "compressed/empty";
	
	verifyFileContents( testName, fileName, 0 );
}

/**
 * Single quotes
 **/
void compressed_singleQuotes(){
	const char* testName = "compressed_singleQuotes";
	const char* fileName = "compressed/\'squote\'";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Grave
 **/
void compressed_grave(){
	const char* testName = "compressed_grave";
	const char* fileName = "compressed/`grave";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a space in its name.
 **/
void compressed_space(){
	const char* testName = "compressed_space";
	const char* fileName = "compressed/spa ce";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a space at the beginning of its name.
 **/
void compressed_space2(){
	const char* testName = "compressed_space2";
	const char* fileName = "compressed/ space2";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a long name.
 **/
void compressed_longName(){
	const char* testName = "compressed_longName";
	const char* fileName = "compressed/startchars012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567end.txt";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * File with a long name.
 **/
void compressed_longDir(){
	const char* testName = "compressed_longDir";
	const char* fileName = "compressed/startchars0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234end/thefile.txt";
	
	verifyFileContents( testName, fileName, gData1 );
}

/**
 * Unicode
 **/
/**
void compressed_unicode(){
	const char* testName = "compressed_unicode";
	const char* fileName = "unicode/ﾃﾄﾅ";
	verifyFileContents( testName, fileName, gData1 );
}
**/

/**
 * File starting with a dot.
 **/
void compressed_dotFile(){
	const char* testName = "compressed_dotFile";
	const char* fileName = "compressed/.dot";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * File starting with 2 dots.
 **/
void compressed_dotDotFile(){
	const char* testName = "compressed_dotDotFile";
	const char* fileName = "compressed/..dotdot";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Dir starting with a dot.
 **/
void compressed_dotDir(){
	const char* testName = "compressed_dotDir";
	const char* fileName = "compressed/.dotdir/file.txt";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Dir starting with 2 dots.
 **/
void compressed_dotDotDir(){
	const char* testName = "compressed_dotDotDir";
	const char* fileName = "compressed/..dotdotdir/file.txt";

	verifyFileContents( testName, fileName, gData1 );
	//printFail( testName, "Fails due to ALC-421" );
}

/**
 * Call stat() on an compressed file.
 **/
void compressed_statFile(){
	const char* testName = "compressed_statFile";
	const char* fileName = "compressed/basic";
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
 * Call stat() on an compressed dir.
 **/
void compressed_statDir(){
	const char* testName = "compressed_statDir";
	const char* dirName = "compressed";
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
void compressed_newDir(){
	const char* testName = "compressed_newDir";
	const char* dirName = "compressed/newdir";

	errno = 0;
	int ret = mkdir( dirName, 0777 );
	int savedErrno = errno;
	
	if( (ret == 0) && (savedErrno == 0) ){
		printPass( testName );
	}else{
		//printf( "compressed_newDir: ret=%d, error=%s\n", ret, strerror( savedErrno ) );
		printFail( testName, "Fails due to ALC-426. (Fixed)" );
	}
}

void compressed_newFile(){
	const char* testName = "compressed_newFile";
	const char* fileName = "compressed/newfile";
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
				//printf( "compressed_newFile: expected=%s, actual=%s", gData1, bytesRead );
				printFail( testName, "Data was not as expected" );
			}
		}else{
			printFail( testName, "File write failed" );
		}
	}else{
		//printf( "compressed_newFile: ret=%d, error=%s\n", ret, strerror( savedErrno ) );
		printFail( testName, "Fails due to ALC-426. (Fixed)" );
	}
}

/**
 * Open an compressed file.
 **/
void compressed_open(){
	const char* testName = "compressed_open";
	const char* fileName = "compressed/basic";
	
	errno = 0;
	int theFile = open( fileName, O_RDONLY );
	int savedErrno = errno;
	
	if( (theFile > -1) && (errno == 0) ){
		printPass( testName );
	}else{
		//printf( "compressed_open: theFile=%d, error=%s\n", theFile, strerror( savedErrno ) );
		printFail( testName, "Expected valid file descriptor" );
	}
	
	close( theFile );	
}

/**
 * Try to use a closed file.
 **/
void compressed_close(){
	const char* testName = "compressed_close";
	const char* fileName = "compressed/basic";
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
void compressed_read(){
	const char* testName = "compressed_read";
	const char* fileName = "compressed/basic";
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
void compressed_write(){
	const char* testName = "compressed_write";
	const char* fileName = "compressed/writeToMe";
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
void compressed_getdirentries(){
	const char* testName = "compressed_getdirentries";
	const char* dirName = "compressed/dir";
	const char* fileName = "compressed/dir/file_in_dir.txt";
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
void compressed_gif(){
	const char* testName = "compressed_gif";
	const char* fileName = "compressed/testgif.gif";
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
void compressed_json(){
	const char* testName = "compressed_json";
	const char* fileName = "compressed/testjson.json";
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
void compressed_xml(){
	const char* testName = "compressed_xml";
	const char* fileName = "compressed/testxml.xml";
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
void compressed_png(){
	const char* testName = "compressed_png";
	const char* fileName = "compressed/testpng.png";
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
