/* APPLE LOCAL file radar 6275956 */
/* Use of explicit "this" inside a block. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.5" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

extern "C" void abort (void);

struct S {

	int ifield;

	int mem ()
	{
	    int (^p) (void) = ^ { return this->ifield; }; 
	    if (p() != ifield)
	     abort ();
	    return 0;
        }
	S (int val) { ifield = val; }

};

int main()
{
	S s(123);

	return s.mem();
}

	
