/* APPLE LOCAL file radar 5023725 */
/* Test that appropriate diagnostic is issued for zerocost exception when 
   deployment target is less than 10.5 */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-m64 -mmacosx-version-min=10.4" } */
int main ()
{
	@try 
	  {
	    
	  }	/* { dg-error "Mac OS X version 10.5 or later is needed for zerocost-exceptions" } */
	@catch (id)
	  {
	  }
}
/* { dg-warning "Mac OS X version 10.5 or later is needed for use of the new objc abi" "" { target *-*-* } 0 } */
