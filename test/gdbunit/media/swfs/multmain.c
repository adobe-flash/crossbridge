#include <stdio.h>

void foo();
void bar();

void
baz()
{
    printf("baz\n");
}

int
main()
{
    foo();
    bar();
    baz();
    foo();
    bar();
    return 0;
}

