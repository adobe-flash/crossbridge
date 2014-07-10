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

void runOfstreamTests(){
	ofstream_basic();
}

/**
 * Basic use of ofstream to write to a file.
 **/
void ofstream_basic(){
	const char* testName = "ofstream_basic";
	FILE* pFile = 0;
	char bytesRead[ gBufSmall ];
	
	// Write to the file.
	ofstream ofs( testName, ofstream::out );
	ofs << gData1;
	ofs.close();
	
	// See what ended up in the file.
	pFile = fopen( testName, "r" );
	memset( bytesRead, 0, gBufSmall );
	fread( bytesRead, sizeof( char ), gBufSmall, pFile );
	fclose( pFile );
	
	if( strcmp( bytesRead, gData1 ) == 0 ){
		printPass( testName );
	}else{
		printFail( testName, "Data was not correct" );
	}
}
