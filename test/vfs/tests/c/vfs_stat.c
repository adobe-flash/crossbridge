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

void runStatTests(){
	stat_file();
	stat_largeFile();
	stat_dir();
	stat_enoent();
	stat_enotdir();	
}

/**
 * Call stat() on a file.  We do this in lots
 * of places but let's have a defined place for it.
 * Note that whether we support st_dev and st_ino is uncertain.
 **/
void stat_file(){
	const char* testName = "stat_file";
	char aBlock[ gBufBlockSize ];
	struct stat statbuf;
	int ret = 0;
	
	// Write 4095 a's followed by a null.
	memset( aBlock, 'a', gBufBlockSize - 1 );
	aBlock[ gBufBlockSize - 1 ] = 0;
	
	int theFile = open( testName, O_CREAT | O_RDWR );
	write( theFile, aBlock, gBufBlockSize );
	close( theFile );

	errno = 0;
	ret = stat( testName, &statbuf );

	if( (errno == 0) && (ret == 0) ){
		if( statbuf.st_mode == S_IFREG ){
			if( statbuf.st_nlink == 1 ){
				if( statbuf.st_blksize == gBufBlockSize ){
					if( statbuf.st_blocks == 1 ){
						if( statbuf.st_size == gBufBlockSize ){
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
 * Call stat() on a file that's larger than a block.
 **/
void stat_largeFile(){
	const char* testName = "stat_largeFile";
	char aBlock[ gBufBlockSize + 1 ];
	struct stat statbuf;
	int ret = 0;
	
	// Write one more byte than a block.
	memset( aBlock, 'a', gBufBlockSize );
	aBlock[ gBufBlockSize ] = 0;
	
	int theFile = open( testName, O_CREAT | O_RDWR );
	write( theFile, aBlock, gBufBlockSize + 1 );
	close( theFile );
	
	errno = 0;
	ret = stat( testName, &statbuf );
	
	if( (errno == 0) && (ret == 0) ){
		if( statbuf.st_mode == S_IFREG ){
			if( statbuf.st_nlink == 1 ){
				if( statbuf.st_blksize == gBufBlockSize ){
					if( statbuf.st_blocks == 2 ){
						if( statbuf.st_size == gBufBlockSize + 1 ){
							printPass( testName );
						}else{
							printFail( testName, "Expected size = 4097" );
						}							
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
}

/**
 * Call stat() on a dir.
 **/
void stat_dir(){
	const char* testName = "stat_dir";
	const char* dirName = "/stat_dir";
	struct stat statbuf;
	int ret = 0;
	
	mkdir( dirName, 0777 );
	
	errno = 0;
	ret = stat( testName, &statbuf );
	
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
		printFail( testName, "Expected errno == 0 and ret == 0" );
	}
}

/**
 * Trigger enoent.
 **/
void stat_enoent(){
	const char* testName = "stat_enoent";
	struct stat statbuf;
	int ret = -1;
	
	errno = 0;
	ret = stat( "stat_enoent_does_not_exist", &statbuf );
	
	if( (errno == ENOENT) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ENOENT" );
	}
}

/**
 * Trigger enotdir
 **/
void stat_enotdir(){
	const char* testName = "stat_enotdir";
	const char* dir1Name = "/stat_enotdir";
	const char* fileName = "/stat_enotdir/file";
	const char* dir2Name = "/stat_enotdir/file/subdir";
	struct stat statbuf;

	mkdir( dir1Name, 0777 );

	int theFile = open( fileName, O_RDWR | O_CREAT );
	close( theFile );

	errno = 0;
	int ret = stat( dir2Name, &statbuf );
		
	if( (errno == ENOTDIR) && (ret == -1) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ENOTDIR and -1.  Fails due to ALC-410. (Fixed)" );
	}
}