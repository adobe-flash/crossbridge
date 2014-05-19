// A simple application with lots of nested function calls for step testing

#include <stdio.h>
#include <string.h>

int  nestedFunction1(int a);
int  nestedFunction2(int a);
void nestedFunction3(int a, int i, int sum);

int nestedFunction1(int a)
{
	int sum = 0;
	
	int i;
	for (i = 0; i < a; i++){
		sum += i;
		sum += nestedFunction2(i);
		nestedFunction3(a, i, sum);
	}
    return sum;
}

int nestedFunction2(int a)
{
    return a * a;
}

void nestedFunction3(int a, int i, int sum)
{
	// shows the value of a and i
	printf("a=%d, i=%d, sum=%d", a, i, sum);
}

int main (int argc, const char * argv[])
{
    int s = 1;
    int t = 2;
    
    printf("Hello, World!\n");
    
    int v = nestedFunction1(s);
    printf("v=%d", v);
    
    int u = nestedFunction1(t);    
    printf("u=%d", u);

	printf("again=%d, %d", nestedFunction1(s), nestedFunction1(t));

    return 0;
}

