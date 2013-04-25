/* APPLE LOCAL file radar 5732232 - blocks */
/* Testing byref syntax checking. */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

int main() {
	__block int x, y;
	^{ };

	^{ int r; return x+y+r; }; 

	^{ int r; return x+y+r; 
           ^{
	      int r; return x+y; 
	    };
         };

	/* Assigning to byref variables. */
	^{ x = 1;}; 

	^{ 
	   if (x != y)
	     x = y = 100;
	 };
}

