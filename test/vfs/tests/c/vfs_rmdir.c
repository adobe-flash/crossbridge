#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

void runRmdirTests(){
	rmdir_empty();
	rmdir_file();
	rmdir_dir();
	rmdir_fileTimes();
	rmdir_dot();
	rmdir_dotdot();
	rmdir_dotFinal(); // must fail
	rmdir_dotdotFinal(); // must fail
	rmdir_enoent();
	rmdir_enotdir();
	rmdir_rdonlyfs();
}

/**
 * Remove an empty dir.
 **/
void rmdir_empty(){
	const char* testName = "rmdir_empty";
	const char* dirName = "/rmdir_empty";
	int ret = 0;
	
	mkdir( dirName, 0777 );
	
	if( searchForDir( testName ) == 0 ){
		errno = 0;
		ret = rmdir( testName );
		if( (ret == 0) && (errno == 0) ){
			if( searchForDir( testName ) == -1 ){
				printPass( testName );
			}else{
				printFail( testName, "Failed to remove directory" );
			}
		}else{
			printFail( testName, "Expected ret = 0 and errno = 0" );
		}
	}else{
		printFail( testName, "Unable to create directory" );
	}
}

/**
 * Remove a dir containing a file.
 **/
void rmdir_file(){
	const char* testName = "rmdir_file";
	const char* dirName = "/rmdir_file";
	const char* fileName = "/rmdir_file/foo.txt";
	int ret = 0;
	int theFile = -1;
	
	mkdir( dirName, 0777 );
	
	if( searchForDir( testName ) == 0 ){
		theFile = open( fileName, O_RDWR | O_CREAT, S_IRWXU );
		write( theFile, gData1, strlen( gData1 ) );
		close( theFile );

		if( searchForFile( fileName ) == 0 ){
			errno = 0;
			ret = rmdir( testName );
			if( (ret == -1) && (errno == ENOTEMPTY) ){
				if( searchForDir( dirName ) == 0 ){
					if( searchForFile( fileName ) == 0 ){
						printPass( testName );					
					}else{
						printFail( testName, "Expected the file to still be there" );
					}
				}else{
					printFail( testName, "Expected the directory to still be there" );		
				}
			}else{
				//printf( "error: %s", strerror( errno ) );
				printFail( testName, "Expected rmdir to fail" );
			}
		}else{
			printFail( testName, "Failed to create the file" );
		}		
	}else{
		printFail( testName, "Unable to create directory" );
	}
}

/**
 * Remove a dir containing a directory.
 **/
void rmdir_dir(){
	const char* testName = "rmdir_dir";
	const char* dirName = "/rmdir_dir";
	const char* dirName2 = "/rmdir_dir/dir2";
	int ret = 0;
	int theFile = -1;
	
	mkdir( dirName, 0777 );
	mkdir( dirName2, 0777 );
	
	if( searchForDir( dirName ) == 0 ){
		if( searchForDir( dirName2 ) == 0 ){
			errno = 0;
			ret = rmdir( dirName );
			if( (ret == -1) && (errno == ENOTEMPTY) ){
				if( searchForDir( dirName ) == 0 ){
					if( searchForDir( dirName2 ) == 0 ){
						printPass( testName );					
					}else{
						printFail( testName, "Expected the subdirectory to still be there" );
					}
				}else{
					printFail( testName, "Expected the directory to still be there" );		
				}
			}else{
				//printf( "error: %s", strerror( errno ) );
				printFail( testName, "Expected rmdir to fail" );
			}
		}else{
			printFail( testName, "Failed to create the subdirectory." );
		}		
	}else{
		printFail( testName, "Failed to create the directory" );
	}
}


/**
 * Remove a dir should update the parent's
 * file times.
 **/
void rmdir_fileTimes(){
	const char* testName = "rmdir_fileTimes";
	printFail( testName, "Fails due to ALC-358" );
}

/**
 * Put a . in the path.
 **/
void rmdir_dot(){
	const char* testName = "rmdir_dot";
	
	mkdir( testName, 0777 );
	errno = 0;
	int ret = rmdir( "./rmdir_dot" );
	
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( testName ) == -1 ){
			printPass( testName );
		}else{
			printFail( testName, "The directory should have been deleted" );
		}
	}else{
		printFail( testName, "Call to rmdir failed" );
	}
}

