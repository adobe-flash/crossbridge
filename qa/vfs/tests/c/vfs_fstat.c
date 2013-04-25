#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * NOTE: We support:
 *	- st_mode
 *	- st_size
 *	- st_blksize
 *	- st_blocks
 **/

void runFstatTests(){
	fstat_file();
	fstat_largeFile();
	fstat_dir();
	fstat_ebadf();
}

/**
 * Call fstat() on a file.
 **/
void fstat_file(){
	const char* testName = "fstat_file";
	char aBlock[ gBufBlockSize ];
	struct stat statbuf;
	int ret = 0;
	
	// Write 4095 a's followed by a null.
	memset( aBlock, 'a', gBufBlockSize - 1 );
	aBlock[ gBufBlockSize - 1 ] = 0;
	
	int theFile = open( testName, O_CREAT | O_RDWR );
	write( theFile, aBlock, gBufBlockSize );
	
	errno = 0;
	ret = fstat( theFile, &statbuf );
	
	if( (errno == 0) && (ret == 0) ){
		if( statbuf.st_mode == S_IFREG ){
			if( statbuf.st_nlink == 1 ){
				if( statbuf.st_blksize == gBufBlockSize ){
					if( statbuf.st_blocks == 1 ){
						printPass( testName );
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
	
	close( theFile );	
}

/**
 * Call fstat() on a file that's larger than a block.
 **/
void fstat_largeFile(){
	const char* testName = "fstat_largeFile";
	char aBlock[ gBufBlockSize + 1 ];
	struct stat statbuf;
	int ret = 0;
	
	// Write one more byte than a block.
	memset( aBlock, 'a', gBufBlockSize );
	aBlock[ gBufBlockSize ] = 0;
	
	int theFile = open( testName, O_CREAT | O_RDWR );
	write( theFile, aBlock, gBufBlockSize + 1 );
	
	errno = 0;
	ret = fstat( theFile, &statbuf );
	
	if( (errno == 0) && (ret == 0) ){
		if( statbuf.st_mode == S_IFREG ){
			if( statbuf.st_nlink == 1 ){
				if( statbuf.st_blksize == gBufBlockSize ){
					if( statbuf.st_blocks == 2 ){
						printPass( testName );
					}else{
						printFail( testName, "Expected st_blocks = 2" );
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
	
	close( theFile );	
}

/**
 * Call fstat() on a dir.
 **/
void fstat_dir(){
	const char* testName = "fstat_dir";
	const char* dirName = "/fstat_dir";
	struct stat statbuf;
	int ret = 0;
	
	mkdir( dirName, 0777 );
	int theDir = open( dirName, O_RDONLY );
	
	errno = 0;
	ret = fstat( theDir, &statbuf );
	
	if( (errno == 0) && (ret == 0) ){
		if( statbuf.st_mode == S_IFDIR ){
			if( statbuf.st_nlink == 1 ){
				if( statbuf.st_blksize == 4096 ){
					if( statbuf.st_blocks == 0 ){
						printPass( testName );
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
		printFail( testName, "Expected errno == 0 and ret == 0" );
	}
	
	close( theDir );
}

/**
 * Trigger ebadf
 **/
void fstat_ebadf(){
	const char* testName = "stat_ebadf";
	struct stat statbuf;
	int ret = -1;
	
	errno = 0;
	ret = fstat( -1, &statbuf );
	
	if( (errno == EBADF) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected EBADF" );
	}
}
