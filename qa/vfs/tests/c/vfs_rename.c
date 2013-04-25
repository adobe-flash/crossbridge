#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**
 * This is now implemented (ALC-386).
 **/
void runRenameTests(){
	rename_file();
	rename_dir();
	rename_populatedDir();
	rename_dirExists();
	rename_dirExistsAndPopulated();
	rename_fileToDir();
	rename_dirToFile();
	rename_moveFile();
	rename_replaceFile();
	rename_same();
	rename_emptyString();
	rename_noExist();
	rename_enotdir();
	rename_rdonlyfs();
}

/**
 * Rename a file.
 **/
void rename_file(){
	const char* testName = "rename_file";
	const char* newName = "rename_file_2";
	
	int theFile = open( testName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );

	errno = 0;
	int ret = rename( testName, newName );

	if( errno == 0 && ret == 0 ){
		verifyFileContents( testName, newName, gData1 );
	}else{
		printFail( testName, "Incorrect return value and/or errno." );
	}
}

/**
 * Rename a directory.
 **/
void rename_dir(){
	const char* testName = "rename_dir";
	const char* newName = "rename_dir_2";

	mkdir( testName, 0777 );
	
	errno = 0;
	int ret = rename( testName, newName );

	if( searchForDir( newName ) == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Did not find the directory." );
	}
}

/**
 * Rename a directory which is not empty.
 **/