/**
 * Put a .. in the path.
 **/
void rmdir_dotdot(){
	const char* testName = "rmdir_dotdot";
	const char* dir1 = "rmdir_dotdot/dir1";
	const char* dir2 = "rmdir_dotdot/dir2";
	
	mkdir( testName, 0777 );
	mkdir( dir1, 0777 );
	mkdir( dir2, 0777 );
	errno = 0;
	int ret = rmdir( "rmdir_dotdot/dir1/../dir2" );
	
	if( (errno == 0) && (ret == 0) ){
		if( searchForDir( dir2 ) == -1 ){
			printPass( testName );
		}else{
			printFail( testName, "The directory dir2 should have been deleted" );
		}
	}else{
		printFail( testName, "Call to rmdir failed" );
	}
}

/**
 * Have the path end in .  This must fail.
 **/
void rmdir_dotFinal(){
	const char* testName = "rmdir_dotFinal";
	
	mkdir( testName, 0777 );
	errno = 0;
	int ret = rmdir( "rmdir_dotFinal/." );

	if( (errno == EINVAL ) && (ret == -1) ){
		if( searchForDir( testName ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "The directory should not have been removed" );
		}
	}else{
		printFail( testName, "The call to rmdir should have failed.  Fails due to ALC-399. (Fixed)" );
	}
}

/**
 * Have the path end in ..  This must fail.
 **/
void rmdir_dotdotFinal(){
	const char* testName = "rmdir_dotdotFinal";
	const char* dir1Name = "rmdir_dotdotFinal";
	const char* dir2Name = "rmdir_dotdotFinal/dir1";
	
	mkdir( dir1Name, 0777 );
	mkdir( dir2Name, 0777 );
	errno = 0;
	int ret = rmdir( "rmdir_dotdotFinal/dir1/.." );
	
	if( (errno == ENOTEMPTY) && (ret == -1) ){
		if( (searchForDir( dir1Name ) == 0) && (searchForDir( dir2Name ) == 0) ){
			printPass( testName );
		}else{
			printFail( testName, "Neither directory should have been deleted" );
		}
	}else{
		printFail( testName, "Call to rmdir should have failed" );
	}
}

/**
 * Part of the path is missing.
 **/
void rmdir_enoent(){
	const char* testName = "rmdir_enoent";
	const char* dirName1 = "/rmdir_enoent";
	const char* dirName2 = "/rmdir_enoent_bogus";
	int ret = 0;
	
	mkdir( dirName1, 0777 );
	
	if( searchForDir( dirName1 ) == 0 ){
		errno = 0;
		ret = rmdir( dirName2 );
		if( (ret == -1) && (errno == ENOENT) ){
			printPass( testName );
		}else{
			printFail( testName, "Expected ret = 0 and errno = 0" );
		}
	}else{
		printFail( testName, "Unable to create directory" );
	}
}

/**
 * Part of the path is a file instead of a dir.
 **/
void rmdir_enotdir(){
	const char* testName = "rmdir_enotdir";
	
	mkdir( testName, 0777 );

	// Create a file.
	int theFile = open( "rmdir_enotdir/foo", O_RDWR | O_CREAT );
	close( theFile );
	
	errno = 0;
	int ret = rmdir( "rmdir_enotdir/foo" );
	
	if( (ret == -1) && (errno == ENOTDIR) ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ret == -1 and errno == ENOTDIR");
	}
}

/**
 * Try to remove a directory from the read only file system. 
 **/
void rmdir_rdonlyfs(){
	const char* testName = "rmdir_rdonlyfs";
	const char* expectedError = "Read-only file system";	
	char* error = 0;	
	struct stat statbuf;
	
	errno = 0;
	int ret = rmdir( "compressed_readonly/dir" );
	int savedErrno = errno;
	
	if( ret != -1 ){
		printFail( testName, "Should not have been able to call rmdir on the read only file system." );
		return;
	}
	
	if( savedErrno != 30 || strcmp( error = strerror( savedErrno ), expectedError ) != 0 ){
		return;
	}

	// Be sure the directory is still there.
	if( stat( "compressed_readonly/dir", &statbuf ) != 0 ){
		printFail( testName, "The directory is gone." );
		return;
	}
	
	printPass( testName );
	return;
}