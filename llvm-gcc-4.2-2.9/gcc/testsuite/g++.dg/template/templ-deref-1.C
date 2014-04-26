/* APPLE LOCAL file C++ */
/* It should be possible to be possible to explicitly call 'operator ...'
   functions with an implicit 'this', so long as the base class is not
   a dependent type.  For dependent base types, 'this->' or 'Base::'
   must be used.  */
/* Contributed by Ziemowit Laski <zlaski@apple.com> */
/* { dg-do compile } */

class Foo
{
	public:
	void read(int i);
};

void Func(Foo* inFoo);

void Foo::read(int i)
{
	i = i + 1;
}

class Baz
{
	public:
	Baz(Foo* in) : fFoo(in) {}
	operator Foo*()  { return fFoo; }
	Foo *foofoo(void) { return fFoo; }
	Foo* fFoo;
};

template <class T> class Boop {
	public:
	Foo *booboo(void) { return gFoo; } 
        operator void *(void) { return (void *)gFoo; }
	Foo *gFoo;
};

void Func(Foo* inFoo)
{
	inFoo->read(1);
}

template <class T> class Bar : public Baz, public Boop <T>
{
	public:
	Bar(T *inFoo) : Baz(inFoo) {}
	void CallMe() 
	{
		void *p;
		Func(this->foofoo());
		Func(this->operator Foo*());
		Func(this->booboo());
		p = Boop<T>::operator void*();
		Func(foofoo());
		Func(operator Foo*());
		Func(booboo());          /* { dg-error "must be available" } */
		p = operator void *();   /* { dg-error "must be available" } */
	}
};

int main (int argc, char * const argv[]) 
{
	Foo theFoo;
	Bar<Foo> theBar(&theFoo);
	theBar.CallMe(); 
}

/* { dg-error "allowing the use of an undeclared name is deprecated" "" { target *-*-* } 0 } */
