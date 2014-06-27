/* This is the function for which it is desired to find more efficient
code.  It must have either one or two arguments, both int, and must
return a 32-bit int quantity.  It is declared in aha.h. */

int userfun(int x) {
// if (x > 0) return 1;                 // x > 0 predicate.
// else return 0;                       // Turn off div & divu.
                                        // Found a new formula for HD.

// if (x >= 32) return 0;
// return 1 << (unsigned)x;

// return 3*x + 1;

//   if (x >= 0) return x;                // Absolute value.
//   else return -x;

 return (x & 0xfffffffc) | ((x & 1) << 1) | ((x & 2) >> 1);
                                        // Swap rightmost 2 bits.
}
