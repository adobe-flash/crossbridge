#include <iostream>
#include <fstream>
using namespace std;

#include <errno.h>
#include <fcntl.h>

#include <fstream>
#include <iostream>


#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../framework/vfs.h"

using namespace std;

void runIfstreamTests(){
	ifstream_basic();
}

/**
 * Basic use of ifstream to read from a file.
 **/
void ifstream_basic(){
	const char* testName = "ifstream_basic";
	FILE* pFile = 0;
	char bytesRead[2];
	memset( bytesRead, 0, 2 );
	
	// Write a file.
	pFile = fopen( testName, "w+" );
	fwrite( gData1, sizeof( char ), strlen( gData1 ), pFile );
	fclose( pFile );

	// Read the file.
	ifstream ifs( testName, ifstream::in );
	ifs >> bytesRead;
	ifs.close();
	
	if( strcmp( bytesRead, gData1 ) == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Data was not correct" );
	}
}
