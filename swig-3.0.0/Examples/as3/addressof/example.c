#include <stdio.h>
#include "example.h"

void printPointer(struct Bar *number){
    printf("printPointer: %d\n", number->num);
}