/* APPLE LOCAL file radar 5814025 */
/* Test that a one cannot assign to a 'const' block pointer variable. */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

#import <stdio.h>

void foo(void) { printf("I'm in foo\n"); }
void bar(void) { printf("I'm in bar\n"); }

int main(char *argc, char *argv[]) {
    void (^const  blockA)(void) = ^ { printf("hello\n"); };
    blockA = ^ { printf("world\n"); }; /* { dg-error "assignment of read-only variable" } */
    return 0;
}


