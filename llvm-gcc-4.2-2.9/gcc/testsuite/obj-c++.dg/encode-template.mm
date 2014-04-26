/* APPLE LOCAL file radar 5619052 */
/* Test for use of template param in objective-c++'s @encode expression. */
#include <stdlib.h>
#include <string.h>
/* { dg-do run } */

template<typename S> const char *swap_struct(S *s)
{
    const char *encoding = @encode(S);
    return encoding;
}


struct SIZE {
        char ch[124];
} ps;

struct SIZE1 {
        struct SIZE s;
        double d;
} ps1;

int *pi;

int main()
{
	if (strcmp(swap_struct(&ps), @encode(SIZE)))
	  abort();
	if (strcmp(swap_struct(&ps1), @encode(SIZE1)))
	  abort();
	if (strcmp(swap_struct(pi), @encode(int)))
	  abort();
	return 0;
}
