/* APPLE LOCAL file radar 6116917 */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -ObjC -Os -Wall -Wextra" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//#include <Block_private.h>

/* APPLE LOCAL begin radar 5847213 - radar 6329245 */
struct Block_basic {
    void *isa; // initialized to &_NSConcreteStackBlock or &_NSConcreteGlobalBlock
    int Block_flags;
    int reserved;
    void (*Block_invoke)(void *);

    struct Block_descriptor_1 {
        unsigned long int reserved;     // NULL
        unsigned long int Block_size;  // sizeof(struct Block_literal_1)

        // optional helper functions
        void (*Block_copy)(void *dst, void *src);
        void (*Block_dispose)(void *src);
    } *descriptor;

    // imported variables
};
/* APPLE LOCAL end radar 5847213 - radar 6329245 */

void
func(void (^b)(void))
{
	b();
}

void
func2(void (^b)(void))
{
	b();
}

extern char **environ;

int
main(int argc __attribute__((unused)), char *argv[])
{
	struct Block_basic *bb;
	long bbi_addr, bb_addr;

	void (^stage1)(void) = ^{
		void (^stage2)(void) = ^{
			/* trick the compiler into slirping argc/argv into this Block */
			if (environ == argv) {
				fprintf(stdout, "You won the lottery! argv == environ\n");
			}
		};

		func2(stage2);
	};

	bb = (void *)stage1;

	/* APPLE LOCAL radar 5847213 - radar 6329245 */
	bbi_addr = (long)bb->Block_invoke;
	bb_addr = (long)bb;

	if (labs(bbi_addr - bb_addr) > (64 * 1024)) {
		func(stage1);
		exit(EXIT_SUCCESS);
	} else {
		fprintf(stderr, "Blocks generated code on the stack! Block_copy() is not safe!\n");
		exit(EXIT_FAILURE);
	}
}
