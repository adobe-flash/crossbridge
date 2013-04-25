/* LLVM LOCAL file */
/* { dg-do run } */
static int (*foo)(long int key);
static int bar(long int key)
{
    foo = 0;
    return key;
}
static int (*foo)(long int key) = bar;

int main() {
	foo(123);
	return 0;
}


