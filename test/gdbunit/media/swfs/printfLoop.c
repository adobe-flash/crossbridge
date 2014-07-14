#include <stdio.h>
#include <string.h>

int main (int argc, const char * argv[])
{
    int i;
    printf("Start\n");
    
    for (i = 0; i < 20; i++){
        printf("  i = %d\n", i);
    }
    
    printf("Done\n");
    return 0;
}

