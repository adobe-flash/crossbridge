/* APPLE LOCAL file radar 5732232 - blocks */
/* Testing byref syntax checking. */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

int main() {
	__block int a;
	__block int b;
	int x, y;
	^{ };
	^{}; 

	^{ int r; return x+y+r; }; 
	^{ int r; return x+y+r; }; 

	^{ int r; return x+y+r; 
           ^{
	   int r; return x+y; 
	    };
         };

	^{a = 1;}; 

	^{ 
	   if (a != b)
	     a = b = 100;
	 };



}

