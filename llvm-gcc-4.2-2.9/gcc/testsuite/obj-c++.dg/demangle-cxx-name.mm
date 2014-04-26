/* APPLE LOCAL file radar 4638467 */
/* dg-do run */

#include <string.h>
extern "C" void abort ();

class foo {
public:
    static void bar(int a, char b) 
	{ 
	  if (strcmp (__PRETTY_FUNCTION__, "static void foo::bar(int, char)"))
	    abort ();
	}
};
int main() {
    foo::bar(1, 'c');
    return 0;
}
