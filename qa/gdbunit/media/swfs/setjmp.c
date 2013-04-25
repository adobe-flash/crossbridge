#include <stdio.h>
#include <setjmp.h>

int
main()
{
    jmp_buf env;
    int i = 0;
    int j = 0;
    int k;

    printf("gonna call setjmp\n");
    k = setjmp(env);
    printf("setjmp returned %d\n", k);

    if (j)
        return 0;
    if (i)
        j = 1;

    for (i = 0; i < 10; i++)
        ;
    printf("i = %d\n", i);
#ifdef DOJMP
    longjmp(env, i);
#endif

    // use a goto loop
    printf("goto loop:");
    int m = 0;   
    loop:
        printf("%d, ", m);
        m++;
        if (m<3) 
            goto loop;
    printf("DONE");
    
    return 1;
}

