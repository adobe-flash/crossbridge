#include "../../framework/vfs.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/**
 * int getdirentries(
 *	int fd (file desc)
 *	char *buf (buffer) 
 *	nbytes (amount of data to put in the buffer, >= file block size (from stat))
 *	long *basep (the position in the buffer to start writing. "The current position
 *				pointer should only be set to a value returned by lseek(2), a value
 *				returned in the location pointed to by basep (getdirentries() only) or
 *				zero.")
 *	);
 *
 * The data in the buffer is a series of dirent structures each containing
 * the following entries:
 *	u_int32_t d_fileno; (Unique ID in the file system)
 *	u_int16_t d_reclen; (Length of this directory record)
 *	u_int8_t  d_type; (sys/dirent.h)
 *	u_int8_t  d_namlen; (File's name)
 *	char    d_name[MAXNAMELEN + 1]; (Length of the file's name)
 *
 * For foo.txt:
 *	printf( "d_fileno=%d, d_reclen=%d, d_type=%d, d_namlen=%d, d_name=%s", 
 *			dirEntry->d_fileno, dirEntry->d_reclen, dirEntry->d_type, dirEntry->d_namlen, dirEntry->d_name );
 * gives:
 *	d_fileno=1, d_reclen=16, d_type=8, d_namlen=7, d_name=foo.txt
 **/

void runGetdirentriesTests(){
	getdirentries_oneFile();
	getdirentries_oneDir();
	getdirentries_filesAndDirs();
	getdirentries_limitBytes();
	getdirentries_basep();
}

/**
 * One file
 **/
void getdirentries_oneFile(){
	const char* testName = "getDirEntries_oneFile";
	const char* dirName = "/getDirEntries_oneFile";
	const char* fileName = "/getDirEntries_oneFile/foo.txt";
	const char* expectedName = "foo.txt";
	char buf[ gBufBlockSize ];
	int ret = 0;
	int theFile = 0;
	long basep = 0;
	struct dirent* dirEntry = 0;
	
	memset( buf, 0, gBufBlockSize );
	
	// Make a directory with a file.
	ret = mkdir( dirName, 0777 );
	theFile = open( fileName, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );
	
	// Open the directory and get info. about it.
	theFile = open( dirName, O_RDONLY );
	errno = 0;
	ret = getdirentries( theFile, buf, gBufBlockSize, &basep );
	
	if( (ret > 0) && (errno == 0) ){
		dirEntry = (struct dirent*)buf;
		
		if( dirEntry->d_fileno > 0 ){
			if( dirEntry->d_reclen == 16 ){
				if( dirEntry->d_type == DT_REG ){
					if( dirEntry->d_namlen == 7 ){
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
		printFail( testName, "Call to getdirentries() failed" );
	}		
}

/**
 * One directory
 **/
void getdirentries_oneDir(){
	const char* testName = "getDirEntries_oneDir";
	const char* subdirName = "getDirEntries_oneDir/foodir";	
	const char* expectedName = "foodir";
	char buf[ gBufBlockSize ];
	int ret = 0;
	int theDir = 0;
	long basep = 0;
	struct dirent* dirEntry = 0;
	memset( buf, 0, gBufBlockSize );

	// Make a directory with a directory.
	mkdir( testName, 0777 );
	mkdir( subdirName, 0777 );

	// Open the directory and get info. about it.
	theDir = open( testName, O_RDONLY );
	ret = getdirentries( theDir, buf, gBufBlockSize, &basep );
	if( (ret > 0) && (errno == 0) ){
		dirEntry = (struct dirent*)buf;
		
		if( dirEntry->d_fileno > 0 ){
			if( dirEntry->d_reclen == 16 ){
				if( dirEntry->d_type == DT_DIR ){
					if( dirEntry->d_namlen == 6 ){
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
		printFail( testName, "Call to getdirentries() failed" );
	}
}

/**
 * Several files and directories.
 **/
void getdirentries_filesAndDirs(){
	const char* testName = "getdirentries_filesAndDirs";
	const char* mainDirName = "/getdirentries_filesAndDirs/main";
	const char* subdir1Name = "/getdirentries_filesAndDirs/main/dir1";
	const char* subdir2Name = "/getdirentries_filesAndDirs/main/directory 2";
	const char* subdir3Name = "/getdirentries_filesAndDirs/main/dir1/dir3"; // This should not get returned.
	const char* file1Name = "/getdirentries_filesAndDirs/main/file1";
	const char* file2Name = "/getdirentries_filesAndDirs/main/file number 2";
	const char* expected1 = "dir1";
	const char* expected2 = "directory 2";
	const char* expected3 = "file1";
	const char* expected4 = "file number 2";
	
	char buf[ gBufLarge ]; // This is 1024.  We don't need all of that.
	long basep;
	int ret = 0;
	int theFile = 0;
	int theDir = 0;
	struct dirent* dirEntry = 0;

	memset( buf, 0, gBufLarge );
	
	// Make a directory with files and directories.
	mkdir( testName, 0777 );
	mkdir( mainDirName, 0777 );
	mkdir( subdir1Name, 0777 );
	mkdir( subdir2Name, 0777 );
	mkdir( subdir3Name, 0777 );
	
	theFile = open( file1Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	theFile = open( file2Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	// Open the directory and get info. about it.
	theDir = open( mainDirName, O_RDONLY );
	ret = getdirentries( theDir, buf, gBufLarge, &basep );
	
	// Now we should have 4 dirent structures in a buffer.
	int bytesToRead = ret;
	int bytesRead = 0;
	int recLength = 0;
	char* offset = 0;
	int found1 = 0;
	int found2 = 0;
	int found3 = 0;
	int found4 = 0;

	dirent* dirPtr = (dirent*)&buf;
	
	while( bytesRead < bytesToRead ){
		recLength = dirPtr->d_reclen;
		
		// Check values.
		if( strcmp( dirPtr->d_name, expected1 ) == 0 ){
			if( dirPtr->d_reclen == 16 ){
				if( dirPtr->d_type == DT_DIR ){
					found1 = 1;
				}else{
					printFail( testName, "Wrong type" );
				}
			}else{
				printFail( testName, "Wrong length" );
			}
		}else if( strcmp( dirPtr->d_name, expected2 ) == 0 ){
			if( dirPtr->d_reclen == 20 ){
				if( dirPtr->d_type == DT_DIR ){
					found2 = 1;
				}else{
					printFail( testName, "Wrong type" );
				}
			}else{
				printFail( testName, "Wrong length" );
			}
		}else if( strcmp( dirPtr->d_name, expected3 ) == 0 ){
			if( dirPtr->d_reclen == 16 ){
				if( dirPtr->d_type == DT_REG ){
					found3 = 1;
				}else{
					printFail( testName, "Wrong type" );
				}
			}else{
				printFail( testName, "Wrong length" );
			}
		}else if( strcmp( dirPtr->d_name, expected4 ) == 0 ){
			if( dirPtr->d_reclen == 24 ){
				if( dirPtr->d_type == DT_REG ){
					found4 = 1;
				}else{
					printFail( testName, "Wrong type" );
				}
			}else{
				printFail( testName, "Wrong length" );
			}
		}
		bytesRead += recLength;
		dirPtr = (dirent*) ((char*)dirPtr + recLength);
	}
	
	if( found1 && found2 && found3 && found4 ){
		printPass( testName );
	}else{
		printFail( testName, "Did not find all entries" );
	}
}

/**
 * Ask for just a few bytes.
 **/
void getdirentries_limitBytes(){
	const char* testName = "getdirentries_limitBytes";
	const char* file1Name = "getdirentries_limitBytes/file1";
	const char* file2Name = "getdirentries_limitBytes/file2";
	const char* expected1 = "file1";
	const char* expected2 = "file2";
	int gotFile = 0;
	int dataLimited = 0;
	
	char buf[ gBufLarge ];
	long basep;
	int ret = 0;
	int theFile = 0;
	int theDir = 0;
	struct dirent* dirEntry = 0;
	
	memset( buf, 0, gBufLarge );
	
	// Make a directory with files and directories.
	mkdir( testName, 0777 );
	
	theFile = open( file1Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );
	
	theFile = open( file2Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );
	
	// Open the directory and get info. about it.
	// 32 bytes are retrieved if we get *all*, so let's
	// try getting 16 at a time.
	theDir = open( testName, O_RDONLY );
	ret = getdirentries( theDir, buf, 16, &basep );
	dirent* dirPtr = (dirent*)&buf;

	if( ret == 16 ){
		if( (strcmp( expected1, dirPtr->d_name ) == 0) || (strcmp( expected2, dirPtr->d_name ) == 0) ){
			gotFile = 1;
		}else{
			printf( "Don't know what file %s is", dirPtr->d_name );
		}
		
		dirPtr = (dirent*) ((char*)dirPtr + dirPtr->d_reclen);
		// Now we're pointing to a zeroed out area of the buffer that
		// should not have been written to yet.
		if( dirPtr->d_reclen == 0 ){
			dataLimited = 1;
		}
	}else{
		printFail( testName, "Too many bytes retrieved" );
	}

	if( gotFile && dataLimited ){
		printPass( testName );
	}else{
		printFail( testName, "Either did not get the file or the data was not limited" );
	}
}

/**
 * Check basep.
 **/
void getdirentries_basep(){
	const char* testName = "getdirentries_basep";
	const char* file1Name = "getdirentries_basep/file1";
	const char* file2Name = "getdirentries_basep/file2";
	const char* file3Name = "getdirentries_basep/file3";
	const char* file4Name = "getdirentries_basep/file4";
	const char* file5Name = "getdirentries_basep/file5";
	const char* file6Name = "getdirentries_basep/file6";
	const char* file7Name = "getdirentries_basep/file7";
	const char* file8Name = "getdirentries_basep/file8";
	dirent* dirPtr = 0;
	char buf[ gBufLarge ];
	char* savedFileName = (char*)calloc( gBufLarge, sizeof( char ) );
	char* debugBuf = (char*)calloc( gBufLarge, sizeof( char ) );
	long basep = 1;
	long basep_ignore = 0;
	int theFile = 0;
	int theDir = 0;
	struct dirent* dirEntry = 0;
	unsigned long lseekRet = 0;
	
	memset( buf, 0, gBufLarge );
	
	// Make a directory with the files.
	mkdir( testName, 0777 );
	
	theFile = open( file1Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	theFile = open( file2Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	theFile = open( file3Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	theFile = open( file4Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	theFile = open( file5Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	theFile = open( file6Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	theFile = open( file7Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );

	theFile = open( file8Name, O_CREAT | O_RDWR );
	write( theFile, gData1, 5 );
	close( theFile );
	
	// Open the directory and get info. about it a few bytes at a time.
	theDir = open( testName, O_RDONLY );

	getdirentries( theDir, buf, 16, &basep );
	getdirentries( theDir, buf, 16, &basep );	
	getdirentries( theDir, buf, 16, &basep );	
	dirPtr = (dirent*)buf;

	strcpy( savedFileName, dirPtr->d_name );
	
	// Now we have the name of the file at basep.  Stop populating basep
	// and make more calls to getdirentries.
	
	getdirentries( theDir, buf, 16, &basep_ignore );
	getdirentries( theDir, buf, 16, &basep_ignore );
	getdirentries( theDir, buf, 16, &basep_ignore );
	getdirentries( theDir, buf, 16, &basep_ignore );
	getdirentries( theDir, buf, 16, &basep_ignore );

	// lseek() to the saved basep.
	// Then, a getdirentries() call should start reading at the file whose name we saved.
	memset( buf, 0, gBufLarge );
	lseek( theDir, basep, SEEK_SET );
	getdirentries( theDir, buf, 16, &basep_ignore );
	dirPtr = (dirent*)buf;

	if( strcmp( savedFileName, dirPtr->d_name ) == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "basep was not correct.  Fails due to ALC-407. (Fixed)" );
	}

	free( debugBuf );
	free( savedFileName );
}
