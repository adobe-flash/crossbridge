/* APPLE LOCAL begin radar 4204303 */
#include <stdio.h>
#include <string.h>
extern void abort();

void* mysym(void* handle, const char* symbolName)
{
	char underscoredName[strlen(symbolName)+2];
	underscoredName[0] = '_';
	strcpy(&underscoredName[1], symbolName);
		
	if ( handle == (void*)-1 ) {
		void* callerAddress = __builtin_return_address(1); 
		return callerAddress;
	}
	return NULL;
}

int main (int argc, const char * argv[]) {
    void* y = mysym((void*)-1, "CFURLCreateFromFileSystemRepresentation");
    void* current = __builtin_return_address(0);
    if (current != y)
      abort();
    return 0;
}
/* APPLE LOCAL end radar 4204303 */
