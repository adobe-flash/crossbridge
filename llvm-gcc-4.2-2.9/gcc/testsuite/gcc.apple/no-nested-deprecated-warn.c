/* APPLE LOCAL file radar 4746503 */
/* Don't issue 'deprecated' warning in a function which itself is deprecated. */

extern void foo() __attribute__((deprecated));
extern void bar() __attribute__((deprecated));

void foo() {}

void bar() {
    foo();  
}


void gorf() {
    foo();	/* { dg-warning "\\'foo\\' is deprecated" } */
    bar();	/* { dg-warning "\\'bar\\' is deprecated" } */
}


