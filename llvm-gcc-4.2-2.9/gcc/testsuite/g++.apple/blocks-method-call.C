/* APPLE LOCAL file radar 6154598 */
/* Test that methods may be called from within a block
   declared inside a member function. */
/* { dg-options "-mmacosx-version-min=10.5" { target *-*-darwin* } } */
/* { dg-do run } */

extern "C" void abort(void);
struct SSSSSS
{
  void Index( void ) { if (_version != 2) abort(); }
  void func(void);
  int _version;
  SSSSSS() { _version = 2; }
};

void SSSSSS::func(void)
{
   void (^X)(void) = ^(void) { Index(); };
   X();
}

int main()
{
	SSSSSS s1;
	s1.func();
	return 0;
}


