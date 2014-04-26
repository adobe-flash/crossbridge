/* APPLE LOCAL file radar 5805175 - blocks */
/* Test that pre/post incr/decr of copied-in variable cuases proper diagnostic. */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

void foo() {
    static int s_i = 10;
    int local;
    ^ { ++s_i; }; 
    ^ { local--; }; /* { dg-error "decrement of read-only variable" } */
    ++s_i;
}

