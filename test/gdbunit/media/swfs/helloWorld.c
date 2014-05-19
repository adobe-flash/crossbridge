// A simple hello world application for simple debugging.

#include <stdio.h>
#include <string.h>

int square(int a);

int a = 5;
int *aPointer = &a;
char letters[5] = {'A', 'B', 'C', 'D', 'E'};
int numbers[5] = {1, 2, 3, 4, 5};

int square(int a)
{
    return a * a;
}

int main (int argc, const char * argv[])
{
    int s = 2;
    int t = 1;
    
    int i;
    for (i = 0; i < 8; i++){
        t *= 2;
    }

    printf("Hello, World!\n");
    printf("%d*%d=%d", s, s, square(s));
    return 0;
}

