/* APPLE LOCAL file radar 6310599 */
/* Check that internal fields do not conflict with user names. */
/* { dg-options "-fblocks" } */
/* { dg-do compile } */

int main()
{
	__block int flags;
	__block void *isa;
	
	^{ flags=1; isa = (void *)isa; };
	return 0;
}

