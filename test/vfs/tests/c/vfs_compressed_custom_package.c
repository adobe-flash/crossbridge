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

void runCompressedCustomPackageTests(){
	compressed_custom_package_basic();
}

/**
 * Simple file.  Just verify that a custom package name works.
 **/
void compressed_custom_package_basic(){
	const char* testName = "compressed_custom_package_basic";
	const char* fileName = "/compressed_custompackage/compressed/basic";
	
	verifyFileContents( testName, fileName, gData1 );
}