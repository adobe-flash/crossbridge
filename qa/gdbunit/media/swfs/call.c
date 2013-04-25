#include <stdio.h>
#include <string.h>

typedef struct {
    int x;
    char y;
} Foo;

int returnInt(int a) {
    return a * a;
}

double returnDouble(double a) {
    return a * a;
}

float returnFloat(float a) {
    return a * a;
}

char returnChar(char a) {
    return a + 1;
}

Foo returnFoo(Foo a)  {
    a.x += 1;
    a.y += 1;
    return a;
}

// Now pointer equivalents


int* returnIntPtr(int a) {
    int* new = &a;
    return new;
}

double* returnDoublePtr(double a) {
    double* new = &a;
    return new;
}

float* returnFloatPtr(float a) {
    float* new = &a;
    return new;
}

char* returnCharPtr(char a) {
    char* new = &a;
    return new;
}

Foo* returnFooPtr(Foo a) {
    Foo* new = &a;
    return new;
}

void returnVoid() {
    int x = 9;
    printf("returnVoid function");
}

int main (int argc, const char * argv[]) {

    int s = 2;
    int t = 1;
    
    int i;
    for (i = 0; i < 8; i++){
        t *= 2;
    }

    printf("Testing gdb call\n");

    printf("returnInt=%d\n", returnInt(2));
    printf("returnDouble=%f\n", returnDouble(2.0));
    printf("returnFloat=%f\n", returnFloat(2.0));
    printf("returnChar=%c\n", returnChar('A'));

    Foo a;
    a.x = 4;
    a.y = 'A';

    Foo b = returnFoo(a);
    printf("returnFoo.x=%d\n", b.x);
    printf("returnFoo.y=%c\n", b.y);

    printf(" -- Pointer Equivalents --\n");

    int intPtr = 22;
    printf("returnIntPtr=%d\n", *(returnIntPtr(intPtr)));

    double doublePtr = 3.1415;
    printf("returnDoublePtr=%f\n", *(returnDoublePtr(doublePtr)));

    float floatPtr = 2.7183;
    printf("returnFloatPtr=%f\n", *(returnFloatPtr(floatPtr)));

    char charPtr = 'F';
    printf("returnCharPtr=%c\n", *(returnCharPtr(charPtr)));

    Foo fooPtr;
    fooPtr.x = 55;
    fooPtr.y = 'Z';
    printf("returnFooPtr=%c\n", (returnFooPtr(fooPtr))->y);

    returnVoid();

    return 0;
}

