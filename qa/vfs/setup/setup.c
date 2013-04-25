#include "AS3/AS3.h"
#include "setup.h"
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;

const int gBigEnough = 10000;

void copyFile( const char*, const char* );

/**
 * This app does setup work for VFS testing.
 * Currently that only involves creating the 
 * files in the Flash LSO storage that the 
 * LSO tests will need to read.
 **/
int main(){
	setup();
	inline_as3(
	 "import flash.system.System;\n"
	 "System.exit( 0 );\n"
	 );	
	return 0;
}

void setup(){
	mkdir( "lso/dir", 0777 );
	mkdir( "lso/.dotdir", 0777 );
	mkdir( "lso/..dotdotdir", 0777 );
	
	
	copyFile( "compressed/ space2", "lso/ space2" );	
	copyFile( "compressed/'squote'", "lso/'squote'" );
	copyFile( "compressed/`grave", "lso/`grave" );
	copyFile( "compressed/basic", "lso/basic" );
	copyFile( "compressed/dir/file_in_dir.txt", "lso/dir/file_in_dir.txt" );
	copyFile( "compressed/empty", "lso/empty" );
	copyFile( "compressed/spa ce", "lso/spa ce" );
	copyFile( "compressed/.dot", "lso/.dot" );
	copyFile( "compressed/..dotdot", "lso/..dotdot" );
	copyFile( "compressed/.dotdir/file.txt", "lso/.dotdir/file.txt" );
	copyFile( "compressed/..dotdotdir/file.txt", "lso/..dotdotdir/file.txt" );

	mkdir( "lso/startchars0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234end", 0777 );
	copyFile( "compressed/startchars012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567end.txt", "lso/startchars012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567end.txt" );
	copyFile( "compressed/startchars0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234end/thefile.txt", "lso/startchars0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234end/thefile.txt" );

	copyFile( "compressed/testgif.gif", "lso/testgif.gif" );
	copyFile( "compressed/testjson.json", "lso/testjson.json" );
	copyFile( "compressed/testpng.png", "lso/testpng.png" );
	copyFile( "compressed/testxml.xml", "lso/testxml.xml" );
	copyFile( "compressed/writeToMe", "lso/writeToMe" );

}

/**
 * Copies a file.  e.g. We use this to copy a file
 * from the compressed file system to the LSO file
 * system.
 **/
void copyFile( const char* from, const char* to ){
	uint8_t buffer[ gBigEnough ];
	int fromFile, toFile = -1;
	int bytesRead = -1;
	int bytesWritten = -1;
	
	memset( buffer, 0, gBigEnough );

	// Open, read, and close the from file.
	errno = 0;
	fromFile = open( from, O_RDONLY );
	if( fromFile < 0 ){
		printf( "copyFile error opening %s: %s\n", from, strerror( errno ) );
	}else{
		errno = 0;
		bytesRead = read( fromFile, buffer, gBigEnough );
		if( bytesRead < 0 ){
			printf( "copyFile error reading %s: %s\n", from, strerror( errno ) );	
		}
	}
	
	// Create and write to the new file.
	toFile = open( to, O_RDWR | O_CREAT | O_TRUNC, 0777 );
	if( toFile < 0 ){
		printf( "copyFile error creating %s: %s\n", to, strerror( errno ) );
	}else{
		errno = 0;
		bytesWritten = write( toFile, buffer, bytesRead );
		if( bytesWritten != bytesRead ){
			printf( "copyFile error writing %s: %s\n", to, strerror( errno ) );
		}
	}
	
	close( fromFile );
	close( toFile );
}