void rename_populatedDir(){
	const char* testName = "rename_populatedDir";
	const char* fileName = "rename_populatedDir/foo.txt";
	const char* newName = "rename_populatedDir_2";
	const char* newFileName = "rename_populatedDir_2/foo.txt";
	
	mkdir( testName, 0777 );
	
	int theFile = open( fileName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	errno = 0;
	int ret = rename( testName, newName );
	
	if( searchForDir( newName ) == 0 ){
		if( searchForFile( newFileName ) == 0 ){
			printPass( testName );
		}else{
			printFail( testName, "Did not find the file." );
		}
	}else{
		printFail( testName, "Did not find the directory." );
	}	
}

/**
 * The new name is a directory which already exists, and is empty.
 **/
void rename_dirExists(){
	const char* testName = "rename_dirExists";
	const char* dirEmpty = "rename_dirExists_empty";
	const char* dirPopulated = "rename_dirExists_populated";	
	const char* fileName = "rename_dirExists_populated/foo.txt";
	const char* newFileName = "rename_dirExists_empty/foo.txt";

	mkdir( dirEmpty, 0777 );
	mkdir( dirPopulated, 0777 );

	int theFile = open( fileName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );

	errno = 0;
	int ret = rename( dirPopulated, dirEmpty );

	if( ret == 0 && errno == 0 ){
		if( searchForDir( dirPopulated ) == -1 ){
			if( searchForFile( newFileName ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "Could not find the moved file." );
			}
		}else{
			printFail( testName, "Expected dirPopulated to be gone because it was renamed." );
		}
	}else{
		printFail( testName, "Expected ret and errno to be 0." );
	}	
}

/**
 * The new name is a directory which already exists, and is populated.  Triggers EEXIST or ENOTEMPTY.
 **/
void rename_dirExistsAndPopulated(){
	const char* testName = "rename_dirExistsAndPopulated";
	const char* dirPopulated1 = "rename_dirExistsAndPopulated1";
	const char* fileName = "rename_dirExistsAndPopulated1/foo.txt";
	const char* dirPopulated2 = "rename_dirExistsAndPopulated2";	

	mkdir( dirPopulated1, 0777 );
	mkdir( dirPopulated2, 0777 );
	
	int theFile = open( fileName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	errno = 0;
	int ret = rename( dirPopulated2, dirPopulated1 );
	
	if( ret == -1 && ( errno == EEXIST || errno == ENOTEMPTY ) ){
		if( searchForDir( dirPopulated1 ) == 0 && 
			searchForDir( dirPopulated2 ) == 0 && 
			searchForFile( fileName ) == 0 ){
		printPass( testName );
		}else{
			printFail( testName, "Should have triggered an error and nothing should have changed" );
		}
	}
}

/**
 * Rename file to existing directory (should fail).
 * e.g. 
 /dir1/
 /dir2/
 /dir2/foo.txt
 Renaming foo.txt to "dir1" should fail. (This triggers EISDIR.)
 **/
void rename_fileToDir(){
	const char* testName = "rename_fileToDir";
	const char* dirName = "rename_fileToDir/dir1";
	const char* fileName = "rename_fileToDir/foo.txt";
	
	mkdir( testName, 0777 );
	mkdir( dirName, 0777 );
	
	int theFile = open( fileName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	errno = 0;
	int ret = rename( fileName, dirName );
	
	if( ret == -1 && errno == EISDIR ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ret == -1 and errno == EISDIR" );
	}
}

/**
 * Like the above, but reversed.  Should fail.  This is one way to trigger ENOTDIR.
 **/
void rename_dirToFile(){
	const char* testName = "rename_fileToDir";
	const char* dirName = "rename_fileToDir/dir1";
	const char* fileName = "rename_fileToDir/foo.txt";
	
	mkdir( testName, 0777 );
	mkdir( dirName, 0777 );
	
	int theFile = open( fileName, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	errno = 0;
	int ret = rename( dirName, fileName );
	
	if( ret == -1 && errno == ENOTDIR ){
		printPass( testName );
	}else{
		printFail( testName, "Expected ret == -1 and errno == ENOTDIR" );
	}
}

/**
 * Move a file to another directory.
 **/
void rename_moveFile(){
	const char* testName = "rename_moveFile";
	const char* dir1 = "rename_moveFile/dir1";
	const char* dir2 = "rename_moveFile/dir2";
	const char* file1 = "rename_moveFile/dir1/foo.txt";
	const char* file2 = "rename_moveFile/dir2/foo.txt";
	
	mkdir( testName, 0777 );
	mkdir( dir1, 0777 );
	mkdir( dir2, 0777 );
	
	// Write file1 to dir1.
	int theFile = open( file1, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Move file1 to dir2.
	errno = 0;
	int ret = rename( file1, file2 );
	
	if( ret == 0 && errno == 0 ){
		if( searchForFile( file1 ) == -1 ){
			if( searchForFile( file2 ) == 0 ){
				printPass( testName );
			}else{
				printFail( testName, "Did not find file2" );
			}
		}else{
			printFail( testName, "Expected file1 to be gone" );
		}
	}else{
		//printf( "test %s: ret=%d, errno=%s\n", testName, ret, strerror(errno) );
		printFail( testName, "Expected ret == 0 and errno == 0" );
	}	
}

/**
 * Replace an existing file.
 **/
void rename_replaceFile(){
	const char* testName = "rename_replaceFile";
	const char* dir = "rename_replaceFile";
	const char* fileOfLetters = "rename_replaceFile/fileOfLetters.txt";
	const char* fileOfNumbers = "rename_replaceFile/fileOfNumbers.txt";
	
	mkdir( dir, 0777 );
	
	// Create a file of letters.
	int theFile = open( fileOfLetters, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );

	// Create a file of numbers.
	theFile = open( fileOfNumbers, O_RDWR | O_CREAT );
	write( theFile, gData2, strlen( gData2 ) );
	close( theFile );

	// Rename the Numbers file to the Letters file. This will result in a new
	// file called fileOfLetters which contains numbers.  This will also 
	// result in there being no file called fileOfNumbers.
	errno = 0;
	int ret = rename( fileOfNumbers, fileOfLetters );
	
	if( ret == 0 && errno == 0 ){
		if( searchForFile( fileOfNumbers ) == -1 ){
			if( searchForFile( fileOfLetters ) == 0 ){
				verifyFileContents( testName, fileOfLetters, gData2 );
			}else{
				printFail( testName, "Did not find fileOfLetters" );
			}
		}else{
			printFail( testName, "Expected fileOfNumbers to be gone" );
		}
	}else{
		printFail( testName, "Expected ret == 0 and errno == 0" );
	}
}

/**
 * Rename a file with the exact same path/file name. (This triggers EINVAL.)
 **/
void rename_same(){
	const char* testName = "rename_same";
	const char* dir = "rename_same";
	const char* file1 = "rename_same/foo.txt";
	const char* file2 = "rename_same/foo.txt";
	
	mkdir( dir, 0777 );
	
	// Write file1 to dir.
	int theFile = open( file1, O_RDWR | O_CREAT );
	write( theFile, gData1, strlen( gData1 ) );
	close( theFile );
	
	// Move file1 to file2.
	errno = 0;
	int ret = rename( file1, file2 );
	
	if( ret == -1 && errno == EINVAL ){
		printPass( testName );
	}else{
		printFail( testName, "Expected return val of -1 and errno == EINVAL" );
	}
}

/**
 * Rename an empty string.  Triggers ENOENT.
 **/
void rename_emptyString(){
	const char* testName = "rename_emptyString";
	const char* emptyString = "";
	const char* fileName = "foo.txt";
	int savedErrno = -1;

	// Write a file.
	int theFile = open( fileName, O_RDWR | O_CREAT );
	close( theFile );
	
	errno = 0;
	int ret = rename( emptyString, emptyString );
	savedErrno = errno;
	if( ret == -1 && savedErrno == ENOENT ){
		errno = 0;
		ret = rename( emptyString, fileName );
		if( ret == -1 && errno == ENOENT ){
			errno = 0;
			ret = rename( fileName, emptyString );
			if( ret == -1 && errno == ENOENT ){
				printPass( testName );
			}else{
				//printf( "ret=%d, error=%s,\n", ret, strerror(savedErrno) );
				printFail( testName, "Third try failed" );
			}
		}else{
			//printf( "ret=%d, error=%s,\n", ret, strerror(savedErrno) );
			printFail( testName, "Second try failed" );
		}
	}else{
		//printf( "ret=%d, error=%s,\n", ret, strerror(savedErrno) );
		printFail( testName, "First try failed.  Fails due to ALC-442. (Fixed)" );
	}
}

/**
 * Rename a nonexistant file.  This is another way to trigger ENOENT.
 **/
void rename_noExist(){
	const char* testName = "rename_noExist";
	const char* file1 = "rename_noExist_IAmTheVeryModelOfAModernMajorGeneral.txt";
	const char* file2 = "rename_noExist_IveInformationVegetableAnimalAndMineral.txt";
	
	errno = 0;
	int ret = rename( file1, file2 );
	
	if( ret == -1 && errno == ENOENT ){
		printPass( testName );
	}else{
		printFail( testName, "Expected return val of -1 and errno == ENOENT" );
	}
}

/**
 * Trigger ENOTDIR by having a path element be a file.
 **/
void rename_enotdir(){
	const char* testName = "rename_enotdir";
	const char* dir1 = "/rename_enotdir";
	const char* file1 = "/rename_enotdir/file1";
	const char* file2 = "/rename_enotdir/file1/file2";
	int theFile = -1;
	
	mkdir( dir1, 0777 );
	theFile = open( file1, O_RDWR | O_CREAT, S_IRWXU );
	close( theFile );
	
	errno = 0;
	int ret = rename( file1, file2 );
	
	if( ret == -1 && errno == ENOTDIR ){
		printPass( testName );
	}else{
		//printf( "test %s: ret=%d, err=%s\n", testName, ret, strerror(errno) );
		printFail( testName, "Expected ret == -1 and errno == ENOTDIR." );
	}
}

/**
 * Try to call rename on the read only file system.
 **/
void rename_rdonlyfs(){
	const char* testName = "rename_rdonlyfs";
	const char* expectedError = "Read-only file system";	
	char* error = 0;	
	struct stat statbuf;
	
	errno = 0;
	int ret = rename( "compressed_readonly/basic", "compressed_readonly/basic2" );
	int savedErrno = errno;
	
	if( ret != -1 ){
		printFail( testName, "Should not have been able to call rename on the read only file system." );
		return;
	}
	
	if( savedErrno != 30 || strcmp( error = strerror( savedErrno ), expectedError ) != 0 ){
		printFail( testName, "The error was not correct." );
		return;
	}
	
	// Be sure the file is still there.
	if( stat( "compressed_readonly/basic", &statbuf ) != 0 ){
		printFail( testName, "The file is gone." );
		return;
	}
	
	printPass( testName );
	return;
}
