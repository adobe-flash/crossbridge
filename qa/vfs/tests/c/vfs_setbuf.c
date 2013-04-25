#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../framework/vfs.h"

/**f
 * NOTE: Test lightly.
 * Also note that this returns nothing and does not set errno.
 **/

void runSetbufTests(){
	setbuf_basic();
}

void setbuf_basic(){
	const char* testName = "setbuf_basic";
	const char* expectedModifiedBuffer = "0bcde";
	char bytesRead[ BUFSIZ ];
	char buf[ BUFSIZ ];
	memset( buf, 0, BUFSIZ );
	memset( bytesRead, 0, BUFSIZ );
	int theFile = -1;
	FILE* pFile = fopen( testName, "w+" );
	setbuf( pFile, buf );
	
	fputs( gData1, pFile );

	// Now there should be something in the buffer.
	if( strcmp( buf, gData1 ) == 0 ){
		// The file should still be empty.
		theFile = open( testName, O_RDONLY );
		read( theFile, bytesRead, strlen( gData1 ) );
		close( theFile );
		if( strlen( bytesRead ) == 0 ){
			// Flush and check again.  The file and the buffer will contain abcde.
			fflush( pFile );
			fputc( gData2[ 0 ], pFile );
			
			// Now the buffer should contain 0bcde.
			if( strcmp( buf, expectedModifiedBuffer ) == 0 ){
				// The file should contain gData1 now.
				theFile = open( testName, O_RDONLY );
				memset( bytesRead, 0, BUFSIZ );
				read( theFile, bytesRead, BUFSIZ );
				close( theFile );
				
				if( strcmp( bytesRead, gData1 ) == 0 ){
					printPass( testName );
				}else{
					printFail( testName, "The file should have contained gData1's data" );
				}
			}else{
				printFail( testName, "The modified buffer was not correct" );
				//printf( "buffer had \'%s\'\n", buf );
			}
		}else{
			printFail( testName, "The file should still be empty" );
		}
	}else{
		printFail( testName, "The buffer data was not correct" );
	}
}


