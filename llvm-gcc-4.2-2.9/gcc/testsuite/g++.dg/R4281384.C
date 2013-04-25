/* APPLE LOCAL file Radar 4281384 */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -feliminate-unused-debug-symbols" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

class A {
public:
    int a;
    int aa;

    A()
    {
        a=1;
        aa=2;
    }
    int afoo();
    int foo();
    
};

int A::afoo() {
    return 1;
}

int A::foo()
{
    return 7;
}

void marker1()
{
}

int main(void)
{

    A a_instance;
    marker1(); // marker1-returns-here
    a_instance.a = 20; // marker1-returns-here
    a_instance.aa = 21;
    return 0;
    
}
/* { dg-final { scan-assembler ".stabs.*int:" } } */
    
    
