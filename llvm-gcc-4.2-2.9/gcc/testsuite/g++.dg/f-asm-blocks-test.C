/* APPLE LOCAL begin CW asm blocks */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */
class Foo
{
    public:
    explicit Foo(int i) { mFoo = i; }
    
    private:
    int mFoo;
};

class FooRange
{
    public:
    explicit FooRange(const Foo &start, const Foo &end) : mStart(start), mEnd(end)
    {
    }
    
    private:
    Foo mStart, mEnd;
};

void FooFunc(const FooRange &inRange) 
{
}

int main (int argc, char * const argv[]) {
    FooRange theRange( Foo( 0 ) , Foo( 0 ) ); // Change this to FooRange theRange = FooRange( Foo( 0 ) , Foo( 0 ) ); 
					      // and everything is good...
    FooFunc(theRange); 
}
/* APPLE LOCAL end CW asm blocks */
