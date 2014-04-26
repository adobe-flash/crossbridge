/* APPLE LOCAL file radar 4995967 */
/* Test out '@catch(...) {...}' in 32bit mode, which should catch
   all uncaught exceptions.  */

/* { dg-options "-fobjc-exceptions" } */
/* { dg-require-effective-target ilp32 } */
/* { dg-do run } */

extern "C" void abort (void);

#include "../objc/execute/Object2.h"
#include <stdio.h>

/* The following is not required in actual user code; we include it
   here to check that the compiler generates an internal definition of
   _setjmp that is consistent with what <setjmp.h> provides.  */
#include <setjmp.h>

#define CHECK_IF(expr) if(!(expr)) abort()

@interface Frob: Object
@end

@implementation Frob: Object
@end

static Frob* _connection = nil;

//--------------------------------------------------------------------


void test (Object* sendPort)
{
	int cleanupPorts = 1;
	Frob* receivePort = nil;
	
	@try {
		printf ("receivePort = %p\n", receivePort);
		printf ("sendPort = %p\n", sendPort);
		printf ("cleanupPorts = %d\n", cleanupPorts);
		printf ("---\n");
		
		receivePort = (Frob *) -1;
		_connection = (Frob *) -1;
		printf ("receivePort = %p\n", receivePort);
		printf ("sendPort = %p\n", sendPort);
		printf ("cleanupPorts = %d\n", cleanupPorts);
		printf ("---\n");
		
		receivePort = nil;
		sendPort = nil;
		cleanupPorts = 0;
		
		printf ("receivePort = %p\n", receivePort);
		printf ("sendPort = %p\n", sendPort);
		printf ("cleanupPorts = %d\n", cleanupPorts);
		printf ("---\n");		
		
		@throw [Object new];
	}
	@catch(Frob *obj) {
		printf ("Exception caught by incorrect handler!\n");
		CHECK_IF(0);
	}
	@catch(...) {
		printf ("Exception caught by correct handler.\n");
		printf ("receivePort = %p (expected 0x0)\n", receivePort);
		printf ("sendPort = %p (expected 0x0)\n", sendPort);
		printf ("cleanupPorts = %d (expected 0)\n", cleanupPorts);
		printf ("---");
		CHECK_IF(!receivePort);
		CHECK_IF(!sendPort);
		CHECK_IF(!cleanupPorts);
	}
}

int main (void) {

	test((Object *)-1);
	return 0;
}
